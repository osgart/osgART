/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
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

    ///

	Tracker::Tracker()
        : osg::Object()
        , osgART::EventHandler()
        , _modifiedCount(0xFFFFF)
        , _stats(new osg::Stats("tracker"))
	{
        osg::UserDataContainer* udc = this->getOrCreateUserDataContainer();

        // *cough*
        udc->addUserObject(this);

        osg::Object* o = new osg::TemplateValueObject<std::string>("name",std::string("what?"));
        udc->addUserObject(o);


        this->dump();


//        udc->addUserObject(new osg::TemplateValueObject(name,std::string("gah")));

//		_fields["name"]	= new TypedField<std::string>(&_name);
//		_fields["version"]	= new TypedField<std::string>(&_version);
//		_fields["enable"]	= new CallbackField<Tracker,bool>(this,
//			&Tracker::getEnable,
        //			&Tracker::setEnable);
    }

    Tracker::Tracker(const Tracker &rhs, const osg::CopyOp & co)
    {
    }

	Tracker::~Tracker()
	{

		//
		// Explicitly delete/unref all targets
		//
        for( TargetList::iterator mi = _targetlist.begin();
			 mi != _targetlist.end();
			 mi++)
		{
			(*mi) = 0L;
		}

		// Targets are associated with a specific tracker instance,
		// so will be deleted when the tracker is deleted.
		_targetlist.clear();
	}

	void 
	Tracker::dump()
	{
		osg::UserDataContainer* udc = this->getOrCreateUserDataContainer();
		for (osg::UserDataContainer::DescriptionList::iterator it = udc->getDescriptions().begin();
			it != udc->getDescriptions().end();
			++it)
		{
			OSG_INFO << (*it) << std::endl;
		}
	}

	/* virtual */
	TrackerConfiguration* 
	Tracker::getConfiguration()
	{
        return 0L;
    }

    /* virtual */
    Tracker::Traits Tracker::getTraits()
    {
        return NoTraits;
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
		OSG_WARN << "Method not implemented for this tracker!" << std::endl;

		return 0L;
	}

	/*virtual */
	void
    Tracker::removeTarget(Target *target)
	{
		TargetList pruned; pruned.reserve(_targetlist.size());

		for (TargetList::iterator it = _targetlist.begin();
			it != _targetlist.end();
			++it)
		{
			if ((*it) != target) pruned.push_back(*it);
		}

		std::swap(_targetlist,pruned);
	}



	Target* Tracker::getTarget( size_t idx )
	{
		return _targetlist.at(idx);
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
	Tracker::update()
	{

	}

	/*virtual*/
	void
	Tracker::updateCallback(osg::NodeVisitor* nv /*=0L*/)
	{
		update();
	}

};
