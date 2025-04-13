#include "cbase.h"
#include "c_point_portal.h"
#include "portalrendering.h"
#include "view_scene.h"

LINK_ENTITY_TO_CLASS(point_portal, C_PointPortal);

IMPLEMENT_CLIENTCLASS_DT(C_PointPortal, DT_PointPortal, CPointPortal)
RecvPropEHandle(RECVINFO(m_hPartner)),
RecvPropFloat(RECVINFO(m_fHalfWidth)),
RecvPropFloat(RECVINFO(m_fHalfHeight)),
END_RECV_TABLE()

C_PointPortal::C_PointPortal()
{
	g_pPortalRendering->Add(this);
	this->SetRenderMode(kRenderNormal);
	Msg("[PORTAL] %s: Created", this->GetDebugName());
}

C_PointPortal::~C_PointPortal()
{
	g_pPortalRendering->Remove(this);
}

bool C_PointPortal::HasPartner()
{
	return m_hPartner;
}

C_PointPortal* C_PointPortal::GetPartner()
{
	return (C_PointPortal*)m_hPartner.Get();
}

void C_PointPortal::DrawStencil(bool fixDepth)
{
	float forwardOffset = fixDepth ? 0.2 : 0.1;
	const IMaterial* pMaterial = fixDepth ? materials->FindMaterial("writez", "") : materials->FindMaterial("", "", false); // this material needs to exist
	// TODO: this needs to get a material that ONLY writes depth and not color.
	// eventually you should be able to put a mask material on this, make real looking portals

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->Bind((IMaterial*)pMaterial, GetClientRenderable());

	// This can depend on the Bind command above, so keep this after!
	UpdateFrontBufferTexturesForMaterial((IMaterial*)pMaterial);

	pRenderContext->MatrixMode(MATERIAL_MODEL); //just in case
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	Vector m_vForward, m_vRight, m_vUp;
	this->GetVectors(&m_vForward, &m_vRight, &m_vUp);

	Vector ptCenter = this->GetAbsOrigin() + m_vForward * forwardOffset;

	Vector verts[4];
	verts[0] = ptCenter + (m_vRight * m_fHalfWidth) - (m_vUp * m_fHalfHeight);
	verts[1] = ptCenter + (m_vRight * m_fHalfWidth) + (m_vUp * m_fHalfHeight);
	verts[2] = ptCenter - (m_vRight * m_fHalfWidth) - (m_vUp * m_fHalfHeight);
	verts[3] = ptCenter - (m_vRight * m_fHalfWidth) + (m_vUp * m_fHalfHeight);

	CMeshBuilder meshBuilder;
	IMesh* pMesh = pRenderContext->GetDynamicMesh(false);
	meshBuilder.Begin(pMesh, MATERIAL_TRIANGLE_STRIP, 2);

	meshBuilder.Position3fv(&verts[3].x);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(&verts[2].x);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(&verts[1].x);
	meshBuilder.AdvanceVertex();

	meshBuilder.Position3fv(&verts[0].x);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();
	pMesh->Draw();

	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PopMatrix();
}
