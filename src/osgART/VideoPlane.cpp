/*
 *	osgART/VideoPlane
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

#include "osgART/VideoPlane"
#include "osgART/VideoManager"

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
#include <osg/Texture2D>
#include <osg/TextureRectangle>

namespace osgART {

	namespace _duplicatedcodehack {

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

	}

	VideoPlane::VideoPlane(GenericVideo* video)		
		: GenericVideoObject(video),
		m_width(video ? video->getWidth() : 0),
		m_height(video ? video->getHeight() : 0)
	{		
		if (video) this->m_layervideos.push_back(video);
	}

	VideoPlane::~VideoPlane()
	{	    
	}

	void 
	VideoPlane::init()
	{
		this->addChild(this->buildObject());
	}

	void 
	VideoPlane::setTransparency(float alpha) 
	{

		osg::BlendFunc* blendFunc = new osg::BlendFunc();
		blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		
		osg::StateSet* stateset = m_geometry->getStateSet();
		
		stateset->setAttribute(blendFunc);
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,alpha));
		m_geometry->setColorArray(colors);
		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);


	}

	osg::Node*
	VideoPlane::buildObject() 
	{
		osg::MatrixTransform* modelview = new osg::MatrixTransform();
		modelview->setMatrix(osg::Matrix::identity());
		//modelview->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);

		//osg::Depth* depth = new osg::Depth;
		//depth->setFunction(osg::Depth::ALWAYS);
		//depth->setRange(1.0, 1.0);

		osg::StateSet* backgroundStateSet = new osg::StateSet();        
		backgroundStateSet->setRenderBinDetails(1, "RenderBin");
		//backgroundStateSet->setAttribute(depth);
		
		osg::Group* backgroundGroup = new osg::Group();
		backgroundGroup->setStateSet(backgroundStateSet);
		backgroundGroup->addChild(buildGeometry());

		modelview->addChild(backgroundGroup);

		return modelview;
	}

	void
	VideoPlane::addVideo(GenericVideo* video) 
	{
		this->m_layervideos.push_back(video);
	}


	osg::Geode* 
	VideoPlane::buildGeometry() 
	{
		float maxU = 1.0f, maxV = 1.0f;

		switch(m_textureMode) {
			case USE_TEXTURE_RECTANGLE:
				maxU = m_width;
				maxV = m_height;
				break;
			case USE_TEXTURE_2D:
				maxU = m_width / (float)mathNextPowerOf2((unsigned int)m_width);
				maxV = m_height / (float)mathNextPowerOf2((unsigned int)m_height);
				break;
			default:
				osg::notify(osg::WARN) << "osgART::VideoBackground::buildBackGeometry(): "
					"Error, unknown texture mode" << std::endl;
		}

		osg::Geode* backGeode = new osg::Geode();
		m_geometry = new osg::Geometry();

		osg::Vec3 vCorner = osg::Vec3(0.0f, 0.0f, 0.0f);
		osg::Vec3 vWidth = osg::Vec3(1.0f, 0.0f, 0.0f);
		osg::Vec3 vHeight = osg::Vec3(0.0f, 1.0f, 0.0f);

		osg::Vec3Array* coords = new osg::Vec3Array(4);

		(*coords)[0] = vCorner;
		(*coords)[1] = vCorner + vWidth;
		(*coords)[2] = vCorner + vWidth + vHeight;
		(*coords)[3] = vCorner + vHeight;

		m_geometry->setVertexArray(coords);

		osg::Vec3Array* norms = new osg::Vec3Array(1);
		(*norms)[0] = vWidth ^ vHeight;
		(*norms)[0].normalize();
	    
		m_geometry->setNormalArray(norms);
		m_geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

		osg::Vec2Array* tcoords = new osg::Vec2Array(4);
		(*tcoords)[0].set(0.0f, maxV);
		(*tcoords)[1].set(maxU, maxV);
		(*tcoords)[2].set(maxU, 0.0f);
		(*tcoords)[3].set(0.0f, 0.0f);
		
	    
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		m_geometry->setColorArray(colors);
		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		m_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	   
		VideoArray::iterator _ii = m_layervideos.begin();

		int _i = 0;

		while (_ii != m_layervideos.end()) {

			osg::Texture* _t = 0L; 

			if ((*_ii).valid()) {
				if (m_textureMode == USE_TEXTURE_RECTANGLE) {
					_t = new osg::TextureRectangle((*_ii).get());
					maxU = m_image->s();
					maxV = m_image->t();
				} else 
				
				if (m_textureMode == USE_TEXTURE_2D) {

					osg::notify() << "osgART::GenericVideoObject() using Texture2D" << std::endl;

					_t = new osg::Texture2D((*_ii).get());

					_duplicatedcodehack::Texture2DCallback *_cb = 
						new _duplicatedcodehack::Texture2DCallback(dynamic_cast<osg::Texture2D*>(_t));

					maxU = _cb->getTexCoordX();
					maxV = _cb->getTexCoordY();

					dynamic_cast<osg::Texture2D*>(_t)->setSubloadCallback(_cb);
				}
			}
			if (0L != _t) {

				m_LayerTextures.push_back(_t);

				m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(_i, _t, 
					osg::StateAttribute::ON);
				m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, 
					osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

				m_geometry->setTexCoordArray(_i, tcoords);

				// increase one 		
				++_i;

			}
			++_ii;
		}

		if (m_vShader.valid())
		{
			m_vShader->apply(*(m_geometry->getOrCreateStateSet()));	
		}

		backGeode->addDrawable(m_geometry.get());

		return backGeode;

	}

};
