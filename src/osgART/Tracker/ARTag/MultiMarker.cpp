#include "MultiMarker"
#include "osgART/Utils"
namespace osgART {

	MultiMarker::MultiMarker() : Marker(),
		m_multi_patt_id(-1)
	{
	}

	MultiMarker::~MultiMarker() {   
		// jcl64: Free the multimarker
//		if (m_multi) arMultiFreeConfig(m_multi);
		if (m_multi_patt_id>0)
			m_multi_patt_id = -1;
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile, const std::string& multiName) {
		static MultiFileID = load_array_file((char*)multiFile.c_str());
		osg::notify() << "MultiMarker, looking for multi : " << multiName << " in file " << multiFile<< std::endl;
		
		// Check if multifile exists!!!
		if (MultiFileID==-1)
			osg::notify(osg::WARN) << "ARTag can not find the multimarker file!" << std::endl;

		m_multi_patt_id = artag_associate_array((char*)multiName.c_str());
		if (m_multi_patt_id == -1) return false;

		setName(multiFile);
		setActive(false);
		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;
/*		if (m_active) arMultiActivate(m_multi);
		else arMultiDeactivate(m_multi);
*/	}

	int MultiMarker::getMultiPatternID()
	{
		return m_multi_patt_id;
	}

	void 
	MultiMarker::update()//ARMarkerInfo* markerInfo, int markerCount) 
	{
		if (!artag_is_object_found(m_multi_patt_id))
		{
			m_valid = false;
			m_seen = false;
		}
		else
		{	
			m_valid = true;
			float modelView[16];
			artag_get_object_opengl_matrix(m_multi_patt_id, modelView);
			osg::Matrix tmp(modelView);
			osgART::PrintMatrix("ARTag multi marker Matrix :", tmp);
		/*	float Scale = 0.5;
			tmp.scale(osg::Vec3(Scale, Scale, Scale));

			osg::Vec3 Pos (tmp.getTrans());
			//osg::Vec3 Rot (tmp.getRotate().asVec3());
			Pos.set(-149.,-53.,-873.);
			tmp = tmp.identity();
			//Pos *= Scale;
		//	Pos._v[0] = 0;
		//	Pos._v[1] = 0;
		//	tmp.setRotate(Rot);
			tmp.setTrans(Pos);
			osgART::PrintMatrix("ARTag multi marker Matrix 2:", tmp);
	*/		updateTransform(tmp);		
		}

/*		m_valid = (arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
		if (m_valid) {
			double modelView[16];
			arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			osg::Matrix tmp(modelView);
			updateTransform(tmp);
		} else {
			m_seen = false;
		}
*/	}
	
};
