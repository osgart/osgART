#include "SingleMarker"

typedef int AR_PIXEL_FORMAT;
#include <AR/AR/gsub_lite.h>
#include <iostream>

namespace osgART {

SingleMarker::SingleMarker():Marker()
{

}

SingleMarker::~SingleMarker(void)
{
    
}

Marker::MarkerType SingleMarker::getType() const {
	return Marker::ART_SINGLE;
}

bool SingleMarker::initialise(ARPattHandle *arPattHandle, const std::string& pattFile, double width, double center[2]) {
	patt_id = arPattLoad(arPattHandle,(char*)pattFile.c_str());
	if (patt_id < 0) return false;
	patt_width = width;
	patt_center[0] = center[0];
	patt_center[1] = center[1];
	return true;
}

void SingleMarker::setActive(bool a) {
	m_active = a;
	
	//HACK RAPH
	//TO FIX HERE
}

void SingleMarker::update(AR3DHandle *ar3DHandle, ARMarkerInfo* markerInfo) {
	
	if (markerInfo == NULL) {
		m_valid = false;
	} else {
		arGetTransMatSquare(ar3DHandle,&(markerInfo[patt_id]), patt_width, patt_trans);
//		arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
		m_valid = true;
#ifdef NDEBUG
		for (int i=0;i<3;i++) {
			for (int j=0;j<4;j++) {
				std::cerr<<patt_trans[i][j]<<" ";
			}
			std::cerr<<std::endl;
		}
#endif
	}
	double modelView[16];
	arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
	osg::Matrix tmp(modelView);
	updateTransform(tmp);	
}

int SingleMarker::getPatternID() {
	return patt_id;
}

double SingleMarker::getPatternWidth() {
	return patt_width;
}
	
double* SingleMarker::getPatternCenter() {
	return patt_center;
}
};