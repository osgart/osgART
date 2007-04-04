#ifndef TextureAccumHandler_H
#define TextureAccumHandler_H

#include <osg/Node>
#include <osg/Group>

#include <osg/GL>
#include <osg/Viewport>
#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/MatrixTransform>
#include <osg/FrameBufferObject>
#include <osg/Node>
#include <osg/CameraNode>

#include <vector>
using namespace std;

#include "FBOManager.h"
#include "DummyImageLayer.h"
#include "ShaderFactory.h"

class TextureAccumHandler : public osg::Group
{

public:
	TextureAccumHandler();
	virtual ~TextureAccumHandler();

	void init(int w,int h, osg::Group *_root, int _numRepeate, int colNum, int rowNum, int _startingBinNum = 1300);
	
	void setSeedTexture(osg::ref_ptr<osg::Texture> _tex);
	void setTextureInput(osg::ref_ptr<osg::Texture> _tex, int indexForLayer, int indexForTexture);

	osg::ref_ptr<osg::Texture> getResultTexture();

	vector< osg::ref_ptr<DummyImageLayer> > getLayers();

	void addNodeAt(int id, osg::ref_ptr<osg::Node> nd,  bool b4DummyLayer);
private:
	
	osg::ref_ptr<FBOManager> fboManager;
	vector< osg::ref_ptr<DummyImageLayer> > layers;
	vector< osg::ref_ptr<osg::Group> > groupForLayers;


	osg::Group *root;
	int width, height;
	int numRepeate;
	int startingBinNum;

};

class TextureAccumHandlerCallback : public osg::NodeCallback
{
public:
	TextureAccumHandlerCallback();
	virtual ~TextureAccumHandlerCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
};


#endif