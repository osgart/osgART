/*
 *	osgART/Marker
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

#include "osgART/Marker"
#include <osg/Notify>

#include "osgART/TransformFilterCallback"

namespace osgART {


	Marker::Marker() : osg::Referenced(), 
		m_valid(false),
		m_name("Marker")		
	{
		// hse25: No fields within osgART
		//m_fields["name"] = new TypedField<std::string>(&m_name);
		//m_fields["active"] = new TypedField<bool>(&m_active);
	}

	Marker::~Marker() 
	{	   
		osg::notify() << "osgART::Marker::~Marker() : Deleted marker " <<
			m_name << " of type '" << typeid(*this).name() << "' " << std::endl; 
	}

	const 
	osg::Matrix& Marker::getTransform() const 
	{
		return m_transform;
	}

	osg::Matrix* Marker::getTransformMatrix()
	{
		return &m_transform;
	}

	bool
	Marker::isValid() const
	{
		return m_valid;
	}

	bool
	Marker::isActive() const 
	{
		return m_active;
	}

	void
	Marker::setName(const std::string& name) 
	{
		m_name = name;
	}

	const std::string&
	Marker::getName() const 
	{
		return m_name;
	}

	void
	Marker::setFilterCallback(Marker::Callback *callback)
	{
		this->m_filtercallback = callback;
	}

	Marker::Callback*
	Marker::getFilterCallback()
	{
		return this->m_filtercallback.get();
	}


	void 
	Marker::updateTransform(const osg::Matrix& transform) 
	{
		if (m_valid) {

			if (m_filtercallback.valid()){
				(*m_filtercallback)(this,transform);
			}
			 else {			
				m_transform = transform;
			}
		}
		// update callback
		if (m_updatecallback.valid()) (*m_updatecallback)(this,transform);
	}
	
	void 
	Marker::copyData(const Marker& marker)
	{
		m_valid = marker.m_valid;
		m_transform = marker.m_transform;
	}

	// ------------------------------------------

	class MarkerContainerCallback : public Marker::Callback 
	{
	public:


		osg::ref_ptr<MarkerContainer> m_container;
		
		MarkerContainerCallback(MarkerContainer* container) : 
			m_container(container)
		{			           
		}

		void operator()(Marker* marker,const osg::Matrix&)
		{
			m_container->copyData(*marker);
		}
	};

	bool
	MarkerContainer::isValid() const
	{
		return (m_marker.valid()) ? m_marker->isValid() : false;
	}


	MarkerContainer::MarkerContainer(Marker* marker) : Marker(), m_marker(marker)
	{
		if (m_marker.valid()) 
		{
			m_marker->m_updatecallback = new MarkerContainerCallback(this);
		}
	}
	
	/*virtual */
	void 
	MarkerContainer::setActive(bool active)
	{
		if (m_marker.valid()) 
		{
			m_marker->setActive(active);
			m_active = m_marker->isActive();
		}
	}


	Marker::MarkerType
	MarkerContainer::getType() const
	{
		return (m_marker.valid()) ? m_marker->getType() : Marker::ART_UNKOWN;
	}
		
};
