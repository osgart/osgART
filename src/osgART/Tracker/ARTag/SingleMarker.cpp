#include "SingleMarker"

#include <artag_rev2.h>
#include "osgART\Utils"

namespace osgART {

	SingleMarker::SingleMarker() : Marker(),
		m_patt_artag_code(-1),
		m_patt_id(-1)
	{
		//m_fields["confidence"] = new TypedField<double>(&m_confidence);
		m_fields["patt_id"] = new TypedField<int>(&m_patt_id);
		m_fields["patt_code"] = new TypedField<int>(&m_patt_artag_code);
	}

	SingleMarker::~SingleMarker()
	{
		// jcl64: Free the pattern
		//if (m_patt_id >= 0) arFreePatt(m_patt_id);
		//no function to free the pattern..???
		m_patt_id = -1;
	}

	Marker::MarkerType SingleMarker::getType() const
	{
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile)//, double width, double center[2])
	{
		if (m_patt_id >= 0) return (false);
		
		//convert the string into an int...
		sscanf(pattFile.c_str(), "%d", &m_patt_artag_code);
		if(m_patt_artag_code <0)
			return false;
		//==============================

		m_patt_id = artag_associate_marker(m_patt_artag_code);
		if (m_patt_id < 0) return false;
		/*patt_width = width;
		patt_center[0] = center[0];
		patt_center[1] = center[1];
		*/
		char Buff[16];
		itoa(m_patt_artag_code, Buff, 10);
		setName(Buff);
		setActive(false);
		return true;
	}

	void SingleMarker::update()//ARMarkerInfo* markerInfo)
	{
		if (!artag_is_object_found(m_patt_id))
		{
			m_valid = false;
			m_seen = false;
		}
		else
		{	
			m_valid = true;
			float modelView[16];
			artag_get_object_opengl_matrix(m_patt_id, modelView);
			osg::Matrix tmp(modelView);
			float PosScale = 10;
			float SizeScale = 0.25;
			
			osgART::PrintMatrix("ARTag simple marker Matrix :", tmp);
			//tmp = tmp * tmp.scale(Scale,Scale,Scale);
			osg::Vec3 Pos (tmp.getTrans());
			//osg::Quat Rot (tmp.getRotate());
			//tmp = tmp.identity();
			tmp = tmp * osg::Matrixd::scale(SizeScale,SizeScale,SizeScale);

			Pos *= PosScale;
		//	Pos._v[0] = 0;
		//	Pos._v[1] = 0;
			
			tmp.setTrans(Pos);
			//tmp.setRotate(Rot);

			osgART::PrintMatrix("ARTag simple marker Matrix 2---:", tmp);
			updateTransform(tmp);	
		}
	}

	void SingleMarker::setActive(bool a)
	{
		m_active = a;
		//no function to activate/desactivate the marker
		/*if (m_active) arActivatePatt(m_patt_id);
		else arDeactivatePatt(m_patt_id);
		*/
	}

	int SingleMarker::getPatternID()
	{
		return m_patt_id;
	}
	int SingleMarker::getARTagCode(void)
	{
		return m_patt_artag_code;
	}

/*
	double SingleMarker::getPatternWidth()
	{
		return patt_width;
	}
	*/	
/*	double* SingleMarker::getPatternCenter()
	{
		return patt_center;
	}
	*/
};
