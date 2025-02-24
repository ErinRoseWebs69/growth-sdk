#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

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
