#include "osgART/TransformFilter"

#include <iostream>

namespace osgART {

	TransformFilter::TransformFilter() : 
		osg::Referenced(),
		m_OutlierRejection(true),
		m_matrixBufferSize (10),			// number of matrices to keep in history
		m_outlierDistance(50),				// i.e. 5 cm
		m_RejectionCount(0),				// current number of subsequent rejections
		m_maxNumOfRejections(10)			// max number of subsequent rejections
	{
	
	}

	TransformFilter::~TransformFilter()
	{
	}

	void TransformFilter::setMatrixBufferSize(int size){
		m_matrixBufferSize = size;
	}

	void TransformFilter::setOutlierDistance(int dist){
		m_outlierDistance = dist;
	}

	void TransformFilter::setmaxNumofRejections(int num){
		m_maxNumOfRejections = num;
	}

	void TransformFilter::enableOutlierRejection(bool val){
		m_OutlierRejection = val;
	}

	/*
	*	check for outliers and store transformation matrix in list
	*/
	osg::Matrix TransformFilter::putTransformMatrix(osg::Matrix& matrix)
	{
		
		// reject outliers and return
		if (m_OutlierRejection){
				
			/*
			*	as it is, check for rotational outliers is rarely positive -> excluded
			*	left the code in case you're interested
			*/
			//if (checkForTransOutlier(matrix) || checkForRotOutlier(matrix)){ 
			if (checkForTransOutlier(matrix)){ 
				//std::cout << "osgART::TransformFilter: OUTLIER rejected" << std::endl;
				m_RejectionCount++;	
				if (m_RejectionCount < m_maxNumOfRejections) return osg::Matrix(averageTransformMatrices());
			}
		}
		
		// if no outlier, add transformation matrix to list
		m_RejectionCount = 0;
		
		// if list exceeds max size, remove oldest entry and add newest matrix
		if ( m_matrixlist.size() >= m_matrixBufferSize && !m_matrixlist.empty() )
		{
			m_matrixlist.erase(m_matrixlist.begin(),m_matrixlist.begin()+1);
			m_matrixlist.push_back(new osg::Matrix(matrix));
		}
		// else add newest matrix
		else 
		{
			m_matrixlist.push_back(new osg::Matrix(matrix));
		}
		// get current average of matrices and return
		return osg::Matrix(averageTransformMatrices());
	}

	bool TransformFilter::checkForTransOutlier(osg::Matrix& matrix){
		
		if (!m_matrixlist.empty())
		{
			// check if new matrix is likely to be an outlier by comparing translations
				
			osg::Matrix* pre_mat;
			MatrixList::iterator _end = m_matrixlist.end() - 1;
			pre_mat = (osg::Matrixd*)(*_end);
		
			osg::Vec3d pre_trans = pre_mat->getTrans(); // previous translation
			osg::Vec3d new_trans = matrix.getTrans();	// new translation
			osg::Vec3d connect = pre_trans - new_trans; // connecting vector

			//std::cout << "trans_diff: " << connect.length() << std::endl;
			if (connect.length() > m_outlierDistance)
			{
				//std::cout << "TransformFilter: rejected translational outlier" << std::endl;
				return true;	
			}
		}
		return false;
	}


	bool TransformFilter::checkForRotOutlier(osg::Matrix& matrix){
		if (!m_matrixlist.empty())
		{
			// check if new matrix is likely to be an outlier by comparing some angles
				
			osg::Matrix* pre_mat;
			MatrixList::iterator _end = m_matrixlist.end() - 1;	// get matrix of previous frame
			pre_mat = (osg::Matrixd*)(*_end);
		
			osg::Quat pre_quat = pre_mat->getRotate(); // previous rotation
			osg::Quat new_quat = matrix.getRotate();	// new rotation
			
			double pre_angle = 0;
			osg::Vec3d pre_axis;
			double new_angle = 0;
			osg::Vec3d new_axis;

			pre_quat.getRotate(pre_angle,pre_axis);		// get angle and rotation axis
			new_quat.getRotate(new_angle,new_axis);

			// looking at distance between rotation axis
			osg::Vec3d connect = pre_axis - new_axis;
			//std::cout << "rot_diff: " << connect.length() << std::endl;
			if (connect.length() > 0.5)					
			{
				//std::cout << "TransformFilter: rejected rotational outlier" << std::endl;
				return true;
			}			
		}
		return false;
	}


	/*
	*	calculate the current averaged transformation matrix and return
	*	-> outlier rejection is done before when putting matrices
	*/
	osg::Matrix TransformFilter::averageTransformMatrices()
	{
		osg::Matrix* tmp_mat = (osg::Matrixd*)m_matrixlist[0];
		// current values are of first entry in list
		osg::Quat result_quat = tmp_mat->getRotate();
		osg::Vec3d result_trans = tmp_mat->getTrans();
			
		// browse through list and average/sum quats and transformations
		MatrixList::iterator _end = m_matrixlist.end();
		for (MatrixList::iterator iter = m_matrixlist.begin() + 1; iter != _end; ++iter)		
		{
			tmp_mat = (osg::Matrix*)(*iter);
			result_quat.slerp(0.5,result_quat,tmp_mat->getRotate());
			result_trans += osg::Vec3d(tmp_mat->getTrans());
		}
		
		// average translation
		double num = m_matrixlist.size();
		result_trans.set(result_trans.x()/num, result_trans.y()/num, result_trans.z()/num);
				
		osg::Matrix result_matrix;
		result_matrix.setRotate(result_quat);
		result_matrix.setTrans(result_trans);
		return result_matrix;
	}
	
}