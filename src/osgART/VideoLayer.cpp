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
#include "osgART/VideoManager"
#include "osgART/GenericTracker"

#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Notify>
#include <osg/Image>


namespace osgART {


	class Texture2DCallback : public osg::Texture2D::SubloadCallback
	{
	public:
		
		Texture2DCallback(osg::Texture2D* texture);

		void load(const osg::Texture2D& texture, osg::State&) const;
		void subload(const osg::Texture2D& texture, osg::State&) const;

		inline float getTexCoordX() const { return (_texCoordX);};
		inline float getTexCoordY() const { return (_texCoordY);};

	protected:
		
		float _texCoordX;
		float _texCoordY;
		
	};


	Texture2DCallback::Texture2DCallback(osg::Texture2D* texture) :
		_texCoordX(texture->getImage()->s() / (float)GenericVideoObject::mathNextPowerOf2((unsigned int)texture->getImage()->s())),
		_texCoordY(texture->getImage()->t() / (float)GenericVideoObject::mathNextPowerOf2((unsigned int)texture->getImage()->t()))
	{
		texture->setTextureSize(GenericVideoObject::mathNextPowerOf2((unsigned int)texture->getImage()->s()),
			GenericVideoObject::mathNextPowerOf2((unsigned int)texture->getImage()->t()));

		texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP);
		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP);
		
	}

	/*virtual*/ 
	void 
	Texture2DCallback::load(const osg::Texture2D& texture, osg::State&) const 
	{
		
		const osg::Image* _image = texture.getImage();

		glTexImage2D(GL_TEXTURE_2D, 0, 
			// hse25: internal texture format gets overwritten by the image format 
			// we need just the components - ???
			osg::Image::computeNumComponents(_image->getInternalTextureFormat()), 
			(float)GenericVideoObject::mathNextPowerOf2((unsigned int)_image->s()), 
			(float)GenericVideoObject::mathNextPowerOf2((unsigned int)_image->t()), 
			0, _image->getPixelFormat(), 
			_image->getDataType(), 0);
	}
	
	/*virtual */ 
	void
	Texture2DCallback::subload(const osg::Texture2D& texture, osg::State&) const 
	{
	
		const osg::Image* _image = texture.getImage();

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
						_image->s(), _image->t(), _image->getPixelFormat(), 
						_image->getDataType(), _image->data());

	}


	// VideoLayer

	VideoLayer::VideoLayer(
		osg::Image* image /* = 0L*/,
		int layerD)
		: GenericVideoObject(image),
		m_width(image ? image->s() : 0),
		m_height(image ? image->t() : 0),
		m_layerDepth(layerD),
		m_alpha(-1),
		m_trackerid_undistort(0)
	{
	}


	VideoLayer::VideoLayer(const VideoLayer& videolayer,
		const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/) : 
		GenericVideoObject(videolayer,copyop)
	{
	}


	VideoLayer::~VideoLayer()
	{	    
	}


	/* virtual */
	void
	VideoLayer::setImageSource(osg::Image* image)
	{
		GenericVideoObject::setImageSource(image);

		m_width = (image) ? image->t() : 0;
		m_height = (image) ? image->s() : 0;
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

		osg::Texture* _texture = 0L;

		float maxU = 1.0f, maxV = 1.0f;

		switch (m_textureMode) 
		{
		case GenericVideoObject::USE_TEXTURE_RECTANGLE:
			
			osg::notify() << "osgART::GenericVideoObject() using TextureRectangle" << std::endl;

			_texture = new osg::TextureRectangle(this->m_image.get());
			
			maxU = m_image->s();
			maxV = m_image->t();
			
			break;

		case GenericVideoObject::USE_TEXTURE_DEFAULT:
		case GenericVideoObject::USE_TEXTURE_2D:
		default:

			osg::notify() << "osgART::GenericVideoObject() using Texture2D" << std::endl;

			_texture = new osg::Texture2D(this->m_image.get());

			Texture2DCallback *_cb = new Texture2DCallback(dynamic_cast<osg::Texture2D*>(_texture));

			maxU = _cb->getTexCoordX();
			maxV = _cb->getTexCoordY();

			dynamic_cast<osg::Texture2D*>(_texture)->setSubloadCallback(_cb);
		}


		_texture->setDataVariance(osg::Object::DYNAMIC);

		m_layerGeode = new osg::Geode();

		m_geometry = new osg::Geometry();
		
		osg::Vec3Array* coords = new osg::Vec3Array();
		m_geometry->setVertexArray(coords);

		osg::Vec2Array* tcoords = new osg::Vec2Array();
		m_geometry->setTexCoordArray(0, tcoords);

		switch (m_distortionMode) {

			case CAMERA_PARAM_CORRECTION:
			{
				// create grid with radial texture correction
				TrackerManager::getInstance()->getTracker(this->m_trackerid_undistort)->
					createUndistortedMesh((int)m_width, (int)m_height, maxU, maxV, *m_geometry);
			}

			default:
				osg::notify() << "osgART::VideoLayer::buildLayerGeometry()"
					"Undefined distortion mode" << std::endl;
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
		}
	    
		m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, 
			_texture, osg::StateAttribute::ON);

		m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, 
			osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		
		m_layerGeode->addDrawable(m_geometry.get());

		return m_layerGeode;

	}

};
