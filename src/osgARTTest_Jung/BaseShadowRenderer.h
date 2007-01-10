

#ifndef OSGART_BASEBaseShadowRenderer
#define OSGART_BASEBaseShadowRenderer


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

#include <osg/CameraNode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/BlendFunc>

#include <osgART/TrackerManager>
#include <osgART/ARTTransform>

namespace osgART {

	/**
	* \class BaseShadowRenderer.
	* Implement the depth shadow rendering based on depth shadow with GLSL shaders support.
	* This class just defined Virtual to Virtual shadow
	*/
	class BaseShadowRenderer : public osg::Group
	{
		public:        
			/** 
			* \brief default constructor.
			* \param model defined the model
			* \param lightTransform defined the light position 
			* \param markerId is the marker id 	
			*/

			BaseShadowRenderer::BaseShadowRenderer(osg::Node* scene, 
													osg::MatrixTransform* light, 
													osg::ref_ptr<osgART::GenericTracker> tracker,
													int marker,
													int width,
													int height) : 
													m_shadowedScene(scene),
													m_lightTransform(light), 
													m_markerId(marker),
													m_tracker(tracker),
													m_shadowWidth(width),
													m_shadowHeight(height)
			{
			}
	
			BaseShadowRenderer(const BaseShadowRenderer& video);
		    
			/**
			* \brief destructor.
			*/
			virtual ~BaseShadowRenderer(){};


			/**
			* Initialize
			*/    
			virtual void init();
			
			virtual void setShadowResolution(int width, int height) {m_shadowWidth=width;m_shadowHeight=height;};
			void addPhantomNode(osg::ref_ptr<osg::Node>);


			osg::Texture2D* getShadowTextureRef()
			{
				return shadowTextureRef;
			};

			osg::Texture2D* getShadowTexture()
			{
				return shadowTexture;
			};

	protected:
			osg::Node* m_shadowedScene;
			osg::ref_ptr<osg::Group> shadowedSceneWithPhantom;


		private:
			osg::MatrixTransform* m_lightTransform;
			int m_markerId;
			osg::ref_ptr<osgART::GenericTracker> m_tracker;

			int m_shadowWidth;
			int m_shadowHeight;

			osg::Texture2D* shadowTextureRef;			
			osg::Texture2D* shadowTexture;
			osg::Texture2D* modelTexture;
	
	};

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

				osg::Matrix m = osgART::TrackerManager::getInstance()->getTracker(0)->getMarker(markerId)->getTransform();

				m.invert(m);

				MVPT.preMult(m);
				
				
				stateset->getUniform("mvpt")->set(MVPT);

			}
	      
	};
};
#endif