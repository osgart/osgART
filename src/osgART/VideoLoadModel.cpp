#include "osgART/VideoLoadModel"

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

#include <osgDB/ReadFile>

namespace osgART {

	VideoLoadModel::VideoLoadModel(int videoId,const char* model_name) : GenericVideoObject(videoId),
		m_videoMatteId(-1),
		m_cropX(-1),
		m_cropY(-1),
		m_cropWidth(-1),
		m_cropHeight(-1),
		m_isCroop(false),
		m_isMatte(false),
		m_isChroma(false),
		m_modelName(model_name)
	{
		m_width=VideoManager::getInstance()->getVideo(videoId)->getWidth();
		m_height=VideoManager::getInstance()->getVideo(videoId)->getHeight();
		
		m_matteColor.set(1.0,1.0,1.0);
		m_chromaColor.set(0.0,1.0,0.0);
	}

	VideoLoadModel::VideoLoadModel(const GenericVideo& video) :
		GenericVideoObject(video.getId()),
		m_width(video.getWidth()), m_height(video.getHeight())
	{
	}

	VideoLoadModel::~VideoLoadModel(void)
	{
	    
	}

	void 
	VideoLoadModel::init()
	{
		this->addChild(buildVideoModel());
	}

	osg::Node*
	VideoLoadModel::buildVideoModel() 
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

	osg::Node* 
	VideoLoadModel::buildGeometry() 
	{
		m_vTexture = new VideoTexture(m_videoId);
		
		m_geometry=osgDB::readNodeFile(m_modelName);
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

		osg::BlendFunc* blendFunc = new osg::BlendFunc();
		blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	
		m_geometry->getOrCreateStateSet()->setAttribute(blendFunc);
		m_geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		m_geometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
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

	//backGeode->addDrawable(m_geometry);

	return m_geometry;//backGeode;

	}

};
