#include "SingleMarker"

typedef int AR_PIXEL_FORMAT;
#include <AR/gsub_lite.h>
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
	//std::cerr<<"DEBUG PattHandle="<<arPattHandle<<"patt name="<<pattFile.c_str()<<std::endl;
	patt_id = arPattLoad(arPattHandle,(char*)pattFile.c_str());
	//std::cerr<<"DEBUG PattHandle="<<arPattHandle<<"patt name="<<pattFile.c_str()<<" id="<<patt_id<<std::endl;
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
	
	if (markerInfo == NULL) 
	{
		m_valid = false;
	} 
	else 
	{
		// get transform matrix
		double err = 0;
		//err = arGetTransMatSquare(ar3DHandle,&(markerInfo[patt_id]), patt_width, patt_trans);
		err = arGetTransMatSquare(ar3DHandle,markerInfo, patt_width, patt_trans);
		if (err > 10.0){
			m_valid = false;
			return;
		}

		m_valid = true;

#ifndef NDEBUG
		for (int i=0;i<3;i++) {
			for (int j=0;j<4;j++) {
				std::cerr<<patt_trans[i][j]<<" ";
			}
			std::cerr<<std::endl;
		}
#endif
		
		// convert to osg format and update
		double modelView[16];
		arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
		osg::Matrix tmp(modelView);
		updateTransform(tmp);	
	}
	
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