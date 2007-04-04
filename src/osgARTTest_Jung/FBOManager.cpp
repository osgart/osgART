#include "FBOManager.h"

FBOManager::FBOManager()
{
	
}

FBOManager::~FBOManager()
{
}

osg::ref_ptr<osg::CameraNode> FBOManager::getCamera(int id)
{
	return cameras.at(id);
}

osg::ref_ptr<osg::Texture> FBOManager::getTexture(int id)
{
	return attachedTextures.at(id);
}

int FBOManager::size()
{
	return attachedTextures.size();
}

void FBOManager::init(int w,int h, osg::Group *_root, bool _useFloatTexture, GLuint _texInternalFormat)
{
	width = w;
	height = h;
	
	root = _root;
   
	getOrCreateStateSet()->setRenderBinDetails(1000, "RenderBin");


	useFloatTexture = _useFloatTexture;
	texInternalFormat = _texInternalFormat;

	root->addChild(this);
}

bool FBOManager::attachTarget( osg::ref_ptr<osg::Node> renderedNode, int binNum, osg::Vec4 bgColor)
{
	int size = (int)attachedTextures.size();
	int maxNum = GL_MAX_COLOR_ATTACHMENTS_EXT;

	if ( size >= maxNum )
	{
		std::cerr << "Cannot attach texture : Max texture number is" << maxNum << std::endl; 
		return false;
	}
	
	osg::ref_ptr<osg::Texture> tex;
	
	//tex = createRenderTexture2D(width, height); 
	tex = createRenderTexture2D(width, height); 

	attachedTextures.push_back(tex);

	osg::ref_ptr<osg::CameraNode> camera = createCamera(tex.get(), width, height);
	camera->setClearColor(bgColor);
	cameras.push_back(camera);

	// attach the subgraph
    camera->addChild(renderedNode.get());
	camera->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");
	renderedNode->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");

	//this->addChild(camera.get());
	root->addChild(camera.get());
	return true;
}

osg::ref_ptr<osg::Texture> FBOManager::createRenderTexture2D(int w, int h) 
{
	osg::ref_ptr<osg::Texture> t = new osg::Texture2D;

	osg::ref_ptr< osg::Texture2D > tex = dynamic_cast< osg::Texture2D* > (t.get());
	
	tex->setTextureSize(w, h);
	tex->setSourceFormat(GL_RGBA);

	if ( useFloatTexture )
	{
		tex->setInternalFormat(texInternalFormat); //!!!
		tex->setSourceType(GL_FLOAT);//!!!
	}
	else
	{
		tex->setInternalFormat(GL_RGBA);
	}


	//tex->setInternalFormat(GL_RGBA32F_ARB);
	//GL_RGBA_FLOAT32_ATI	

	tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR); 
	//tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST_MIPMAP_LINEAR);
	//tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST_MIPMAP_LINEAR);

	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	
	return t;
}

osg::ref_ptr<osg::Texture> FBOManager::createRenderTextureRect(int w, int h)
{
	osg::ref_ptr<osg::Texture> t = new osg::TextureRectangle;
	
	osg::ref_ptr< osg::TextureRectangle > tex = dynamic_cast< osg::TextureRectangle* > (t.get());
	
	tex->setTextureSize(w, h);
	tex->setInternalFormat(GL_RGBA);
	tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR); 
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	
	return t;
}

osg::ref_ptr<osg::CameraNode> FBOManager::createCamera(osg::Texture *tex, int w, int h)
{
	osg::ref_ptr<osg::CameraNode> camera = new osg::CameraNode;
	camera->setClearColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewport(0, 0, w, h);
	camera->setRenderOrder(osg::CameraNode::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);
	camera->attach(osg::CameraNode::COLOR_BUFFER, tex);
	
	//camera->setProjectionMatrix(proj);
	//camera->setViewMatrix(view);
	return camera;
}
