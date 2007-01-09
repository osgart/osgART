#include "osgARTSmoothAs.h"

namespace osgART {

	SmoothAs::SmoothAs()
	{
	m_matrixBufferSize = 5; // default
	}

	SmoothAs::~SmoothAs(){

	}

	void SmoothAs::setMatrixBufferSize(int size){

		m_matrixBufferSize = size;

	}

	void SmoothAs::putTransform(osg::Matrix& matrix)
	{
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

	osg::Matrix* SmoothAs::getTransform()
	{
		// calculate average quat and return
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