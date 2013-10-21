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

#include "osgART/Sensor"
#include "osgART/SensorData"

#include <osg/Notify>

#include <osg/ValueObject>
#include <osg/UserDataContainer>

namespace osgART {

    ///

	Sensor::Sensor() :
        osgART::Object(),
        osg::Object(),
        FieldContainer<Sensor>()
	{
		_stats=new osg::Stats("sensor");

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

    Sensor::Sensor(const Sensor &container, 
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		osgART::Object(),
		osg::Object(), 
		FieldContainer<Sensor>()
    {
    }

	Sensor::~Sensor()
	{
	
	}
	
	Sensor& 
	Sensor::operator=(const Sensor &)
	{
		return *this;
	}

	// static 
	Sensor* Sensor::cast(osg::Referenced* instance)
	{ 
		return reinterpret_cast<Sensor*>(instance);
	}
	
	Field*
	Sensor::get(const std::string& name)
	{
		FieldMap::iterator _found = _fields.find(name);
		// return 0 if the field is not existant
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	// virtual
	SensorConfiguration* 
	Sensor::getConfiguration()
	{
        return 0L;
    }

	// virtual
	void 
	Sensor::setConfiguration(SensorConfiguration* config)
	{

	}

	// virtual
	void
	Sensor::updateCallback(osg::NodeVisitor* nv /*=0L*/)
	{
		update();
	}
	
	void 
	Sensor::dump()
	{
		osg::UserDataContainer* udc = this->getOrCreateUserDataContainer();
		for (osg::UserDataContainer::DescriptionList::iterator it = udc->getDescriptions().begin();
			it != udc->getDescriptions().end();
			++it)
		{
			OSG_INFO << (*it) << std::endl;
		}
	}
};
