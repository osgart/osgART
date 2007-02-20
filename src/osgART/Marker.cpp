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
		m_seen(false),
		m_name("Marker")		
	{
		m_fields["name"] = new TypedField<std::string>(&m_name);
		m_fields["active"] = new TypedField<bool>(&m_active);

		setRotationalSmoothing(0.15f);
		setTranslationalSmoothing(0.15f);

		m_updatecallback = new TransformFilterCallback();
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
	Marker::setRotationalSmoothing(float r) 
	{
		m_rotationSmoothFactor = 
			1.0f - osg::clampBetween<float>(r, 0, 1);
	}
	float
	Marker::getRotationalSmoothing() const 
	{
		return (1.0f - m_rotationSmoothFactor);
	}

	void
	Marker::setTranslationalSmoothing(float t) {
		m_positionSmoothFactor = 
			1.0f - osg::clampBetween<float>(t, 0, 1);
	}

	float 
	Marker::getTranslationalSmoothing() const 
	{
		return (1.0f - m_positionSmoothFactor);
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

			if (m_seen) {				
				
				osg::Vec3 newPosition;
				osg::Quat newRotation;

				newPosition = transform.getTrans();
				transform.get(newRotation);
				m_storedRotation.slerp(m_rotationSmoothFactor, m_storedRotation, newRotation);

				osg::Vec3 a = newPosition - m_storedPosition;

				osg::Vec3 b = a * m_positionSmoothFactor;
				m_storedPosition += b;

				if (m_filtercallback.valid())(*m_filtercallback)(this,transform);


			} else {			
			
				m_storedPosition = transform.getTrans();
				transform.get(m_storedRotation);
				m_seen = true;

			}

			m_transform.set(m_storedRotation);
			m_transform.setTrans(m_storedPosition);


		} else {
		
			m_seen = false;

		}

		// update callback
		if (m_updatecallback.valid()) (*m_updatecallback)(this,transform);

	}
	
	void 
	Marker::copyData(const Marker& marker)
	{
		m_valid = marker.m_valid;
		m_transform = marker.m_transform;
		m_seen = marker.m_seen;
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
