/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "osgART/ShadowRenderer"

#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/BlendFunc>

#include <osg/CameraNode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/BlendFunc>


namespace osgART {

	ShadowRenderer::ShadowRenderer(osg::Node* scene, 
		osg::MatrixTransform* light, 
		int marker) : osg::Group(),
		m_shadowedScene(scene),
		m_lightTransform(light), 
		m_markerId(marker),
		m_shadowWidth(2048),
		m_shadowHeight(2048)
	{		
	}

	ShadowRenderer::ShadowRenderer(const ShadowRenderer& srenderer)
	{
	}

	ShadowRenderer::~ShadowRenderer()
	{	    
	}


	
class UpdateCameraAndTexGenCallback : public osg::NodeCallback
{

    protected:
        virtual ~UpdateCameraAndTexGenCallback() {}
        osg::ref_ptr<osg::MatrixTransform>  _light_transform;
        osg::ref_ptr<osg::CameraNode>       camNode;
        osg::ref_ptr<osg::StateSet>       stateset;
		int markerId;

    public:
    
        UpdateCameraAndTexGenCallback(osg::MatrixTransform* light_transform, osg::CameraNode* cam, osg::StateSet* ss,int marker_id):
            _light_transform(light_transform),
            camNode(cam),
            stateset(ss),
			markerId(marker_id){ }
       
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

            traverse(node,nv);
            
            // now compute the camera's view and projection matrix to point at the shadower (the camera's children)
            osg::BoundingSphere bs;
            for (unsigned int i = 0; i < camNode->getNumChildren(); ++i) {
				bs.expandBy(camNode->getChild(i)->getBound());
            }

            if (!bs.valid()) return;
  
            osg::Vec3 position = _light_transform->getMatrix().getTrans();

            float centerDistance = (position - bs.center()).length();
            float znear = centerDistance - bs.radius();
            float zfar  = centerDistance + bs.radius();
            float zNearRatio = 0.001f;
            if (znear<zfar*zNearRatio) znear = zfar * zNearRatio;
            float top   = (bs.radius() / centerDistance) * znear;
            float right = top;

            camNode->setReferenceFrame(osg::CameraNode::ABSOLUTE_RF);
            camNode->setProjectionMatrixAsFrustum(-right, right, -top ,top, znear, zfar);
            camNode->setViewMatrixAsLookAt(position, bs.center(), osg::Vec3(0.0f, 1.0f, 0.0f));

            osg::Matrix MVPT = camNode->getViewMatrix() * 
                               camNode->getProjectionMatrix() *
                               osg::Matrix::translate(1.0,1.0,1.0) *
                               osg::Matrix::scale(0.5f,0.5f,0.5f);

			// TODO: replace!
			/*osg::Matrix m = osgART::TrackerManager::getInstance()->getTracker(0)->getMarker(markerId)->getTransform();*/
			osg::Matrix m;

			m.invert(m);

			MVPT.preMult(m);
			
			
			stateset->getUniform("mvpt")->set(MVPT);

        }
      
};

	void 
	ShadowRenderer::init()
	{   
  
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setTextureSize(m_shadowWidth, m_shadowHeight);

		texture->setInternalFormat(GL_DEPTH_COMPONENT);
		texture->setShadowComparison(true);
		texture->setShadowTextureMode(osg::Texture2D::LUMINANCE);
		texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		texture->setBorderColor(osg::Vec4(1, 1, 1, 1));  // HACK JULIAN <-- why does this work!?
		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_BORDER);
		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_BORDER);
	    
		osg::CameraNode* camera = new osg::CameraNode;
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		camera->setComputeNearFarMode(osg::CameraNode::DO_NOT_COMPUTE_NEAR_FAR);
		camera->setViewport(0, 0, m_shadowWidth, m_shadowHeight);
		camera->setRenderOrder(osg::CameraNode::PRE_RENDER);
		camera->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);
		camera->attach(osg::CameraNode::DEPTH_BUFFER, texture);

		osg::StateSet* camStateSet = camera->getOrCreateStateSet();
		camStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		camStateSet->setAttribute(new osg::PolygonOffset(4, 8), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		camStateSet->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		camera->addChild(m_shadowedScene);
		this->addChild(camera);

		osg::Group* shadowedGroup = new osg::Group;
		shadowedGroup->addChild(m_shadowedScene);
		this->addChild(shadowedGroup);
	            
		osg::StateSet* stateset = shadowedGroup->getOrCreateStateSet();
		stateset->setTextureAttributeAndModes(1, texture, osg::StateAttribute::ON);

		osg::Program* program = new osg::Program;   

		osg::Shader* vertShader = new osg::Shader(osg::Shader::VERTEX);

		// TODO: replace with a static string
		vertShader->loadShaderSourceFromFile("shaders/shadow.vert");
		program->addShader(vertShader);

		osg::Shader* fragShader = new osg::Shader(osg::Shader::FRAGMENT);

		// TODO: replace with a static string
		fragShader->loadShaderSourceFromFile("shaders/shadow.frag");
		program->addShader(fragShader);
		
		stateset->setAttribute(program);
		stateset->addUniform(new osg::Uniform("baseTexture", 0));
		stateset->addUniform(new osg::Uniform("shadowTexture", 1));
		stateset->addUniform(new osg::Uniform("mvpt", osg::Matrix::identity()));

		this->setUpdateCallback(new UpdateCameraAndTexGenCallback(m_lightTransform, camera, stateset,m_markerId));

		this->addChild(m_lightTransform);
	}


}
