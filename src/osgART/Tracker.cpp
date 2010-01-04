/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "osgART/Tracker"
#include "osgART/Marker"
#include "osgART/Video"

#include <osg/Notify>

#include <iostream>

namespace osgART {

	TrackerCallback::TrackerCallback(Tracker* tracker)
		: _tracker(tracker), _framenumber(-1)
	{
	}

	/* static */
	TrackerCallback* 
	TrackerCallback::addOrSet(osg::Node* node, osgART::Tracker* tracker)
	{
		TrackerCallback *callback = new TrackerCallback(tracker);

		node->getEventCallback() ? node->getEventCallback()->addNestedCallback(callback)
			: node->setEventCallback(callback);

		return callback;

	}

	void TrackerCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) 
	{			
		if (_tracker.valid()) 
		{
			if (nv->getFrameStamp()->getFrameNumber() != _framenumber) 
			{
				_tracker->update(nv);
				
				_framenumber = nv->getFrameStamp()->getFrameNumber();
			}
		}

		// must traverse the Node's subgraph            
		traverse(node,nv);
	}


	Tracker::Tracker() 
		: osg::Referenced(),
		m_enable(true),
		m_lastModifiedCount(0xFFFFF),
		_stats(new osg::Stats("tracker"))
	{
		m_fields["name"]	= new TypedField<std::string>(&m_name);
		m_fields["version"]	= new TypedField<std::string>(&m_version);
		m_fields["enable"]	= new CallbackField<Tracker,bool>(this,
			&Tracker::getEnable,
			&Tracker::setEnable);
	}

	Tracker::~Tracker() 
	{

		// 
		// Explicitly delete/unref all markers 
		//
		for( MarkerList::iterator mi = m_markerlist.begin();
			 mi != m_markerlist.end();
			 mi++)
		{
			(*mi) = 0L;
		}

		// Markers are associated with a specific tracker instance,
		// so will be deleted when the tracker is deleted.
		m_markerlist.clear();		
	}

	/*virtual*/
	Calibration* Tracker::getOrCreateCalibration()
	{
		return _calibration.get();
	}

	/* static */
	Tracker* Tracker::cast(osg::Referenced* instance)
	{
		return dynamic_cast<Tracker*>(instance);
	}

	/*virtual */
	Marker*
	Tracker::addMarker(const std::string& config)
	{
		osg::notify(osg::WARN) << "Method not implemented for this tracker!" << std::endl;

		return 0L;
	}

	/*virtual */
	void
	Tracker::removeMarker(Marker *marker)
	{
		// TODO: implement
	}



	Marker* 
	Tracker::getMarker(int id) 
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
	Tracker::getMarkerCount() const 
	{
		return (unsigned int)m_markerlist.size();
	}

	/*virtual*/ 
	void 
	Tracker::createUndistortedMesh(int,int,
		float,float,osg::Geometry&)
	{
		osg::notify(osg::WARN) << "Warning: osgART::Tracker::createUndistortedMesh(): "
			"Empty implementation called!" << std::endl;
	}

	/*virtual*/ 
	void 
	Tracker::setImage(osg::Image* image)
	{
		m_imagesource = image;
	}

	/*virtual*/ 
	osg::Image* Tracker::getImage() 
	{
		return m_imagesource.get();
	}

	/*virtual*/
	void 
	Tracker::update(osg::NodeVisitor* nv /*=0L*/)
	{		
	}

	/* virtual */
	const double* 
	Tracker::getProjectionMatrix() const 
	{
		return m_projectionMatrix;
	}

	/* virtual */
	void	Tracker::setEnable(const bool & _enable)
	{
		m_enable = _enable;
	}

	/* virtual */
	bool	Tracker::getEnable() const
	{
		return m_enable;
	}

	std::string Tracker::getLabel() const 
	{
		std::string Result = m_name;
		Result += "-";
		Result += m_version;
		return Result;
	}
	

};
