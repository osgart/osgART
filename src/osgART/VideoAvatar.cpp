#include "osgART/VideoAvatar"

#include "osgART/VideoTexture"
#include "osgART/VideoManager"
#include "osgART/VideoTextureRectangle"


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

	VideoAvatar::VideoAvatar(int videoId) : GenericVideoObject(videoId),
		m_videoMatteId(-1),
		m_cropX(-1),
		m_cropY(-1),
		m_cropWidth(-1),
		m_cropHeight(-1),
		m_isCroop(false),
		m_isMatte(false),
		m_isChroma(false)
	{
		m_width=VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_height=VideoManager::getInstance()->getVideo(videoId)->getHeight();
		
		m_matteColor.set(1.0,1.0,1.0);
		m_chromaColor.set(0.0,1.0,0.0);
	}

	VideoAvatar::VideoAvatar(const GenericVideo& video) :
		GenericVideoObject(video.getId()),
		m_width(video.getWidth()), m_height(video.getHeight())
	{
	}

	VideoAvatar::~VideoAvatar(void)
	{
	    
	}

	void 
	VideoAvatar::init()
	{
		this->addChild(buildPlane());
	}

	void 
	VideoAvatar::setTransparency(float alpha) 
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
	VideoAvatar::buildPlane() 
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

	osg::Geode* 
	VideoAvatar::buildGeometry() 
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


	/*		osg::Program* program = new osg::Program;   
	osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);
	fragShader->loadShaderSourceFromFile("skybox.frag");
    program->addShader(fragShader);
	skybox->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);

	osg::Uniform* transParam = new osg::Uniform("trans", 0.0f);
	transParam->setUpdateCallback(new SkyboxShaderCallback(transitionTrans));

	osg::Uniform* eyeParam = new osg::Uniform("eye", osg::Vec3(0, 0, 0));
	eyeParam->setUpdateCallback(new EyeShaderCallback(transitionTrans));
	
	m_Root->getOrCreateStateSet()->addUniform(new osg::Uniform("texture", 0));
    m_Root->getOrCreateStateSet()->addUniform(transParam);
    m_Root->getOrCreateStateSet()->addUniform(eyeParam);
*/
	if (m_isChroma)
	{
		osg::Program* program = new osg::Program;
		osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);
		fragShader->loadShaderSourceFromFile("chromakey.frag");
		program->addShader(fragShader);
		m_geometry->getOrCreateStateSet()->setAttributeAndModes( program, osg::StateAttribute::ON );
		m_geometry->getOrCreateStateSet()->addUniform(new osg::Uniform("texture", 0));

		setTransparency(1.0);
	}
	else
	{
		if (m_isMatte)
		{
			VideoTextureRectangle* m_vTexture2 = new VideoTextureRectangle(m_videoMatteId);
			m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(1, m_vTexture2, osg::StateAttribute::ON);

			osg::Program* program = new osg::Program;
			osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);
			fragShader->loadShaderSourceFromFile("mattecomposite.frag");
			program->addShader(fragShader);
			m_geometry->getOrCreateStateSet()->setAttributeAndModes( program, osg::StateAttribute::ON );
			m_geometry->getOrCreateStateSet()->addUniform(new osg::Uniform("texture", 0));
			m_geometry->getOrCreateStateSet()->addUniform(new osg::Uniform("texture2", 1));
		}
	}
	
	//osg::Uniform* alphaUniform = new osg::Uniform("alpha", getVideoAlpha());
	//alphaUniform->setUpdateCallback(new AlphaCallback());
	//m_videoBackground->getOrCreateStateSet()->addUniform(alphaUniform);

	backGeode->addDrawable(m_geometry);

	return backGeode;

	}

};
