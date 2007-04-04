#include "GeometryMotionHandler.h"

GeometryMotionHandler::GeometryMotionHandler()
{
	firstUpdate = true;
}

GeometryMotionHandler::~GeometryMotionHandler()
{
	
	
	
}

void GeometryMotionHandler::init(osg::Geometry *_geom, int _attriIndex)
{
	geom = _geom;
	attriIndex = _attriIndex;

	currentID = 0;
	prevID = 1;

	points[0] = new osg::Vec3Array();
	points[1] = new osg::Vec3Array();
	points[2] = new osg::Vec3Array();
	

	origialVertex = dynamic_cast<osg::Vec3Array*>( geom->getVertexArray() ); 

	int size = origialVertex->size();

	for ( int k=0; k < size ; k++)
	{
		points[0]->push_back( origialVertex->at(k) );
		points[1]->push_back( origialVertex->at(k) );
		points[2]->push_back( osg::Vec3() );		
	}
}
	
void GeometryMotionHandler::update(osg::Matrix m)
{
	int size = origialVertex->size();

	osg::Node *parent = geom->getParent(0);
	osg::MatrixList mList = parent->getWorldMatrices();
	osg::Matrix mm = osg::Matrixf::identity();
	int mListSize = mList.size();
	
	for ( int k=0; k < mListSize; k++)
	{
		mm = mm * mList.at(k);
	}

	for ( int k=0; k < size ; k++)
	{
		//points[currentID]->at(k) = origialVertex->at(k) * m;
		points[currentID]->at(k) = m.preMult(origialVertex->at(k));
		//points[currentID]->at(k) = m.postMult(origialVertex->at(k));
	}

	if ( firstUpdate )
	{
		for ( int k=0; k < size ; k++)
		{
			//points[prevID]->at(k) = origialVertex->at(k) * m;
			points[prevID]->at(k) = m.preMult(origialVertex->at(k));
			//points[prevID]->at(k) = m.postMult(origialVertex->at(k));
		}
		firstUpdate = false;
	}

	for ( int k=0; k < size ; k++)
	{
		points[2]->at(k) = points[currentID]->at(k) - points[prevID]->at(k);
	}

	geom->setVertexAttribData( attriIndex, osg::Geometry::ArrayData(points[2], osg::Geometry::BIND_PER_VERTEX, GL_FALSE) );

	//std::cout << points[2]->at(0)[0] <<  " " << points[2]->at(0)[1] << std::endl;
	// ping... pong...
	int tempID = currentID;
	currentID = prevID;
	prevID = tempID;
	
}
