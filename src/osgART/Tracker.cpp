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
#include "osgART/Target"
#include "osgART/Video"

#include <osg/Notify>

#include <iostream>

#include <osg/ValueObject>
#include <osg/UserDataContainer>

namespace osgART {

	TrackerCallback::TrackerCallback(Tracker* tracker): _tracker(tracker), _framenumber(-1) {

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


    ///

	Tracker::Tracker()
        : osg::Object()
        , _enable(true)
        , m_lastModifiedCount(0xFFFFF)
        , _stats(new osg::Stats("tracker"))
	{
        osg::UserDataContainer* udc = this->getOrCreateUserDataContainer();

        // *cough*
        udc->addUserObject(this);

//        osg::Object* o = new osg::TemplateValueObject<std::string>("name",std::string("what?"));
//        udc->addUserObject(o);
        //udc->addUserObject(new osg::TemplateValueObject(name,std::string("gah")));

//		_fields["name"]	= new TypedField<std::string>(&_name);
//		_fields["version"]	= new TypedField<std::string>(&_version);
//		_fields["enable"]	= new CallbackField<Tracker,bool>(this,
//			&Tracker::getEnable,
        //			&Tracker::setEnable);
    }

    Tracker::Tracker(const Tracker &rhs, const osg::CopyOp &)
    {
        //\todo implement!
    }

	Tracker::~Tracker()
	{

		//
		// Explicitly delete/unref all markers
		//
        for( TargetList::iterator mi = _markerlist.begin();
			 mi != _markerlist.end();
			 mi++)
		{
			(*mi) = 0L;
		}

		// Markers are associated with a specific tracker instance,
		// so will be deleted when the tracker is deleted.
		_markerlist.clear();
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
	Target*
	Tracker::addTarget(const std::string& config)
	{
		osg::notify(osg::WARN) << "Method not implemented for this tracker!" << std::endl;

		return 0L;
	}

	/*virtual */
	void
    Tracker::removeTarget(Target *target)
	{
		// TODO: implement
	}



	Target*
	Tracker::getTarget(int id)
	{
		Target *_m = (Target*)0L;

		try
		{
			_m = _markerlist[id].get();

		} catch(...)
		{

			osg::notify(osg::WARN) << "No Marker with ID: " << id << std::endl;
		}

		// return the Marker
		return _m;
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
		_imagesource = image;
	}

	/*virtual*/
	osg::Image* Tracker::getImage()
	{
		return _imagesource.get();
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
		return _projectionMatrix;
	}

	/* virtual */
	void	Tracker::setEnable(const bool &e)
	{
		_enable = e;
	}

	/* virtual */
	bool	Tracker::getEnable() const
	{
		return _enable;
	}

	std::string Tracker::getLabel() const
	{
		std::string Result = _name;
		Result += "-";
		Result += _version;
		return Result;
	}


};
