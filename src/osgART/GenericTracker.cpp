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
		trackerId(GenericTracker::trackerNum++)
	{
	}

	GenericTracker::~GenericTracker() 
	{
		// hse25: delete markers
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
	GenericTracker::setImageRaw(unsigned char* grabbed_image,
		PixelFormatType format)
	{
		m_imageptr = grabbed_image;
		m_imageptr_format = format;
	}

	void 
	GenericTracker::setImage(GenericVideo* video)
	{
		if (video) 
		{
			this->setImageRaw(video->getImageRaw(),
			video->getPixelFormat(false));

		} else 
		{
			osg::notify(osg::WARN) << "Warning: osgART::GenericTracker::setImage(video) "
				"should receive a valid video" << std::endl;
		}
	}

	

	/* virtual */
	const double* 
	GenericTracker::getProjectionMatrix() const 
	{
		return m_projectionMatrix;
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
		if (m_tracker.valid()) 
		{
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

		if (m_tracker.valid()) 
		{
			_projection->setMatrix(osg::Matrix(m_tracker->getProjectionMatrix()));
		}

		return _projection;
	}

};
