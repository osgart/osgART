#include "MultiMarker"



#include <AR/gsub_lite.h>
namespace osgART {

	MultiMarker::MultiMarker() : Marker(), 
		m_smoothAs(new SmoothAs)
	{
		m_smoothAs->setMatrixBufferSize(10);
	}

	MultiMarker::~MultiMarker() {   
		// jcl64: Free the multimarker
		if (m_multi) arMultiFreeConfig(m_multi);
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile) {

		// Check if multifile exists!!!
		m_multi = arMultiReadConfigFile(multiFile.c_str());
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;
		
		if (m_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
	}

	void 
	MultiMarker::update(ARMarkerInfo* markerInfo, int markerCount) 
	{
		m_valid = (arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
      //  m_valid=1;
		if (m_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			filteredUpdate(tmp);
			//updateTransform(tmp);
		} else {
			m_seen = false;
		}
	}
	
	void MultiMarker::filteredUpdate(osg::Matrixd& matrix)
	{
			// do filtering here
			m_smoothAs->putTransform(matrix);
			matrix.set(*m_smoothAs->getTransform());
			updateTransform(matrix);
	}


};
