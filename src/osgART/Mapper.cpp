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

// std include

// OpenThreads include

// OSG include
#include <osg/Notify>
#include <osg/ValueObject>
#include <osg/UserDataContainer>

// local include
#include "osgART/Mapper"
#include "osgART/MapData"
#include "osgART/Video"



namespace osgART {

    ///

    Mapper::Mapper() :
        osgART::Object(),
        FieldContainer<Mapper>(),
        _mapperConfiguration(0L)
	{
        _stats=new osg::Stats("Mapper");
		 
        osg::UserDataContainer* udc = this->getOrCreateUserDataContainer();

        // *cough*
        udc->addUserObject(this);

        osg::Object* o = new osg::TemplateValueObject<std::string>("name",std::string("what?"));
        udc->addUserObject(o);

        this->dump();

//        udc->addUserObject(new osg::TemplateValueObject(name,std::string("gah")));

//		_fields["name"]	= new TypedField<std::string>(&_name);
//		_fields["version"]	= new TypedField<std::string>(&_version);
//		_fields["enable"]	= new CallbackField<Mapper,bool>(this,
//			&Mapper::getEnable,
        //			&Mapper::setEnable);
    }

    Mapper::Mapper(const Mapper &container,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) :
		osgART::Object(),
        FieldContainer<Mapper>(),
        _mapperConfiguration(0L)
    {

    }

    Mapper::~Mapper()
	{
     //   removeAllMapData();
	}
	
    Mapper&
    Mapper::operator=(const Mapper &)
	{
		return *this;
	}

	// static 
    Mapper* Mapper::cast(osg::Referenced* instance)
	{ 
        return reinterpret_cast<Mapper*>(instance);
	}
	
	Field*
    Mapper::get(const std::string& name)
	{
		FieldMap::iterator _found = _fields.find(name);
		// return 0 if the field is not existent
		return (_found != _fields.end()) ? _found->second.get() : 0L;
	}

	// virtual
    MapperConfiguration*
    Mapper::getOrCreateConfiguration()
	{
        if (!_mapperConfiguration)
		{
            _mapperConfiguration=new osgART::MapperConfiguration();
		}
        return _mapperConfiguration;
    }

	// virtual
	void 
    Mapper::setConfiguration(MapperConfiguration* config)
	{
        *_mapperConfiguration=*config;
	}

    /*
	// virtual
    CameraConfiguration* Mapper::getOrCreateCameraConfiguration()
	{
		if (!_cameraConfiguration)
		{
			_cameraConfiguration=new osgART::CameraConfiguration();
		}
		return _cameraConfiguration;
	}

	// virtual
    void Mapper::setCameraConfiguration(CameraConfiguration* config)
	{
		*_cameraConfiguration=*config;
	}

	void
    Mapper::removeAllMapData()
	{
		//
		// Explicitly delete/unref all targets
		//
        for( MapDataListType::iterator mi = _mapDataList.begin();
            mi != _mapDataList.end();
			mi++)
		{
			(*mi) = 0L;
		}
		
        // Targets are associated with a specific Mapper instance,
        // so will be deleted when the Mapper is deleted.
        _mapDatatList.clear();

	}

    // virtual
    Mapper::Traits Mapper::getTraits()
    {
        return NoTraits;
    }

    // virtual
    MapData*
    Mapper::addMapData(const std::string& config)
	{
        OSG_WARN << "Method not implemented for this Mapper!" << std::endl;

		return 0L;
	}

    / /virtual
	void
    Mapper::removeMapData(MapData *map)
	{
        MapDataListType pruned; pruned.reserve(_mapDataList.size());

        for (MapDataListType::iterator it = _mapDataList.begin();
            it != _mapDataList.end();
			++it)
		{
            if ((*it) != map) pruned.push_back(*it);
		}

        std::swap(_mapDataList,pruned);
	}


    */
    MapData* Mapper::getMapData( size_t idx )
	{
        return _mapDataList.at(idx);
	}

};
