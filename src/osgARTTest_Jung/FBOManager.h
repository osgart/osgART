#ifndef FBOMANAGER_H
#define FBOMANAGER_H


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
#include <string>
#include <iostream>
using namespace std;
class FBOManager :  public osg::Group
{
public:
	FBOManager();
	virtual ~FBOManager();

	void init(int w,int h, osg::Group *_root);
	
	bool attachTarget( osg::ref_ptr<osg::Node> renderedNode, int binNum =1000);
	


	osg::ref_ptr<osg::Texture> getTexture(int);
	osg::ref_ptr<osg::CameraNode> getCamera(int);
private:
	osg::Group *root;
	int width, height;

	osg::ref_ptr<osg::Texture> createRenderTexture2D(int w, int h);
	osg::ref_ptr<osg::Texture> createRenderTextureRect(int w, int h);

	osg::ref_ptr<osg::CameraNode> createCamera(osg::Texture *tex, int w, int h);

	int numOfattachedTexture;
	vector< osg::ref_ptr<osg::Texture> > attachedTextures;
	vector< osg::ref_ptr<osg::CameraNode> > cameras;
	

};

#endif
