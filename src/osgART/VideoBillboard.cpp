#include "osgART/VideoBillboard"
#include "osgART/VideoTexture"
#include "osgART/VideoManager"
#include "osgART/VideoTextureRectangle"

#include <osg/Group>
#include <osg/Node>
#include <osg/AutoTransform>
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


	VideoBillboard::VideoBillboard(int videoId)
	{
		m_width=VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_height=VideoManager::getInstance()->getVideo(videoId)->getHeight();
	}

	VideoBillboard::~VideoBillboard(void)
	{
	    
	}

	///////////////////////////////////////////////////////////////////////////////
	// PUBLIC : Interface 
	///////////////////////////////////////////////////////////////////////////////

	void 
	VideoBillboard::init()
	{
		this->addChild(buildBillboard());
	}

	void 
	VideoBillboard::setTransparency(float alpha) 
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
	///////////////////////////////////////////////////////////////////////////////
	// PROTECTED : Services
	///////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////
	// PRIVATE : Services
	///////////////////////////////////////////////////////////////////////////////



	osg::Node*
	VideoBillboard::buildBillboard() 
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

		osg::AutoTransform* billboardTransform = new osg::AutoTransform;
		billboardTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

		billboardTransform->addChild(buildGeometry());
		backgroundGroup->addChild(billboardTransform);

		modelview->addChild(backgroundGroup);

		return modelview;
	}


	osg::Geode* 
	VideoBillboard::buildGeometry() 
	{

		float maxU = 1.0f, maxV = 1.0f;

		switch(m_textureMode) {
			case USE_TEXTURE_RECTANGLE:
				maxU = m_width;
				maxV = m_height;
				m_vTexture = new VideoTextureRectangle(m_videoId);
				break;
			case USE_TEXTURE_2D:
				maxU = m_width / (float)mathNextPowerOf2((unsigned int)m_width);
				maxV = m_height / (float)mathNextPowerOf2((unsigned int)m_height);
				m_vTexture = new VideoTexture(m_videoId);
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
		m_geometry->setTexCoordArray(0, tcoords);
	    
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		m_geometry->setColorArray(colors);
		m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		m_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	   
		m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, m_vTexture, osg::StateAttribute::ON);
		m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		backGeode->addDrawable(m_geometry);

		return backGeode;


	}

};
