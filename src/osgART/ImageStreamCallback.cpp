/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
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

#include <osg/Version>
#include <osg/NodeVisitor>

#include "osgART/ImageStreamCallback"

namespace osgART {


ImageStreamCallback::ImageStreamCallback(osg::ImageStream* imagestream)
	: _imagestream(imagestream)
	, _framenumber(-1)
{
}

void ImageStreamCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) 
{
	
	if (_imagestream) 
	{

		// hse25: here the imagestream could be started ...
		// but we need a little change in the API to 
		// make sure the stream is ready and not invalid or just stopped

		//if (_imagestream->getStatus() != osg::ImageStream::PLAYING) 
		//	_imagestream->play();

		if (nv->getFrameStamp()->getFrameNumber() != _framenumber) 
		{

			// a change after 2.5.6
#if (OPENSCENEGRAPH_SOVERSION > 41)
			_imagestream->update(nv);
#else
			#error Unsupported version of OpenSceneGraph
#endif
			
			_framenumber = nv->getFrameStamp()->getFrameNumber();
		}
	}

	// must traverse the Node's subgraph            
	traverse(node,nv);
}

}

