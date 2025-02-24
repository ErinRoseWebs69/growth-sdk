#ifndef PORTAL_RENDERING_H
#define PORTAL_RENDERING_H

#include "c_point_portal.h"
#include "viewrender.h"

class PortalRendering {
public:
	void Render(CViewRender* view);
	void Add(C_PointPortal* portal);
	void Remove(C_PointPortal* portal);
private:
	int m_iRecursionLevel;
	CUtlVector<C_PointPortal*> m_vPortals;
};

extern PortalRendering* g_pPortalRendering;
#endif