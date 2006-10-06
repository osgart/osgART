/*
 * osgART / AR Toolkit for OpenSceneGraph
 * (C) 2004-2006 HIT Lab NZ, University of Canterbury
 *
 * Licensing is governed by the LICENSE.txt which is 
 * part of this library distribution.
 *
 */
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

#define  PD_LOOP   3


template <typename T> 
int Observer2Ideal(	const T dist_factor[4], 
					const T ox, 
					const T oy,
					T *ix, T *iy )
{
    T  z02, z0, p, q, z, px, py;
    register int i = 0;

    px = ox - dist_factor[0];
    py = oy - dist_factor[1];
    p = dist_factor[2]/100000000.0;
    z02 = px*px+ py*py;
    q = z0 = sqrt(px*px+ py*py);

    for( i = 1; ; i++ ) {
        if( z0 != 0.0 ) {
            z = z0 - ((1.0 - p*z02)*z0 - q) / (1.0 - 3.0*p*z02);
            px = px * z / z0;
            py = py * z / z0;
        }
        else {
            px = 0.0;
            py = 0.0;
            break;
        }
        if( i == PD_LOOP ) break;

        z02 = px*px+ py*py;
        z0 = sqrt(px*px+ py*py);
    }

    *ix = px / dist_factor[3] + dist_factor[0];
    *iy = py / dist_factor[3] + dist_factor[1];

    return(0);
}


namespace osgART {


	class ImageUpdateCallback : public osg::NodeCallback
	{
	public:

		ImageUpdateCallback(osg::TextureRectangle* texture, osgART::GenericVideo* video):
			_texture(texture),
			_video(video)
		{			
		}

		virtual void operator()(osg::Node*, osg::NodeVisitor* nv)
		{
			_texture->setImage(_video->getImage().get());        
		}
	    
		protected:
			osg::ref_ptr<osg::TextureRectangle>	_texture;
			osg::ref_ptr<osgART::GenericVideo>	_video;
			
	};

	VideoLayer::VideoLayer(int videoId,int layerD)
		: GenericVideoObject(videoId) ,
		m_layerDepth(layerD),
		m_trackerid_undistort(0)
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
		this->addChild(buildLayer().get());
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

				unsigned int rows = 20, cols = 20;
				float rowSize = m_height / (float)rows;
				float colSize = m_width / (float)cols;
				double x, y, px, py, u, v;

				// new version by Hartmut, should work for both ARToolkit and ART4
				const osgART::CameraParameter p = 
					dynamic_cast<osgART::GenericTracker*>(osgART::TrackerManager::getInstance()->getTracker(this->m_trackerid_undistort))->getIntrinsicParameters();
				

				for (unsigned int r = 0; r < rows; r++) {
					for (unsigned int c = 0; c <= cols; c++) {

						x = c * colSize;
						y = r * rowSize;

						Observer2Ideal(p.dist_factor, x, y, &px, &py);
						coords->push_back(osg::Vec3(px, py, 0.0f));

						u = (c / (float)cols) * maxU;
						v = (1.0f - (r / (float)rows)) * maxV;
						tcoords->push_back(osg::Vec2(u, v));

						x = c * colSize;
						y = (r+1) * rowSize;

						Observer2Ideal(p.dist_factor, x, y, &px, &py);
						coords->push_back(osg::Vec3(px, py, 0.0f));

						u = (c / (float)cols) * maxU;
						v = (1.0f - ((r+1) / (float)rows)) * maxV;
						tcoords->push_back(osg::Vec2(u, v));

					}

					m_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
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

			m_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

			break;
		}
	    
		m_geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, _texture, osg::StateAttribute::ON);
		m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		if (m_vShader.valid())
		{
			m_vShader->Apply(*(m_geometry->getOrCreateStateSet()));	
		}
		m_layerGeode->addDrawable(m_geometry.get());

		return m_layerGeode;

	}

};
