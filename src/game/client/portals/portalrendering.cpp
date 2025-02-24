
#include "cbase.h"
#include "portalrendering.h"
#include "c_point_portal.h"
#include "c_stencil.h"
#include "viewrender.h"

#include "engine/ivdebugoverlay.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar r_max_stencil_recursion("r_max_stencil_recursion", "1", FCVAR_CLIENTDLL, "(BROKEN) Shows portals through portals, if it worked properly");
static ConVar r_debug_portals("r_debug_portals", "0", FCVAR_CLIENTDLL, "Shows debug information about portals");

static PortalRendering s_PortalRendering;
PortalRendering* g_pPortalRendering = &s_PortalRendering;

void PortalRendering::Render(CViewRender* view)
{
	if (m_vPortals.Size() > 0 && m_iRecursionLevel < r_max_stencil_recursion.GetInt()) {
		CMatRenderContextPtr pRenderContext(materials);
		pRenderContext->Flush(true);

		const CViewSetup* viewSetup = view->GetViewSetup();
		CViewSetup viewBackup;
		memcpy(&viewBackup, viewSetup, sizeof(CViewSetup));

		VMatrix rotation;
		rotation.Identity();
		rotation[0][0] = -1.0f;
		rotation[1][1] = -1.0f;

		C_BasePlayer* localPlayer = C_BasePlayer::GetLocalPlayer();
		CSimpleWorldView* pClientView = new CSimpleWorldView(view);

		if (m_iRecursionLevel == 0)
			g_pStencilTool->SetupInitialStencilRendering(pRenderContext, m_iRecursionLevel);
		for (int i = 0; i < m_vPortals.Size(); ++i) {
			//const float forwardOffset = 0.1;
			C_PointPortal* window = m_vPortals[i];
			if (!window->HasPartner()) continue;

			Vector myPos = window->GetAbsOrigin();
			QAngle myAngles = window->GetAbsAngles();
			Vector myFwd, myRht, myUp;
			AngleVectors(myAngles, &myFwd, &myRht, &myUp);

			Vector rExt = myPos + myRht * (window->m_fHalfWidth - 0.1);
			Vector lExt = myPos - myRht * (window->m_fHalfWidth - 0.1);
			Vector uExt = myPos + myUp * (window->m_fHalfHeight - 0.1);
			Vector dExt = myPos - myUp * (window->m_fHalfHeight - 0.1);

			if (!localPlayer->IsInFieldOfView(myPos) || !localPlayer->ComputeLOS(localPlayer->EyePosition(), myPos)) { // if the center is visible then its guaranteed the portal is visible
				if (!localPlayer->IsInFieldOfView(rExt) &&
					!localPlayer->IsInFieldOfView(lExt) &&
					!localPlayer->IsInFieldOfView(uExt) &&
					!localPlayer->IsInFieldOfView(dExt)) continue; // if none of the points are even in the field of view, why bother checking if its occluded?
				if (!localPlayer->ComputeLOS(localPlayer->EyePosition(), lExt) &&
					!localPlayer->ComputeLOS(localPlayer->EyePosition(), rExt) &&
					!localPlayer->ComputeLOS(localPlayer->EyePosition(), uExt) &&
					!localPlayer->ComputeLOS(localPlayer->EyePosition(), dExt)) continue; // this is an excuse not to do a depth hack that I can't figure out how to do
			}

			g_pStencilTool->SetStencilReferenceValue(pRenderContext, m_iRecursionLevel);
			window->DrawStencil(false);
			m_iRecursionLevel += 1;
			g_pStencilTool->ClearDepthBuffer(pRenderContext, m_iRecursionLevel);

			CViewSetup windowView = *viewSetup;

			VMatrix myModel(
				myFwd,
				-myRht,
				myUp,
				myPos
			);

			Vector friendPos = window->GetPartner()->GetAbsOrigin();
			QAngle friendAngles = window->GetPartner()->GetAbsAngles();
			Vector friendFwd, friendRht, friendUp;
			AngleVectors(friendAngles, &friendFwd, &friendRht, &friendUp);
			VMatrix friendModel(
				friendFwd,
				-friendRht,
				friendUp,
				friendPos
			);

			VMatrix viewToPortalSpaceMat = (VMatrix(friendModel) * rotation * VMatrix(myModel).InverseTR());

			Vector ptCameraOrigin = viewToPortalSpaceMat * viewSetup->origin;

			QAngle angles = TransformAnglesToWorldSpace(viewSetup->angles, viewToPortalSpaceMat.As3x4());
			windowView.zNear = 1.0f;
			windowView.origin = ptCameraOrigin;
			windowView.angles = angles;

			// while this is technically a 180 degree rotation about up, it doesn't produce the right result
			// I didn't pay attention in linear algebra enough to understand why
			// all I know is manually making a matrix that inverts x and y produces the correct result without more complex matrix calculations in the render thread.
			//MatrixBuildRotationAboutAxis(rotation, myUp, 180.0f);

			if (r_debug_portals.GetBool()) {
				debugoverlay->AddTextOverlay(window->GetAbsOrigin(), 0.0f, "window %d", i);
				debugoverlay->AddTextOverlay(ptCameraOrigin, 0.0f, "ptCameraOrigin %d", i);
			}

			float clipPlane[4];
			clipPlane[0] = friendFwd.x;
			clipPlane[1] = friendFwd.y;
			clipPlane[2] = friendFwd.z;
			clipPlane[3] = friendFwd.Dot(window->GetPartner()->GetAbsOrigin() - (friendFwd * 0.5f));

			pRenderContext->PushCustomClipPlane(clipPlane);

			render->Push3DView(windowView, 0, nullptr, view->GetFrustum());

			render->OverrideViewFrustum(view->GetFrustum());

			VisibleFogVolumeInfo_t fogInfo;
			render->GetVisibleFogVolume(windowView.origin, &fogInfo);

			WaterRenderInfo_t waterInfo;
			view->DetermineWaterRenderInfo(fogInfo, waterInfo);

			
			pClientView->Setup(windowView, VIEW_CLEAR_OBEY_STENCIL, true, fogInfo, waterInfo, nullptr);
			view->AddViewToScene(pClientView);

			render->PopView(view->GetFrustum());

			memcpy((void*)viewSetup, &viewBackup, sizeof(CViewSetup));
			g_pStencilTool->SetupInitialStencilRendering(pRenderContext, m_iRecursionLevel); // basically just reset everything
			pRenderContext->SetStencilCompareFunction(STENCILCOMPARISONFUNCTION_EQUAL);
			pRenderContext->SetStencilPassOperation(STENCILOPERATION_KEEP);
			g_pStencilTool->RestoreStencilMask(pRenderContext, m_iRecursionLevel);

			pRenderContext->PopCustomClipPlane();
			m_iRecursionLevel -= 1;
		}
		pRenderContext->Flush(true);
		SafeRelease(pClientView);
	}
}

void PortalRendering::Add(C_PointPortal* portal)
{
	m_vPortals.AddToTail(portal);
}

void PortalRendering::Remove(C_PointPortal* portal) 
{
	m_vPortals.FindAndRemove(portal);
}
