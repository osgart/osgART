#include "osgART/TransformFilterCallback"

namespace osgART {

	TransformFilterCallback::TransformFilterCallback()
	{			           
	}

	void TransformFilterCallback::operator()(Marker* marker, const osg::Matrix& nmatrix)
	{
		osg::Matrix _matrix = marker->getTransform();
	}
};