#ifndef ARNODE_H
#define ARNODE_H

#include <osg/Node>
#include <osg/Group>
#include <osg/Projection>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>

#include <osg/Matrixf>


class ARNode : public osg::Group
{
public:
	ARNode();
	virtual ~ARNode();

	void init(int markerID, int trackerID = 0);
	void addModel(osg::ref_ptr<osg::Node> model);

	osg::ref_ptr<osgART::ARTTransform> getMarkerTrans();
private:

	osg::ref_ptr<osgART::ARTTransform> markerTrans;

};

#endif