#include "MultiMarker"

#include <iostream>

namespace osgART {

MultiMarker::MultiMarker():Marker()
{

}

MultiMarker::~MultiMarker(void)
{
    
}

/* virtual */
Marker::MarkerType MultiMarker::getType() const
{
	return Marker::ART_MULTI;
}

bool 
MultiMarker::initialise(const std::string& multiFile) 
{
	m_multi = arMultiReadConfigFile((char*)multiFile.c_str());
	m_multi->prevF=false;//don't user history
	return (m_multi != NULL);
}

void MultiMarker::setActive(bool a) {
	m_active = a;
	
	//TO FIX HERE
}

void 
MultiMarker::update(AR3DHandle *ar3DHandle,ARMarkerInfo* markerInfo, int markerCount) 
{
	std::cerr<<"to update.."<<std::endl;
	for (int i=0;i<m_multi->marker_num;i++) {
		m_multi->marker[i].visible=-1;
	}
	//arGetTransMatMultiSquare(ar3DHandle,markerInfo, markerCount, m_multi);

	for (int i=0;i<m_multi->marker_num;i++) {
		m_valid |= (m_multi->marker[i].visible==-1?false:true);
	}
	GLdouble modelView[16];
	arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
	osg::Matrix tmp(modelView);
	updateTransform(tmp);
}
};