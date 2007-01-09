#include "SingleMarker"

#include <AR/gsub_lite.h>

#include "osgARTSmoothAs.h"

namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		patt_id(-1),
		m_smoothAs(new SmoothAs)
	{
		m_fields["confidence"] = new TypedField<double>(&m_confidence);
		m_smoothAs->setMatrixBufferSize(5);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		if (patt_id >= 0) arFreePatt(patt_id);
		patt_id = -1;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2])
	{
		if (patt_id >= 0) return (false);
		patt_id = arLoadPatt(pattFile.c_str());
		if (patt_id < 0) return false;
		patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		setName(pattFile);
		setActive(false);
		return true;
	}

	void SingleMarker::update(ARMarkerInfo* markerInfo)
	{
		if (markerInfo == NULL) {
			m_valid = false;
			m_seen = false;
		} else {
			m_valid = true;
			arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
			//arGetTransMat(markerInfo, patt_center, patt_width, patt_trans);
			m_confidence = markerInfo->cf;
			double modelView[16];
			arglCameraViewRH(patt_trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);

			// do filtering here
			filteredUpdate(tmp);
			//updateTransform(tmp); // direct update
		}
	}

	void SingleMarker::filteredUpdate(osg::Matrixd& matrix)
	{
			// do filtering here
			m_smoothAs->putTransform(matrix);
			matrix.set(*m_smoothAs->getTransform());
			updateTransform(matrix);
	}

	void SingleMarker::setActive(bool a)
	{
		m_active = a;
		
		if (m_active) arActivatePatt(patt_id);
		else arDeactivatePatt(patt_id);

	}

	int SingleMarker::getPatternID()
	{
		return patt_id;
	}

	double SingleMarker::getPatternWidth()
	{
		return patt_width;
	}
		
	double* SingleMarker::getPatternCenter()
	{
		return patt_center;
	}
};
