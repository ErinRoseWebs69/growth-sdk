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
	CNetworkVar(int, m_iHalfWidth);
	CNetworkVar(int, m_iHalfHeight);
};

LINK_ENTITY_TO_CLASS(point_portal, CPointPortal);

BEGIN_DATADESC(CPointPortal)
DEFINE_KEYFIELD(m_sPartnerName, FIELD_STRING, "partner"),
DEFINE_FIELD(m_hPartner, FIELD_EHANDLE),
DEFINE_KEYFIELD(m_iHalfWidth, FIELD_INTEGER, "halfwidth"),
DEFINE_KEYFIELD(m_iHalfHeight, FIELD_INTEGER, "halfheight"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPointPortal, DT_PointPortal)
SendPropEHandle(SENDINFO(m_hPartner)),
SendPropInt(SENDINFO(m_iHalfWidth)),
SendPropInt(SENDINFO(m_iHalfHeight))
END_SEND_TABLE()

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
}
