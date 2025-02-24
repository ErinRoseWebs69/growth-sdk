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
	string_t m_iPartnerName;
	CNetworkHandle(CPointPortal, m_hPartner);
};

LINK_ENTITY_TO_CLASS(point_portal, CPointPortal);

BEGIN_DATADESC(CPointPortal)
DEFINE_KEYFIELD(m_iPartnerName, FIELD_STRING, "partner"),
DEFINE_FIELD(m_hPartner, FIELD_EHANDLE),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPointPortal, DT_PointPortal)
SendPropEHandle(SENDINFO(m_hPartner)),
END_SEND_TABLE()

CPointPortal::CPointPortal()
{

}

void CPointPortal::Activate(void)
{
	BaseClass::Activate();

	CBaseEntity* entity = gEntList.FindEntityByName(NULL, m_iPartnerName);
	if (CPointPortal* mirror = dynamic_cast<CPointPortal*>(entity)) {
		m_hPartner.Set(mirror);
	}
}
