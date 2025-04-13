#ifndef C_POINTPORTAL_H
#define C_POINTPORTAL_H

#include "c_baseentity.h"

class C_PointPortal : public C_BaseEntity
{
	DECLARE_CLASS(C_PointPortal, C_BaseEntity);
	DECLARE_CLIENTCLASS();
public:
	C_PointPortal();
	~C_PointPortal();

	bool HasPartner();
	C_PointPortal* GetPartner();

	void DrawStencil(bool fixDepth);

private:
	EHANDLE m_hPartner;
public:
	float m_fHalfWidth, m_fHalfHeight;
};

#endif