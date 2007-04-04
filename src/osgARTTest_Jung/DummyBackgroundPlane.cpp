#include "DummyBackgroundPlane.h"
#include <osg/CullFace>

DummyBackgroundPlane::DummyBackgroundPlane()
{
}

DummyBackgroundPlane::~DummyBackgroundPlane()
{
}

void DummyBackgroundPlane::init(int _width, int _height, int _row, int _col, float _angle)
{
	width  = _width;
	height = _height;
	row    = _row;
	col    = _col;
	angle  = _angle;


	
	//camera = new osg::CameraNode;
	group = new osg::Group;

	
	if ( width < height )
		minVal = width;
	else
		minVal = height;

	minValHalf = minVal / 2.0f;
	
	dist = minValHalf / (float)tan( osg::DegreesToRadians( angle / 2.0f ) );

	m_layerProjectionMatrix = new osg::Projection( osg::Matrix::perspective(angle, 1.0f, 1.0f , 1000) );
	//m_layerProjectionMatrix = new osg::Projection( osg::Matrix::lookAt( osg::Vec3f(0,0,0), osg::Vec3f(0,0, 1), osg::Vec3f(0,1,0)) );
	

	std::cout << "Dist "  << dist << std::endl;

	m_layerModelViewMatrix = new osg::MatrixTransform();
	m_layerModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	
	
	//this->addChild(camera.get());
	//camera->addChild(m_layerProjectionMatrix.get());
	this->addChild(m_layerProjectionMatrix.get());
	m_layerProjectionMatrix->addChild(m_layerModelViewMatrix.get());
	m_layerModelViewMatrix->addChild(group.get());
	group->addChild(buildGeometry().get());
	group->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));
	//camera->setClearColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//camera->setViewport(0, 0, width, height);

	group->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::CullFace* cullFace = new osg::CullFace;
    cullFace->setMode(osg::CullFace::BACK);
  	group->getOrCreateStateSet()->setAttributeAndModes(cullFace, osg::StateAttribute::ON);
}

void DummyBackgroundPlane::setTexture( osg::ref_ptr<osg::Texture> _tex, int id)
{
	group->getOrCreateStateSet()->setTextureAttributeAndModes(id, _tex.get() ,osg::StateAttribute::ON);	
}

osg::ref_ptr<osg::Geode> DummyBackgroundPlane::buildGeometry()
{

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		
	if ( row == 1 && col == 1 )
	{
		
		float halfWidth = minValHalf;
		float halfHeight = minValHalf;

		geode->addDrawable
		(
			osg::createTexturedQuadGeometry
			(
				osg::Vec3(-halfWidth, -halfHeight , -dist ),
				osg::Vec3(minVal, 0, 0), 
				osg::Vec3(0, minVal, 0)
			)
		);
		

		return geode;
	}

	osg::Geometry* backgroundGeometry = new osg::Geometry();

	osg::Vec3Array* backgroundVertices = new osg::Vec3Array;
	osg::Vec2Array* texcoords = new osg::Vec2Array( (row+1) * (col+1));
			osg::DrawElementsUInt* backgroundIndices =
			new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

	float currentWidth, currentHeight;
	float sUnit = 1.0f / (float) col;
	float tUnit = 1.0f / (float) row;

	float xUnit = minVal / (float) col;
	float yUnit = minVal / (float) row;
	float xCoord, yCoord;

	currentWidth  = -minValHalf;
	currentHeight = -minValHalf;
	

	xCoord = 0.0;
	yCoord = 0.0;

	for ( int k=0; k < row + 1 ; k++) 
	{
		currentWidth = -minValHalf;
		xCoord = 0.0f;
		for ( int j=0; j < col + 1; j++)
		{
			
			backgroundVertices->push_back( osg::Vec3( currentWidth, currentHeight, -dist) );
			
			//xCoord = (currentWidth + minValHalf) / (float)width;
			//yCoord = (currentHeight + minValHalf)/ (float)height;
			(*texcoords)[j + k * (col + 1)].set(xCoord,yCoord);

			currentWidth = currentWidth + xUnit;
			xCoord = xCoord + sUnit;

		
		}
		currentHeight = currentHeight + yUnit;
		yCoord = yCoord + tUnit;
	}

	for ( int k=0; k < row ; k++)
	{	
		for ( int j=0; j < col ; j++)
		{
			backgroundIndices->push_back(j + k *(col+1) );
			backgroundIndices->push_back((j + 1) + k *(col+1) );
			backgroundIndices->push_back((j + 1) + (k + 1) *(col+1) );
			backgroundIndices->push_back( j  + (k + 1) *(col+1) );	
			
		}		
	}
 
	osg::Vec4Array* colors = new osg::Vec4Array; 
	colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));


	osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));

	backgroundGeometry->addPrimitiveSet(backgroundIndices);
    backgroundGeometry->setTexCoordArray(0,texcoords);
	backgroundGeometry->setNormalArray(normals);
	backgroundGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	backgroundGeometry->setVertexArray(backgroundVertices);
	backgroundGeometry->setColorArray(colors);
	backgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	geode->addDrawable(backgroundGeometry);
	return geode;
}