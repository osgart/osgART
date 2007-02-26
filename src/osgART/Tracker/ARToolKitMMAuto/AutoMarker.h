#ifndef AUTOMARKER_H
#define AUTOMARKER_H	1

#include <osg/Matrix>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

#include <osgART/ARTTransform>
#include "SingleMarker"
#include "osgARTSmoothAs.h"

#include <string>
#include <Map>
#include <List>


namespace osgART{

#define CATRANSLISTSIZE 50 // number of CandidateMarker->AnchorMarker transformation matrices kept in memory for averaging

class SmoothAs;

class AutoMarker : public osgART::SingleMarker{



public:

	AutoMarker();
	~AutoMarker();

	osg::Matrixd	getCATransMat();
	osg::Matrixd	getCATransMatMean();
	osg::Matrixd	getCATransMatMedian();
	osg::Matrixd	getCATransMatVariance();
    		
	int				getNumberOfCATransMatSamples();
			
	double			getConfidence();
	void			setConfidence(double cf);
	void			addCATransMat(osg::Matrixd*);

	void			setValid();
	
	void			overrideupdateTransform(const osg::Matrix& transform);



	bool isTrusted();
	void isTrusted(bool);

protected:

	// list of transformation matrices AnchorMarker -> thisCandidateMarker
	typedef std::vector< osg::Matrixd* > CATransList;
	CATransList m_caTransMatList;

	osg::Matrixd	m_CATransmat;
	osg::Matrixd	m_CATransmatMean;
	osg::Matrixd	m_CATransmatMedian;
	osg::Matrixd	m_CATransmatVariance;
	osgART::Marker* m_Marker;
	bool			m_trusted;
	osgART::SmoothAs*		m_mySmoothAs;

private:

	
};
};
#endif