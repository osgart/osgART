///////////////////////////////////////////////////////////////////////////////
// File name : PlaneARShadowRenderer.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O ??
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////

#include <osgART/PlaneARShadowRenderer>
#include <osgART/VideoTexture>
#include <osgART/VideoManager>
#include <osgART/VideoTextureRectangle>
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/TextureRectangle>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>

#include <osg/CameraNode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>

#include <osgART/TrackerManager>

namespace osgART {

	PlaneARShadowRenderer::PlaneARShadowRenderer(osg::Node* scene,osg::MatrixTransform* light,osg::Vec2f size,int marker) : 
ShadowRenderer(scene,light,marker),m_planeSize(size)
	{

	}

	PlaneARShadowRenderer::~PlaneARShadowRenderer(void)
	{
	    
	}

	void 
	PlaneARShadowRenderer::init()
	{   
		ShadowRenderer::init();
	//HACK we need to construct a scene here from the size parameters
		dynamic_cast<osg::Group*>(m_shadowedScene)->addChild(osgDB::readNodeFile("myplane.ive"));
		m_shadowedScene->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
	}


	///////////////////////////////////////////////////////////////////////////////
	// PROTECTED : Services
	///////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////
	// PRIVATE : Services
	///////////////////////////////////////////////////////////////////////////////

}