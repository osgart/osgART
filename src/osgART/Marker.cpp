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


namespace osgART {


	Marker::Marker() : osg::Referenced() {
		m_transform.makeIdentity();
		m_valid = false;

		setRotationalSmoothing(0.15f);
		setTranslationalSmoothing(0.15f);

		m_name = "marker";

		m_seen = false;
	}

	Marker::~Marker() {	   
#ifndef NDEBUG
		std::cout << "Deleting marker" << std::endl;
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

	void 
	Marker::updateTransform(osg::Matrix transform) {
		
		// jcl64: removed loops
		if (m_valid) {

			if (m_seen) {				
				
				osg::Vec3 newPosition;
				osg::Quat newRotation;

				newPosition = transform.getTrans();
				transform.get(newRotation);
				m_storedRotation.slerp(m_rotationSmoothFactor, 
					m_storedRotation, newRotation);

				osg::Vec3 a = newPosition - m_storedPosition;

				osg::Vec3 b = a * m_positionSmoothFactor;
				m_storedPosition += b;
				
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
	}
	
	
	Field*
	Marker::get(const std::string& name)
	{
		FieldMap::iterator _found = m_fields.find(name);
		return (_found != m_fields.end()) ? _found->second.get() : 0L;
	}


	// static
	/*Marker* Marker::create(Marker::MarkerType type) 
	{
		Marker* ret = 0L;

		switch (type) {
			case Marker::ART_SINGLE :
				ret = new SingleMarker();
				break;
			case Marker::ART_MULTI :
				ret = new MultiMarker();
				break;
			default:
				// we could warn here ?
				break;
		};

		return ret;
	};
	*/
};