/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */
#include "osgART/Marker"

namespace osgART {


	Marker::Marker() 
	{
		m_transform.makeIdentity();
		m_valid = false;

		setRotationalSmoothing(0.15f);
		setTranslationalSmoothing(0.15f);

		m_name = "marker";

		m_seen = false;

	}

	Marker::~Marker() {	    
	}

	const 
	osg::Matrix &Marker::getTransform() const {
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
	Marker::updateTransform(double trans[3][4]) {
		
		// hse25: this needs optimisation! (jcl64: removed loops) done?
		if (m_valid) {

			// Copy ARToolKit matrix into OSG matrix
			osg::Matrix tmp(
				trans[0][0], trans[1][0], trans[2][0], 0.0f,
				trans[0][1], trans[1][1], trans[2][1], 0.0f,
				trans[0][2], trans[1][2], trans[2][2], 0.0f,
				trans[0][3], trans[1][3], trans[2][3], 1.0f);

			if (m_seen) {				
				
				osg::Vec3 newPosition;
				osg::Quat newRotation;

				newPosition = tmp.getTrans();
				tmp.get(newRotation);
				m_storedRotation.slerp(m_rotationSmoothFactor, 
					m_storedRotation, newRotation);

				osg::Vec3 a = newPosition - m_storedPosition;

				osg::Vec3 b = a * m_positionSmoothFactor;
				m_storedPosition += b;
				
			} else {			
			
				m_storedPosition = tmp.getTrans();
				tmp.get(m_storedRotation);
				m_seen = true;

			}

			m_transform.set(m_storedRotation);
			m_transform.setTrans(m_storedPosition);


		} else {
		
			m_seen = false;

		}
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