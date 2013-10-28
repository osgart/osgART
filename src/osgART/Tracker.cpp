/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include "osgART/Tracker"
#include "osgART/Target"
#include "osgART/Video"

#include <osg/Notify>

#include <osg/ValueObject>
#include <osg/UserDataContainer>

namespace osgART {

    ///

	Tracker::Tracker() :
        osgART::Object(),
        FieldContainer<Tracker>(),
		_trackerConfiguration(0L)
	{
		_stats=new osg::Stats("tracker");
		 
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

    Tracker::Tracker(const Tracker &container, 
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		osgART::Object(),
		FieldContainer<Tracker>(),
		_trackerConfiguration(0L)
    {

    }

	Tracker::~Tracker()
	{
		removeAllTargets();
	}
	
	Tracker& 
	Tracker::operator=(const Tracker &)
	{
		return *this;
	}

	// static 
	Tracker* Tracker::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<Tracker*>(instance);
	}
	
	Field*
	Tracker::get(const std::string& name)
	{
		FieldMap::iterator _found = _fields.find(name);
		// return 0 if the field is not existent
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	// virtual
	TrackerConfiguration* 
	Tracker::getOrCreateConfiguration()
	{
		if (!_trackerConfiguration)
		{
			_trackerConfiguration=new osgART::TrackerConfiguration();
		}
		return _trackerConfiguration;
    }

	// virtual
	void 
	Tracker::setConfiguration(TrackerConfiguration* config)
	{
		*_trackerConfiguration=*config;
	}

	// virtual
	CameraConfiguration* Tracker::getOrCreateCameraConfiguration()
	{
		if (!_cameraConfiguration)
		{
			_cameraConfiguration=new osgART::CameraConfiguration();
		}
		return _cameraConfiguration;
	}

	// virtual
	void Tracker::setCameraConfiguration(CameraConfiguration* config)
	{
		*_cameraConfiguration=*config;
	}

	void
	Tracker::removeAllTargets()
	{
		//
		// Explicitly delete/unref all targets
		//
        for( TargetListType::iterator mi = _targetlist.begin();
			mi != _targetlist.end();
			mi++)
		{
			(*mi) = 0L;
		}
		
		// Targets are associated with a specific tracker instance,
		// so will be deleted when the tracker is deleted.
		_targetlist.clear();

	}

    // virtual
    Tracker::Traits Tracker::getTraits()
    {
        return NoTraits;
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
		TargetListType pruned; pruned.reserve(_targetlist.size());

		for (TargetListType::iterator it = _targetlist.begin();
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

};
