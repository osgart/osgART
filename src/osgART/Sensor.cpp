/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2014 Raphael Grasset, Julian Looser, Hartmut Seichter
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

// std include

// OpenThreads include

// OSG include
#include <osg/Notify>

#include <osg/ValueObject>
#include <osg/UserDataContainer>

// local include
#include "osgART/Export"
#include "osgART/Sensor"
#include "osgART/SensorData"



namespace osgART {

    ///

	Sensor::Sensor() :
        osgART::Object(),
        FieldContainer<Sensor>(),
		_sensorConfiguration(0L)
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
		FieldContainer<Sensor>(),
		_sensorConfiguration(0L)
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
		// return 0 if the field is not existent
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	// virtual
	SensorConfiguration* 
	Sensor::getOrCreateConfiguration()
	{
		if (!_sensorConfiguration)
		{
			_sensorConfiguration=new osgART::SensorConfiguration();
		}
		return _sensorConfiguration;
    }

	// virtual
	void 
	Sensor::setConfiguration(SensorConfiguration* config)
	{
		*_sensorConfiguration=*config;

	}

	// virtual 
	SensorData* 
		Sensor::getSensorData(size_t i /* =0 */) 
	{
		//todo check stream id exist
		return _sensorDataList[i];
	}
};
