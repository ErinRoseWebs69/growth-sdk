#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <collisionutils.h>

class CPointPortal : public CBaseEntity 
{
public:
	DECLARE_CLASS(CPointPortal, CBaseEntity);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPointPortal();

	int UpdateTransmitState()
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}

	virtual void Activate(void);
	virtual void Touch(CBaseEntity* other);
	virtual void Spawn();

	virtual bool TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr);
public:
	string_t m_sPartnerName;
	CNetworkHandle(CPointPortal, m_hPartner);
	CNetworkVar(float, m_fHalfWidth);
	CNetworkVar(float, m_fHalfHeight);
};

BEGIN_DATADESC(CPointPortal)
DEFINE_KEYFIELD(m_sPartnerName, FIELD_STRING, "partner"),
//DEFINE_FIELD(m_hPartner, FIELD_EHANDLE),
DEFINE_KEYFIELD(m_fHalfWidth, FIELD_FLOAT, "halfwidth"),
DEFINE_KEYFIELD(m_fHalfHeight, FIELD_FLOAT, "halfheight"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPointPortal, DT_PointPortal)
SendPropEHandle(SENDINFO(m_hPartner)),
SendPropFloat(SENDINFO(m_fHalfWidth)),
SendPropFloat(SENDINFO(m_fHalfHeight))
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(point_portal, CPointPortal);

CPointPortal::CPointPortal()
{
}

void CPointPortal::Activate(void)
{
	BaseClass::Activate();

	CBaseEntity* entity = gEntList.FindEntityByName(NULL, m_sPartnerName);
	if (CPointPortal* mirror = dynamic_cast<CPointPortal*>(entity)) {
		m_hPartner.Set(mirror);
	}

	this->DispatchUpdateTransmitState();
}

void CPointPortal::Spawn() {
	BaseClass::Spawn();

	SetSolid(SOLID_OBB);
	SetSolidFlags(FSOLID_TRIGGER | FSOLID_NOT_SOLID | FSOLID_CUSTOMBOXTEST | FSOLID_CUSTOMRAYTEST);

	this->SetSize({-0.5, -this->m_fHalfWidth, -this->m_fHalfHeight }, { 0.5, this->m_fHalfWidth, this->m_fHalfHeight });
}

bool CPointPortal::TestCollision(const Ray_t& ray, unsigned int fContentsMask, trace_t& tr)
{
	return IntersectRayWithOBB(ray, this->EntityToWorldTransform(), { -0.5, -this->m_fHalfWidth, -this->m_fHalfHeight }, { 0.5, this->m_fHalfWidth, this->m_fHalfHeight }, 0.0f, &tr);
}

void CPointPortal::Touch(CBaseEntity* pOther)
{
	BaseClass::Touch(pOther);

	if (CBasePlayer* player = dynamic_cast<CBasePlayer*>(pOther)) {
		UTIL_ShowMessage("Touched!!", player);
		CPointPortal* other = this->m_hPartner.Get();

		
	}
}