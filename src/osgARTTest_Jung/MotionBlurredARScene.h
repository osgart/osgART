#ifndef MOTIONBLURREDARSCENE_H
#define MOTIONBLURREDARSCENE_H

#include <Producer/RenderSurface>
#include <osgProducer/Viewer>

#include <osg/Node>
#include <osg/Group>
#include <osg/Projection>

#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/VideoLayer>


#include "ARScene.h"
#include "ARNode.h"
#include "DummyImageLayer.h"
#include "FBOManager.h"
#include "MotionBlurredModel.h"
#include "ShaderFactory.h"
#include "TextureAccumHandler.h"

#include <vector>
using namespace std;

class MotionBlurredARScene : public ARScene
{
public:
	MotionBlurredARScene();
	virtual ~MotionBlurredARScene();

	virtual void init(osg::ref_ptr<osgART::GenericTracker> tracker, int _trakerID = 0);
	virtual osg::ref_ptr<ARNode> addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum = 20);
	
	osg::ref_ptr<osg::Texture> initOffsetTexture(int _attriIndex);
	

	osg::ref_ptr<MotionBlurredModel> getMotionBlurredModelAt(int id);


protected:
	int attriIndex;
	osg::ref_ptr<osg::Projection> projectionMatrixForOffset;

	osg::ref_ptr<osg::Group> offsetGroup;
	osg::ref_ptr<osg::Texture> motionBlurredLayerTexture;
	osg::ref_ptr<osg::Texture> offsetTexture;

	osg::ref_ptr<TextureAccumHandler> textureAccumHandler;

	vector< osg::ref_ptr<MotionBlurredModel> > motionBlurredModels;
};

#endif