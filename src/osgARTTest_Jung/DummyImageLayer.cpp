#include "DummyImageLayer.h"

DummyImageLayer::DummyImageLayer()
{
}
DummyImageLayer::~DummyImageLayer()
{
}

void DummyImageLayer::init(int w, int h, int colNum , int rowNum)
{
	width = w;
	height = h;

	row = rowNum;
	col = colNum;

	buildGeometry();
}

void DummyImageLayer::setRenderBin(int binNum)
{
	m_layerProjectionMatrix->getOrCreateStateSet()->setRenderBinDetails(binNum, "RenderBin");
}

void DummyImageLayer::buildGeometry()
{
	m_layerProjectionMatrix = new osg::Projection(osg::Matrix::ortho2D(0, width, 0, height));
	this->addChild(	m_layerProjectionMatrix.get());

	m_layerModelViewMatrix = new osg::MatrixTransform();
	m_layerModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_layerProjectionMatrix->addChild(m_layerModelViewMatrix.get());

	layerGroup = new osg::Group();
	m_layerModelViewMatrix->addChild(layerGroup.get());

	m_layerStateSet = layerGroup->getOrCreateStateSet();
	
	layerGroup->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));

	m_layerGeode = new osg::Geode();
	layerGroup->addChild(m_layerGeode.get());
	
	m_layerGeode->addDrawable(createDrawable());
	

		
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

osg::Drawable* DummyImageLayer::createDrawable()
{
	if( row == 1 && col == 1)
		return osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(width, 0, 0), osg::Vec3(0, height, 0));
  
	osg::Geometry* backgroundGeometry = new osg::Geometry();

	osg::Vec3Array* backgroundVertices = new osg::Vec3Array;
	osg::Vec2Array* texcoords = new osg::Vec2Array( (row+1) * (col+1));
			osg::DrawElementsUInt* backgroundIndices =
			new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

	float currentWidth, currentHeight;
	float xUnit = width / (float) col;
	float yUnit = height / (float) row;
	float xCoord, yCoord;

	currentWidth  = 0;
	currentHeight = 0;
	

	for ( int k=0; k < row + 1 ; k++) 
	{
		currentWidth = 0;
		for ( int j=0; j < col + 1; j++)
		{
			
			backgroundVertices->push_back( osg::Vec3( currentWidth, currentHeight,0) );
			
			xCoord = currentWidth / (float)width;
			yCoord = currentHeight/ (float)height;
			(*texcoords)[j + k * (col + 1)].set(xCoord,yCoord);

			currentWidth = currentWidth + xUnit;
		}
		currentHeight = currentHeight + yUnit;
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
	
	return backgroundGeometry;
}
void DummyImageLayer::setTexture( osg::ref_ptr<osg::Texture> _tex, int id)
{
	getOrCreateStateSet()->setTextureAttributeAndModes(id, _tex.get() ,osg::StateAttribute::ON);	
}