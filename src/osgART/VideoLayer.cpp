/*
 *	osgART/VideoLayer
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include "osgART/VideoLayer"
#include "osgART/TrackerManager"
#include "osgART/VideoTexture"
#include "osgART/VideoManager"
#include "osgART/VideoTextureRectangle"
#include "osgART/GenericTracker"

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
#include <osg/Notify>

namespace osgART {


	class ImageUpdateCallback : public osg::NodeCallback
	{
	public:

		ImageUpdateCallback(osg::TextureRectangle* texture, osgART::GenericVideo* video):
			_texture(texture),
			_video(video)
		{			
		}

		virtual void operator()(osg::Node*, osg::NodeVisitor*)
		{
			_texture->setImage(_video.get());        
		}
	    
		protected:
			osg::ref_ptr<osg::TextureRectangle>	_texture;
			osg::ref_ptr<osgART::GenericVideo>	_video;
			
	};

	VideoLayer::VideoLayer(
		GenericVideo* video /* = 0L*/,
		int layerD)
		: GenericVideoObject(video) ,
		m_layerDepth(layerD),
		m_alpha(-1),
		m_trackerid_undistort(0)
	{
		// Should check whether it's a valid video id!
		// m_videoId = videoId;
		
		m_width = video->getWidth();
		m_height = video->getHeight();

	}



	VideoLayer::~VideoLayer()
	{	    
	}

	/* virtual */
	void
	VideoLayer::init()
	{
		// add as a child
		this->addChild(buildLayer().get());
	}

	void 
	VideoLayer::setTransparency(float alpha) 
	{
		m_alpha=alpha;
		if (alpha<1.0f) //if no transparency, non activate blending op (if already activate, override)
		{
			osg::BlendFunc* blendFunc = new osg::BlendFunc();
			blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		
			osg::StateSet* stateset = m_geometry->getStateSet();
		
			stateset->setAttribute(blendFunc);
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,alpha));
		m_geometry->setColorArray(colors);
		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	}

	void 
	VideoLayer::setLayerDepth(int level)
	{
		m_layerDepth=level;
		m_layerStateSet->setRenderBinDetails(m_layerDepth, "RenderBin");
	}
	
	osg::ref_ptr<osg::Projection>
	VideoLayer::buildLayer() 
	{
		m_layerProjectionMatrix = new osg::Projection(osg::Matrix::ortho2D(0, m_width, 0, m_height));

		m_layerModelViewMatrix = new osg::MatrixTransform();
		m_layerModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		m_layerProjectionMatrix->addChild(m_layerModelViewMatrix.get());

		osg::Group* layerGroup = new osg::Group();
		m_layerModelViewMatrix->addChild(layerGroup);

		m_layerStateSet = new osg::StateSet();
		layerGroup->setStateSet(m_layerStateSet.get());

		setLayerDepth(m_layerDepth);
		layerGroup->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));
		layerGroup->addChild(buildLayerGeometry().get());

		return m_layerProjectionMatrix;
	}


	osg::ref_ptr<osg::Geode>
	VideoLayer::buildLayerGeometry() 
	{
		float maxU = 1.0f, maxV = 1.0f;

		osg::Texture* _texture = NULL;

		switch(m_textureMode) {
			case USE_TEXTURE_RECTANGLE:
				maxU = m_width;
				maxV = m_height;
				_texture = new VideoTextureRectangle(m_video.get());
				break;
			case USE_TEXTURE_2D:
				maxU = m_width / (float)mathNextPowerOf2((unsigned int)m_width);
				maxV = m_height / (float)mathNextPowerOf2((unsigned int)m_height);
				_texture = new VideoTexture(m_video.get());
				break;

			case USE_TEXTURE_VIDEO:

				_texture = new osg::TextureRectangle;
				this->setUpdateCallback(new ImageUpdateCallback((osg::TextureRectangle*)_texture,
					m_video.get()));
				
				break;

			default:
				std::cerr << "VideoBackground::buildBackGeometry(): Error, unknown texture mode" << std::endl;
		}

		this->m_vTexture = _texture;
		_texture->setDataVariance(osg::Object::DYNAMIC);

		m_layerGeode = new osg::Geode();

		m_geometry = new osg::Geometry();
		
		osg::Vec3Array* coords = new osg::Vec3Array();
		m_geometry->setVertexArray(coords);

		osg::Vec2Array* tcoords = new osg::Vec2Array();
		m_geometry->setTexCoordArray(0, tcoords);

		osg::Vec4Array* colors = new osg::Vec4Array();
		colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_geometry->setColorArray(colors);
		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		switch (m_distortionMode) {

			case CAMERA_PARAM_CORRECTION:
			{
				// create grid with radial texture correction
				TrackerManager::getInstance()->getTracker(this->m_trackerid_undistort)->
					createUndistortedMesh((int)m_width, (int)m_height, maxU, maxV, *m_geometry);
			}


			case NO_CORRECTION:
			{

				coords->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
				coords->push_back(osg::Vec3(m_width, 0.0f, 0.0f));
				coords->push_back(osg::Vec3(m_width, m_height, 0.0f));
				coords->push_back(osg::Vec3(0.0f, m_height, 0.0f));

				tcoords->push_back(osg::Vec2(0.0f, maxV));
				tcoords->push_back(osg::Vec2(maxU, maxV));
				tcoords->push_back(osg::Vec2(maxU, 0.0f));
				tcoords->push_back(osg::Vec2(0.0f, 0.0f));

				m_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

				break;
			}
			default:
				osg::notify() << "osgART::VideoLayer::buildLayerGeometry()"
					"Undefined distortion mode" << std::endl;
		}
	    
		m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, _texture, osg::StateAttribute::ON);
		m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		
		//if we have transparency texture and we are not the background layer
		if ((dynamic_cast<osgART::VideoTextureBase*>(_texture)->getVideo()->pixelSize()==4)&&(m_layerDepth!=1))
		{
			osg::BlendFunc* blendFunc = new osg::BlendFunc();
			blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		
			m_geometry->getOrCreateStateSet()->setAttribute(blendFunc);
			m_geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			m_geometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			dynamic_cast<osgART::VideoTextureBase*>(_texture)->setAlphaBias(0.0);	
		}

		if (m_vShader.valid())
		{
			m_vShader->apply(*(m_geometry->getOrCreateStateSet()));	
		}
		m_layerGeode->addDrawable(m_geometry.get());

		return m_layerGeode;

	}

};
