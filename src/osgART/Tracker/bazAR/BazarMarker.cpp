#include "BazarMarker"

#include <AR/gsub_lite.h>
#include <osg/io_utils>

namespace osgART {

	BazARMarker::BazARMarker() : Marker() 
	{			
	}


	BazARMarker::~BazARMarker() 
	{	
	}

	Marker::MarkerType BazARMarker::getType() const 
	{
		return Marker::ART_NFT;
	}

	bool BazARMarker::initialise() 
	{
		return true;
	}

	void BazARMarker::update(CvMat* matCameraRT4_4) 
	{
	
		if (matCameraRT4_4 == NULL) 
		{
			m_valid = false;
		} 
		else 
		{
		
			double modelView[16];
		 	double patt_trans[3][4];

			// take 4x3 submatrix from matCameraRT4_4 and transpose to 3x4 patt_trans
			for(int i=0; i<3; i++){
				for (int j=0;j<4;j++){
				patt_trans[i][j]=((double*)(matCameraRT4_4->data.ptr + matCameraRT4_4->step*j))[i];
				}
			}
			// apply changes to match gl's right handed coordinate system -> modelView
			arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);

			m_valid = true;
			updateTransform(tmp,true);
		}
	}

	void BazARMarker::setActive(bool a) 
	{
		m_active = a;
	}
};