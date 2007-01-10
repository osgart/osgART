#include "osgARTSmoothAs.h"

namespace osgART {

	SmoothAs::SmoothAs()
	{
		// defaults
		m_matrixBufferSize = 5; 
		m_outlierDistance = 50; // i.e. 5 cm
		m_outlierCount = 0;
		m_maxNumOfOutliers = 10; 
		m_OutlierRejection = true;
	}

	SmoothAs::~SmoothAs(){

	}

	void SmoothAs::setMatrixBufferSize(int size){
		m_matrixBufferSize = size;
	}

	void SmoothAs::setOutlierDistance(int dist){
		m_outlierDistance = dist;
	}

	void SmoothAs::setmaxNumofOutliers(int num){
		m_maxNumOfOutliers = num;
	}

	void SmoothAs::enableOutlierRejection(bool val){
		m_OutlierRejection = val;
	}

	void SmoothAs::putTransform(osg::Matrix& matrix)
	{
		if (m_OutlierRejection){
			if (checkForTransOutlier(matrix) || checkForRotOutlier(matrix)){
				m_outlierCount++;
				if (m_outlierCount < m_maxNumOfOutliers) return;
			}
		}
	
		m_outlierCount = 0;
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
	}

	bool SmoothAs::checkForTransOutlier(osg::Matrix& matrix){
		
		if (!m_matrixlist.empty())
		{
			// check if new matrix is likely to be an outlier by comparing translations
				
			osg::Matrix* pre_mat;
			MatrixList::iterator _end = m_matrixlist.end() - 1;
			pre_mat = (osg::Matrixd*)(*_end);
		
			osg::Vec3d pre_trans = pre_mat->getTrans(); // previous translation
			osg::Vec3d new_trans = matrix.getTrans();	// new translation
			osg::Vec3d connect = pre_trans - new_trans; // connecting vector

			if (connect.length() > m_outlierDistance)
			{
				return true;	
			}
		}
		return false;
	}


	bool SmoothAs::checkForRotOutlier(osg::Matrix& matrix){
		if (!m_matrixlist.empty())
		{
			// check if new matrix is likely to be an outlier by comparing some angles
				
			osg::Matrix* pre_mat;
			MatrixList::iterator _end = m_matrixlist.end() - 1;
			pre_mat = (osg::Matrixd*)(*_end);
		
			osg::Quat pre_quat = pre_mat->getRotate(); // previous translation
			osg::Quat new_quat = matrix.getRotate();	// new translation
			
			double pre_angle = 0;
			osg::Vec3d pre_axis;
			double new_angle = 0;
			osg::Vec3d new_axis;

			pre_quat.getRotate(pre_angle,pre_axis);
			new_quat.getRotate(new_angle,new_axis);

			// looking at distance between rotation axis here
			osg::Vec3d connect = pre_axis - new_axis;
			if (connect.length() > 1)
			{
				return true;
			}			
		}
		return false;
	}


	osg::Matrix* SmoothAs::getTransform()
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
				
		osg::Matrix* result_matrix = new osg::Matrixd(result_quat);
		result_matrix->setTrans(result_trans);
		return result_matrix;
	}
	
}