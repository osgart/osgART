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

#include "osgART/TransformFilterCallback"

#include <osg/io_utils>
#include <osg/MatrixTransform>
#include <osg/Matrixd>


namespace osgART {

	TransformFilterCallback::TransformFilterCallback() : osg::NodeCallback(),
		m_translationSmoothFactor(0.75f),
		m_rotationSmoothFactor(0.75f),
		m_TransOutlierDistance(50),				// i.e. 5cm
		m_maxNumOfTransRejections(10),			// max number of subsequent rejections
		m_TransRejectionCount(0),				// current number of rejections
		m_doRotationalSmoothing(true),
		m_doTranslationalSmoothing(true),
		m_doTransOutlierRejection(false)
	{			           
	}

	void TransformFilterCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{

		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

		if (mt) 
		{

			const osg::Matrix& _matrix = mt->getMatrix();
			osg::Matrix _nm = _matrix;

			// osg::Matrix* target_transfrom = m_target->getTransformMatrix();
			
			// outlier rejection
			if (m_doTransOutlierRejection) 
			{
				// if outlier do nothing
				if (isTransOutlier(_matrix)){
					m_TransRejectionCount++;
					if (m_TransRejectionCount < m_maxNumOfTransRejections) {
						// std::cout << "TransformFilterCallback::operator():/ Rejected Translational outlier" << std::endl;					
						return;
					}
				}
			}
			
			// if no outlier do smoothing
			m_TransRejectionCount = 0;
			mt->setMatrix(_nm);

			// rotational smoothing
			if (m_doRotationalSmoothing){
				// new rotation
				osg::Quat newRotation;
				_matrix.get(newRotation);
				// slerp stored and new rotation
				m_storedRotation.slerp(m_rotationSmoothFactor, m_storedRotation, newRotation);

				// update target trans
				_nm.makeRotate(m_storedRotation);				
				
			}
		
			// translational smoothing
			if (m_doTranslationalSmoothing){
				
				// new translation
				osg::Vec3 newTranslation;
				newTranslation = _matrix.getTrans();
				// connecting vector
				osg::Vec3 a = newTranslation - m_storedTranslation;
				// bias translation
				osg::Vec3 b = a * m_translationSmoothFactor;
				m_storedTranslation += b;
				// update target trans
				_nm.setTrans(m_storedTranslation);
				//std::cout << "TransformFilterCallback::operator():/ Translational Smoothing" << std::endl;
			}

			mt->setMatrix(_nm);
		}

		

		traverse(node,nv);

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
