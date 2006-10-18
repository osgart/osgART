#ifndef ARSCENE_H
#define ARSCENE_H

#include <Producer/RenderSurface>
#include <osgProducer/Viewer>

#include <osg/Node>
#include <osg/Group>
#include <osg/Projection>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/VideoBackground>
#include <osgART/VideoTextureRectangle>

#include "ARNode.h"
#include "DummyImageLayer.h"
#include "FBOManager.h"

#include <vector>
using namespace std;


class ARScene : public osg::Group
{
public:
	ARScene();
	virtual ~ARScene();

	void init(osg::ref_ptr<osgART::GenericTracker> tracker, int _trakerID = 0);

	void addARNode(osg::ref_ptr<ARNode> arnode, int binNum, bool addToSceneGraph = true);

	osg::ref_ptr<ARNode> addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum = 20);
	
	void addToBackgroundGroup(osg::Node *aNode);
	void addToBackgroundTextureGroup(osg::Node *aNode, bool isARNode);
	

	// choose to use only one!
	osg::ref_ptr<osgART::VideoBackground> initDefaultVideoBackground(int id);
	osg::ref_ptr<osg::Texture> initTextureVideoBackground(int id, bool addDummyLayer = true);

	osg::ref_ptr<osg::Group> getBackgroundGroup()
	{
		return background;
	};
	
	osg::ref_ptr<osg::Group> getBackgroundTextureGroup()
	{
		return backgroundTextureBuffer;
	};

	osg::ref_ptr<osg::Group> getForegroundGroup()
	{
		return foreground;
	};

	osg::ref_ptr<osg::Texture> getBackgroundTexture()
	{
		return videoBackgroundTexture;
	};

	osg::ref_ptr<ARNode> at(int id);
	int size();
private:

	osg::ref_ptr<osgART::VideoBackground> makeVideoBackground(int id);

	osg::ref_ptr<FBOManager> fboManager;
	vector< osg::ref_ptr<ARNode> > arNodes;

	int trackerID;
	osg::ref_ptr<osg::Projection> projectionMatrix;
	osg::ref_ptr<osg::Projection> projectionMatrixForFBO;
	
	osg::ref_ptr<osg::Group> background;
	osg::ref_ptr<osg::Group> backgroundTextureBuffer;
	osg::ref_ptr<osg::Group> foreground;


	// for background
	int bgWidth, bgHeight;

	osg::ref_ptr<osg::Texture> videoBackgroundTexture;
};

#endif