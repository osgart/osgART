#include "osgART/VideoUtils"

#include "osgART/VideoLayer"
#include "osgART/VideoGeode"

namespace osgART {

	osg::Group*
	createImageBackground(osg::Image* video,
						  bool useTextureRectangle /*= false*/)
	{
		osgART::VideoLayer* _layer = new osgART::VideoLayer();
		osgART::VideoGeode* _geode = new osgART::VideoGeode(video, NULL, 1, 1, 20, 20,
			useTextureRectangle ? osgART::VideoGeode::USE_TEXTURE_RECTANGLE : osgART::VideoGeode::USE_TEXTURE_2D);
		_layer->addChild(_geode);
		return _layer;
	}

}
