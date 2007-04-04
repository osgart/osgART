#include "NodeBoundingAreaBillboard.h"

NodeBoundingAreaBillboard::NodeBoundingAreaBillboard()
{
}

NodeBoundingAreaBillboard::~NodeBoundingAreaBillboard()
{
	int size = (int)(updateHandlers.size());
	for ( int k =0 ; k < size ; k++)
	{
		delete updateHandlers.at(k);
	}
	updateHandlers.clear();
}


void NodeBoundingAreaBillboard::setBillboardPosition( int id, osg::Vec3f pos)
{
	this->setPosition(id , pos);
}

void NodeBoundingAreaBillboard::init(osg::ref_ptr<osg::Texture2D> texture)
{

	this->setMode(osg::Billboard::AXIAL_ROT);
	this->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
	this->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));

	//osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
	//alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);

	billBoardStateSet = new osg::StateSet;

	billBoardStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	billBoardStateSet->setTextureAttributeAndModes
		(0, texture.get(), osg::StateAttribute::ON );
	//billBoardStateSet->setAttributeAndModes
	//	(new osg::BlendFunc, osg::StateAttribute::ON );
	
	//billBoardStateSet->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );

	billBoardStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
	billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);


	billBoardStateSet->setRenderBinDetails(100 , "RenderBin");
	
	

	this->setEventCallback( new BillboardUpdateCallback() );
	


	//layerGroup->getOrCreateStateSet()->setAttribute(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));

	//m_layerGeode = new osg::Geode();
	//layerGroup->addChild(m_layerGeode.get());
	//
	//m_layerGeode->addDrawable(createDrawable());
	
		

}

void NodeBoundingAreaBillboard::addBillboardFor( osg::ref_ptr<osg::Node> node , float scale )
{
	osg::BoundingSphere bound = node->getBound();
	osg::Drawable *drawable = createDrawable(bound.radius()* 2 * scale);

	this->addDrawable( drawable , bound.center() );	

	BillboardUpdateHandler *bupdateHandler = new BillboardUpdateHandler();
	this->scale = scale;
	bupdateHandler->init(this);
	bupdateHandler->setNode( node );

    updateHandlers.push_back( bupdateHandler );
}

void NodeBoundingAreaBillboard::updateBillboard()
{
	for (int k=0; k < (int)(updateHandlers.size()); k++)
	{
		updateHandlers.at(k)->update(k);
	}
	
}

osg::Drawable* NodeBoundingAreaBillboard::createDrawable(float size)
{
   float width = size;;
   float height = size;

   osg::Geometry* shrubQuad = new osg::Geometry;

   osg::Vec3Array* shrubVerts = new osg::Vec3Array(4);
   (*shrubVerts)[0] = osg::Vec3(-width/2.0f, 0, 0);
   (*shrubVerts)[1] = osg::Vec3( width/2.0f, 0, 0);
   (*shrubVerts)[2] = osg::Vec3( width/2.0f, 0, height);
   (*shrubVerts)[3] = osg::Vec3(-width/2.0f, 0, height);

   shrubQuad->setVertexArray(shrubVerts);

   osg::Vec2Array* shrubTexCoords = new osg::Vec2Array(4);
   (*shrubTexCoords)[0].set(0.0f,0.0f);
   (*shrubTexCoords)[1].set(1.0f,0.0f);
   (*shrubTexCoords)[2].set(1.0f,1.0f);
   (*shrubTexCoords)[3].set(0.0f,1.0f);
   shrubQuad->setTexCoordArray(0,shrubTexCoords);

   shrubQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

   // Need to assign a color to the underlying geometry, otherwise we'll get
   // whatever color is current applied to our geometry.
   // Create a color array, add a single color to use for all the vertices

   osg::Vec4Array* colorArray = new osg::Vec4Array;
   colorArray->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); // white, fully opaque

   // An index array for assigning vertices to colors (based on index in the array)
   osg::TemplateIndexArray
      <unsigned int, osg::Array::UIntArrayType,4,1> *colorIndexArray;
   colorIndexArray = 
      new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,1>;
   colorIndexArray->push_back(0);

   // Use the index array to associate the first entry in our index array with all 
   // of the vertices.
   shrubQuad->setColorArray(colorArray);
   shrubQuad->setColorIndices(colorIndexArray);
   shrubQuad->setColorBinding(osg::Geometry::BIND_OVERALL);

   shrubQuad->setStateSet(billBoardStateSet); 

   return shrubQuad;
}

///////////////////////////////////////////////////////////////////////////////////////////


BillboardUpdateCallback::BillboardUpdateCallback()
{
}

BillboardUpdateCallback::~BillboardUpdateCallback()
{
}

void BillboardUpdateCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
	NodeBoundingAreaBillboard *nbab = dynamic_cast<NodeBoundingAreaBillboard*>( nd ); 
	
	nbab->updateBillboard();
}