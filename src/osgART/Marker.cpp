/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */
#include "osgART/Marker"
#include <iostream>
#include <osg/io_utils>

namespace osgART {


	Marker::Marker() : osg::Referenced(),
		m_doTransFiltering(false),
		m_transFilter(new TransformFilter)
	{
		m_fields["name"] = new TypedField<std::string>(&m_name);
		m_fields["active"] = new TypedField<bool>(&m_active);

		m_transform.makeIdentity();
		m_valid = false;

		// not used by new filter
		//setRotationalSmoothing(0.15f);
		//setTranslationalSmoothing(0.15f);

		m_name = "marker";
		
		// not used by new filter
		// m_seen = false;
	}

	Marker::Marker(const Marker& marker) 
	{
		std::cout << "Stuff it!" << std::endl;
	}

	Marker::~Marker() {	   
#ifndef NDEBUG
		osg::notify() << "Deleting marker" << std::endl;
#endif
	}

	const 
	osg::Matrix& Marker::getTransform() const {
		return m_transform;
	}

	bool Marker::isValid() const {
		return m_valid;
	}

	bool Marker::isActive() const {
		return m_active;
	}

	void Marker::setName(const std::string& name) {
		m_name = name;
	}

	const std::string& Marker::getName() const {
		return m_name;
	}

	void Marker::setRotationalSmoothing(float r) {
		m_rotationSmoothFactor = 1.0f - osg::clampBetween<float>(r, 0, 1);
	}
	float Marker::getRotationalSmoothing() const {
		return (1.0f - m_rotationSmoothFactor);
	}
	void Marker::setTranslationalSmoothing(float t) {
		m_positionSmoothFactor = 1.0f - osg::clampBetween<float>(t, 0, 1);
	}

	float Marker::getTranslationalSmoothing() const {
		return (1.0f - m_positionSmoothFactor);
	}

	void Marker::updateTransform(const osg::Matrix& transform, bool alternative /*= false*/) 
	{
		// check for valid transform filter
		if (!m_transFilter.valid())
		{
			std::cerr << "Marker::updateTransform: m_transFilter not valid!" << std::endl; 
			return;
		}

		// if marker valid
		if (m_valid) 
		{
			// filter for ouliers and do smoothing
			if (m_doTransFiltering)
			{
				m_transform = m_transFilter->putTransformMatrix(osg::Matrix(transform));
			}
			else
			{
				m_transform = transform;
			}
		} 
	}
};
