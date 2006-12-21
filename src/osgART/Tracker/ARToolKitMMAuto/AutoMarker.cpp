#include <Producer/RenderSurface>
#include <osgProducer/Viewer>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/Matrix>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
#include <osgART/VideoPlane>
#include <osg/io_utils>

#include "AutoMarker.h"


// += operator for osg matrices
osg::Matrix& operator += (osg::Matrix& m1, const osg::Matrix& m2) 
{
	for (register int i = 0; i < 4;i++)
	for (register int j = 0; j < 4;j++)
	m1(i,j) += m2(i,j);

	return m1;
}


AutoMarker::AutoMarker()
{
	// no AutoMarker can be trusted right away..
	m_trusted = false;
	m_CATransmat.makeIdentity();
}

AutoMarker::~AutoMarker()
{
}

// change in here which value/method to use!!!
osg::Matrixd AutoMarker::getCATransMat()
{
	return getCATransMatMean();
}


osg::Matrixd AutoMarker::getCATransMatMean()
{
	osg::Matrixd mean;
	osg::Matrixd denominator;
	osg::Matrixd* currentM;
	double number = 0;
	
	if (!m_caTransMatList.empty()){
	
		mean.set(0,0,0,0,
				0,0,0,0,
				0,0,0,0,
				0,0,0,0);
		// sum up all matrices
		for (int i = 0; i < m_caTransMatList.size(); i++){
			currentM = m_caTransMatList[i];
			mean += *currentM;
		}
		// normalize sum
		number = m_caTransMatList.size();
		denominator.set(1/number,0,0,0,
						0,1/number,0,0,
						0,0,1/number,0,
						0,0,0,1/number);
		
		mean.preMult(denominator);
		//std::cout << "Mean: " << mean << std::endl;
	 }
	return mean;
}
// try that later
osg::Matrixd AutoMarker::getCATransMatMedian()
{
	return osg::Matrixd();
}
// try that later, maybe used to reject arriving matrices that exceed certain bounds in variance
osg::Matrixd AutoMarker::getCATransMatVariance()
{
	return osg::Matrixd();
}
	

int AutoMarker::getNumberOfCATransMatSamples()
{
	return m_caTransMatList.size();
}

bool AutoMarker::isSeen()
{
	return m_seen;
}

double AutoMarker::getConfidence()
{
	return m_confidence;
}

void AutoMarker::setConfidence(double cf)
{
	m_confidence = cf;
}

void AutoMarker::addCATransMat(osg::Matrixd* matrix)
{
	// if list exceeds max size, remove oldest entry and add newest matrix
	if (m_caTransMatList.size() >= CATRANSLISTSIZE && !m_caTransMatList.empty() ){
		m_caTransMatList.erase(m_caTransMatList.begin(),m_caTransMatList.begin()+1);
		m_caTransMatList.push_back(matrix);
	}
	// else add newest matrix
	else {
		m_caTransMatList.push_back(matrix);
	}
}

void AutoMarker::setValid()
{
	m_valid = true;
}
void AutoMarker::setSeen()
{
	m_seen = true;
}


void AutoMarker::overrideupdateTransform(const osg::Matrix& transform){
	m_valid = true;
	m_seen = true;
	m_active = true;
	updateTransform(transform);
}


bool AutoMarker::isTrusted()
{
	return m_trusted;
}

void AutoMarker::isTrusted(bool val)
{
	m_trusted = val;
}