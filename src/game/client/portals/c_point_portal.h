#ifndef C_POINTPORTAL_H
#define C_POINTPORTAL_H

#include "c_baseentity.h"

class C_PointPortal : public C_BaseEntity
{
public:
	DECLARE_CLASS(C_PointPortal, C_BaseEntity);
	DECLARE_CLIENTCLASS();
public:
	C_PointPortal();
	~C_PointPortal();

	bool HasPartner();
	C_PointPortal* GetPartner();

	virtual void SpawnClientEntity(void);

	void DrawStencil();

private:
	EHANDLE m_hPartner;
};

#endif