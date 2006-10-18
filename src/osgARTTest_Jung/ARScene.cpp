#include "ARScene.h"

ARScene::ARScene()
{
	background = new osg::Group();       
	backgroundTextureBuffer = new osg::Group(); 
	backgroundTextureBuffer->ref();

	foreground = new osg::Group(); 
	

	background->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");
	foreground->getOrCreateStateSet()->setRenderBinDetails(20 , "RenderBin");
	
	fboManager = new FBOManager;
}

ARScene::~ARScene()
{
}

void ARScene::init(osg::ref_ptr<osgART::GenericTracker> tracker, int _trakerID)
{
	projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	projectionMatrixForFBO = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
	trackerID = _trakerID;

	this->addChild(projectionMatrix.get());
	
	projectionMatrix->addChild( foreground.get() );
	backgroundTextureBuffer->addChild(projectionMatrixForFBO.get());

	this->addChild( background.get() );
}
	
void ARScene::addARNode(osg::ref_ptr<ARNode> arNode, int binNum,  bool addToSceneGraph )
{	
	arNodes.push_back(arNode);
	arNode->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");

	if ( addToSceneGraph )
		foreground->addChild( arNode.get() );	
}
	
osg::ref_ptr<ARNode> ARScene::addNewARNodeWith(osg::ref_ptr<osg::Node> node, int binNum)
{
	osg::ref_ptr<ARNode> arNode = new ARNode;
	int lastIndex = (int)arNodes.size();

	arNode->init(lastIndex, trackerID);
	arNode->addModel(node);
	
	addARNode( arNode, binNum );
	return arNode;
}

osg::ref_ptr<osgART::VideoBackground> ARScene::makeVideoBackground(int id)
{
		
	osg::ref_ptr<osgART::VideoBackground> videoBackground = new osgART::VideoBackground(id);
	videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);
	videoBackground->init();

	videoBackground->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");
	
	bgWidth   = videoBackground->getWidth();
	bgHeight  = videoBackground->getHeight();
	
	return videoBackground;
}

osg::ref_ptr<osgART::VideoBackground> ARScene::initDefaultVideoBackground(int id)
{
	osg::ref_ptr<osgART::VideoBackground> videoBackground = makeVideoBackground(id);
	background->addChild(videoBackground.get());
	
	return videoBackground;
}

osg::ref_ptr<osg::Texture> ARScene::initTextureVideoBackground(int id, bool addDummyLayer)
{
	osg::ref_ptr<osgART::VideoBackground> videoBackground = makeVideoBackground(id);
	backgroundTextureBuffer->addChild(videoBackground.get());

	fboManager->init(bgWidth, bgHeight, this);
	
	fboManager->attachTarget( backgroundTextureBuffer.get(), 1000);
	videoBackgroundTexture = fboManager->getTexture(0);

	if ( addDummyLayer )
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(bgWidth,bgHeight);
		dummy->setTexture( videoBackgroundTexture.get() );	
		dummy->getOrCreateStateSet()->setRenderBinDetails(5 , "RenderBin");

		background->addChild( dummy.get() );
	}

	return videoBackgroundTexture;
}

void ARScene::addToBackgroundTextureGroup(osg::Node *aNode, bool isARNode)
{
	
	if ( isARNode )
		projectionMatrixForFBO->addChild(aNode);
	else
		backgroundTextureBuffer->addChild(aNode);
}

void ARScene::addToBackgroundGroup(osg::Node *aNode)
{
	background->addChild(aNode);
}


osg::ref_ptr<ARNode> ARScene::at(int id)
{
	return arNodes.at(id);
}
int ARScene::size()
{
	return arNodes.size();
}