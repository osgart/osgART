#include <osgART/VideoPlaneMulti>
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


namespace osgART {

	VideoPlaneMulti::VideoPlaneMulti(int width, 
		int height) 
		: GenericVideoObject(0),
		m_width(width),
		m_height(height)
	{		
	}

	VideoPlaneMulti::~VideoPlaneMulti()	{	    
	}

	void 
	VideoPlaneMulti::init()
	{
		this->addChild(buildPlane());
	}

	void 
	VideoPlaneMulti::setTransparency(float alpha) 
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
	VideoPlaneMulti::buildPlane() 
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

	void VideoPlaneMulti::addVideo(int videoId) {
		this->m_LayerVideoID.push_back(videoId);
	}


	osg::Geode* 
	VideoPlaneMulti::buildGeometry() 
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
				std::cerr << "VideoBackground::buildBackGeometry(): Error, unknown texture mode" << std::endl;
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
	   
		std::vector<int>::iterator _ii = m_LayerVideoID.begin();

		int _i = 0;

		while (_ii != m_LayerVideoID.end()) {

			osg::Texture* _t = 0L; 

			// negative ids are for skipping a texture unit
			if (*_ii >= 0) {
				if (m_textureMode == USE_TEXTURE_RECTANGLE) {
					_t = new VideoTextureRectangle(*_ii);
				} else 
				if (m_textureMode == USE_TEXTURE_2D) {
					_t = new VideoTexture(*_ii);
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
			m_vShader->Apply(*(m_geometry->getOrCreateStateSet()));	
		}

		backGeode->addDrawable(m_geometry);

		return backGeode;

	}

};
