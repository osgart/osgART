/*
 *	osgART/GenericTracker
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include "osgART/GenericTracker"
#include "osgART/Marker"
#include "osgART/GenericVideo"

#include <osg/Notify>

#include <iostream>


namespace osgART {

	int GenericTracker::trackerNum = 0;


	GenericTracker::GenericTracker() 
		: osg::Referenced(),
		m_imageptr(0L),
		m_enable(true),
		trackerId(GenericTracker::trackerNum++)
	{
		m_fields["name"]	= new TypedField<std::string>(&m_name);
		m_fields["version"]	= new TypedField<std::string>(&m_version);
		/* gcc is choking on this declaration with "no matching function call"
		m_fields["enable"]	= new CallbackField<GenericTracker,bool>(this,
			GenericTracker::getEnable,
			GenericTracker::setEnable); */
	}

	GenericTracker::~GenericTracker() 
	{
		// Markers are associated with a specific tracker instance,
		// so will be deleted when the tracker is deleted.
		m_markerlist.clear();		
	}

	int
	GenericTracker::getId()
	{
		return trackerId;
	}

	Marker* 
	GenericTracker::getMarker(int id) 
	{
		Marker *_m = (Marker*)0L;

		try 
		{
			_m = m_markerlist[id].get();

		} catch(...) 
		{

			osg::notify(osg::WARN) << "No Marker with ID: " << id << std::endl;
		}

		// return the Marker
		return _m;
	}

	unsigned int 
	GenericTracker::getMarkerCount() const 
	{
		return (unsigned int)m_markerlist.size();
	}

	/*virtual*/ 
	void 
	GenericTracker::createUndistortedMesh(int,int,
		float,float,osg::Geometry&)
	{
		osg::notify(osg::WARN) << "Warning: osgART::GenericTracker::createUndistortedMesh(): "
			"Empty implementation called!" << std::endl;
	}


	/*virtual*/ 
	void 
	GenericTracker::setImageRaw(unsigned char *image,
		PixelFormatType format)
	{
		m_imageptr = image;
		m_imageptr_format = format;
	}

	/*virtual*/ 
	void 
	GenericTracker::setImage(GenericVideo* video)
	{
		if (video) {
			this->setImageRaw(video->getImageRaw(),
			video->getPixelFormat(false));
		} else {
			osg::notify(osg::WARN) << "Warning: invalid video object supplied to osgART::GenericTracker::setImage(video)." << std::endl;
		}
	}

	/*virtual*/
	void 
	GenericTracker::update()
	{
	}

	/* virtual */
	const double* 
	GenericTracker::getProjectionMatrix() const 
	{
		return m_projectionMatrix;
	}

	/* virtual */
	void	GenericTracker::setEnable(const bool & _enable)
	{
		m_enable = _enable;
	}

	/* virtual */
	bool	GenericTracker::getEnable() const
	{
		return m_enable;
	}

	std::string GenericTracker::getLabel() const 
	{
		std::string Result = m_name;
		Result += "-";
		Result += m_version;
		return Result;
	}
	
	// ------------------------------------------------

	TrackerContainer::TrackerContainer(GenericTracker* tracker) : GenericTracker(), m_tracker(tracker) 
	{
	}
	
	void 
	TrackerContainer::createUndistortedMesh(int a,int b,
		float c,float d,osg::Geometry& e)
	{
		if (m_tracker.valid()) m_tracker->createUndistortedMesh(a,b,c,d,e);
	}

	void
	TrackerContainer::update()
	{
		if (m_tracker.valid()) {
			m_tracker->setImage(m_video.get());
			m_tracker->update();
		}
	}

	TrackerContainer::~TrackerContainer()
	{
	}

	/*virtual*/ 
	bool 
	TrackerContainer::init(int xsize, int ysize, 
			const std::string& pattlist_name/*="Data/markers_list.dat"*/,
			const std::string& camera_name/*="Data/camera_para.dat"*/) 
	{

		bool _ret = false;

		if (m_tracker.valid()) {

			_ret = m_tracker->init(xsize,ysize,pattlist_name,camera_name);

			this->m_markerlist = m_tracker->m_markerlist;
			this->m_imageptr = m_tracker->m_imageptr;

			memcpy(m_projectionMatrix,m_tracker->m_projectionMatrix,
				sizeof(double) * 16);
		}

		return _ret; 
	}


	/*virtual*/ 
	bool 
	TrackerContainer::init(GenericVideo* video, 
			const std::string& pattlist_name/*="Data/markers_list.dat"*/,
			const std::string& camera_name/*="Data/camera_para.dat"*/) 
	{
		m_video = video;

		return (m_tracker.valid() && m_video.valid()) ? 
			this->init(video->getWidth(),video->getHeight(),pattlist_name,camera_name) : false;
	}


	osg::Projection*
	TrackerContainer::createProjection() const
	{
		osg::Projection *_projection = new osg::Projection();

		if (m_tracker.valid()) {
			_projection->setMatrix(osg::Matrix(m_tracker->getProjectionMatrix()));
		}

		return _projection;
	}

};
