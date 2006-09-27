///////////////////////////////////////////////////////////////////////////////
// File name : VideoLayer.C
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

//HACK RAPH
// #include "osgART/ARToolKitTracker"
// #include <osgART/ARToolKit4Tracker>

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

#include <AR/param.h>


//#include "util.h"

namespace osgART {


class ImageUpdateCallback : public osg::NodeCallback
{
public:

	ImageUpdateCallback(osg::TextureRectangle* texture, osgART::GenericVideo* video):
        _texture(texture),
		_video(video)
    {
		if (video->getImage()) {
			std::cout << (long)video->getImage()->data() << std::endl;
		}
    }

    virtual void operator()(osg::Node*, osg::NodeVisitor* nv)
    {
		_texture->setImage(_video->getImage());        
    }
    
	protected:
		osg::ref_ptr<osg::TextureRectangle>	_texture;
		osg::ref_ptr<osgART::GenericVideo>	_video;
		
};



	VideoLayer::VideoLayer(int videoId,int layerD)
		: GenericVideoObject(videoId) ,
		m_layerDepth(layerD)
	{
		// Should check whether it's a valid video id!
		// m_videoId = videoId;
		
		m_width = VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_height = VideoManager::getInstance()->getVideo(videoId)->getHeight();

	}

	VideoLayer::~VideoLayer()
	{	    
	}

	/* virtual */
	void
	VideoLayer::init()
	{
		// add as a child
		this->addChild(buildLayer());
	}

	void 
	VideoLayer::setTransparency(float alpha) 
	{
		m_alpha=alpha;
		if (alpha<1.0f) //if no transparency, non activate blending op
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


	osg::Node* 
	VideoLayer::buildLayer() 
	{
		osg::Projection* projection = new osg::Projection(osg::Matrix::ortho2D(0, m_width, 0, m_height));

		osg::MatrixTransform* modelview = new osg::MatrixTransform();
		modelview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		projection->addChild(modelview);

		osg::Group* layerGroup = new osg::Group();
		modelview->addChild(layerGroup);

		m_layerStateSet = new osg::StateSet();
		layerGroup->setStateSet(m_layerStateSet);

		setLayerDepth(m_layerDepth);
		layerGroup->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));
		layerGroup->addChild(buildLayerGeometry());

		return projection;
	}


	osg::Geode* 
	VideoLayer::buildLayerGeometry() 
	{
		float maxU = 1.0f, maxV = 1.0f;

		osg::Texture* _texture = NULL;

		switch(m_textureMode) {
			case USE_TEXTURE_RECTANGLE:
				maxU = m_width;
				maxV = m_height;
				_texture = new VideoTextureRectangle(m_videoId);
				break;
			case USE_TEXTURE_2D:
				maxU = m_width / (float)mathNextPowerOf2((unsigned int)m_width);
				maxV = m_height / (float)mathNextPowerOf2((unsigned int)m_height);
				_texture = new VideoTexture(m_videoId);
				break;

			case USE_TEXTURE_VIDEO:

				_texture = new osg::TextureRectangle;
				this->setUpdateCallback(new ImageUpdateCallback((osg::TextureRectangle*)_texture,
					VideoManager::getInstance()->getVideo(m_videoId)));
				
				break;

			default:
				std::cerr << "VideoBackground::buildBackGeometry(): Error, unknown texture mode" << std::endl;
		}

		this->m_vTexture = _texture;
		_texture->setDataVariance(osg::Object::DYNAMIC);

		osg::Geode* backGeode = new osg::Geode();
		osg::Geometry* geometry = new osg::Geometry();
			
		osg::Vec3Array* coords = new osg::Vec3Array();
		geometry->setVertexArray(coords);

		osg::Vec2Array* tcoords = new osg::Vec2Array();
		geometry->setTexCoordArray(0, tcoords);

		osg::Vec4Array* colors = new osg::Vec4Array();
		colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		geometry->setColorArray(colors);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		switch (m_distortionMode) {

			case CAMERA_PARAM_CORRECTION:
			{
				// create grid with radial texture correction

				unsigned int rows = 20, cols = 20;
				float rowSize = m_height / (float)rows;
				float colSize = m_width / (float)cols;
				double x, y, px, py, u, v;

				// new version by Hartmut, should work for both ARToolkit and ART4
				const osgART::CameraParameter p = 
					dynamic_cast<osgART::GenericTracker*>(osgART::TrackerManager::getInstance()->getTracker(0))->getIntrinsicParameters();
				

				for (unsigned int r = 0; r < rows; r++) {
					for (unsigned int c = 0; c <= cols; c++) {

						x = c * colSize;
						y = r * rowSize;

						arParamObserv2Ideal(p.dist_factor, x, y, &px, &py);
						coords->push_back(osg::Vec3(px, py, 0.0f));

						u = (c / (float)cols) * maxU;
						v = (1.0f - (r / (float)rows)) * maxV;
						tcoords->push_back(osg::Vec2(u, v));

						x = c * colSize;
						y = (r+1) * rowSize;

						arParamObserv2Ideal(p.dist_factor, x, y, &px, &py);
						coords->push_back(osg::Vec3(px, py, 0.0f));

						u = (c / (float)cols) * maxU;
						v = (1.0f - ((r+1) / (float)rows)) * maxV;
						tcoords->push_back(osg::Vec2(u, v));

					}

					geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
						r * 2 * (cols+1), 2 * (cols+1)));

				}

				break;
			}

		case NO_CORRECTION:

			coords->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
			coords->push_back(osg::Vec3(m_width, 0.0f, 0.0f));
			coords->push_back(osg::Vec3(m_width, m_height, 0.0f));
			coords->push_back(osg::Vec3(0.0f, m_height, 0.0f));

			tcoords->push_back(osg::Vec2(0.0f, maxV));
			tcoords->push_back(osg::Vec2(maxU, maxV));
			tcoords->push_back(osg::Vec2(maxU, 0.0f));
			tcoords->push_back(osg::Vec2(0.0f, 0.0f));

			geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

			break;
		}
	    
		geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, _texture, osg::StateAttribute::ON);
		geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		backGeode->addDrawable(geometry);

		return backGeode;

	}

};
