
#include <osg/io_utils>

#include "osgART/TransformFilterCallback"

namespace osgART {

	TransformFilterCallback::TransformFilterCallback():
	m_TransOutlierDistance(50),				// i.e. 5cm
	m_maxNumOfTransRejections(10),		// max number of subsequent rejections
	m_TransRejectionCount(0),			// current number of rejections
	m_rotationSmoothFactor(0.15f),
	m_translationSmoothFactor(0.15f),
	m_doTransOutlierRejection(false),
	m_doRotationalSmoothing(true),
	m_doTranslationalSmoothing(true)
	{			           
	}

	void TransformFilterCallback::operator()(Marker* marker, const osg::Matrix& nmatrix)
	{
		osg::Matrix* marker_transfrom = marker->getTransformMatrix();
		
		// outlier rejection
		if (m_doTransOutlierRejection){
			// if outlier do nothing
			if (isTransOutlier(nmatrix)){
				m_TransRejectionCount++;
				if (m_TransRejectionCount < m_maxNumOfTransRejections) {
					// std::cout << "TransformFilterCallback::operator():/ Rejected Translational outlier" << std::endl;					
					return;
				}
			}
		}
		
		// if no outlier do smoothing
		m_TransRejectionCount = 0;
		marker_transfrom->set(nmatrix);

		// rotational smoothing
		if (m_doRotationalSmoothing){
			// new rotation
			osg::Quat newRotation;
			nmatrix.get(newRotation);
			// slerp stored and new rotation
			m_storedRotation.slerp(m_rotationSmoothFactor, m_storedRotation, newRotation);
			// update marker trans
			marker_transfrom->setRotate(m_storedRotation);
			//std::cout << "TransformFilterCallback::operator():/ Rotational Smoothing" << std::endl;
		}
	
		// translational smoothing
		if (m_doTranslationalSmoothing){
			
			// new translation
			osg::Vec3 newTranslation;
			newTranslation = nmatrix.getTrans();
			// connecting vector
			osg::Vec3 a = newTranslation - m_storedTranslation;
			// bias translation
			osg::Vec3 b = a * m_translationSmoothFactor;
			m_storedTranslation += b;
			// update marker trans
			marker_transfrom->setTrans(m_storedTranslation);
			//std::cout << "TransformFilterCallback::operator():/ Translational Smoothing" << std::endl;
		}

	}

	bool TransformFilterCallback::isTransOutlier(const osg::Matrix& matrix){
			// check if new matrix is likely to be an outlier by comparing translations
			osg::Vec3d new_trans = matrix.getTrans();				// new translation
			osg::Vec3d connect = m_storedTranslation - new_trans;	// connecting vector

			//std::cout << "trans_diff: " << connect.length() << std::endl;
			if (connect.length() > m_TransOutlierDistance)
			{
				//std::cout << "TransformFilterCallback::isTransOutlier:/ rejected translational outlier" << std::endl;
				return true;	
			}
		return false;
	}


	void TransformFilterCallback::setTransOutlierMaxNumber(unsigned int val){
		m_maxNumOfTransRejections = val;
	}
	unsigned int TransformFilterCallback::getTransOutlierMaxNumber(){
		return m_maxNumOfTransRejections;
	}
	void TransformFilterCallback::setTransOutlierDist(float val){
		m_TransOutlierDistance = val;
	}
	float TransformFilterCallback::getTransOutlierDist(){
		return m_TransOutlierDistance;
	}


	void TransformFilterCallback::setRotationalSmoothingFactor(float val){
		m_rotationSmoothFactor = val;
	}
	float TransformFilterCallback::getRotationalSmoothingFactor(){
		return m_rotationSmoothFactor;
	}

	void TransformFilterCallback::setTranslationalSmoothingFactor(float val){
		m_translationSmoothFactor = val;
	}
	float TransformFilterCallback::getTranslationalSmoothingFactor(){
		return m_translationSmoothFactor;
	}

	// enable/disable optional parts
	void TransformFilterCallback::enableRotationalSmoothing(bool val){
		m_doRotationalSmoothing = val;
	}
	bool TransformFilterCallback::RotationalSmoothing(){
		return m_doRotationalSmoothing;
	}

	void TransformFilterCallback::enableTranslationalSmoothing(bool val){
		m_doTranslationalSmoothing = val;
	}
	bool TransformFilterCallback::TranslationalSmoothing(){
		return m_doTranslationalSmoothing;
	}

	void TransformFilterCallback::enableTransOutlierRejection(bool val){
		m_doTransOutlierRejection = val;
	}
	bool TransformFilterCallback::TransOutlierRejection(){
		return m_doTransOutlierRejection;
	}
};
