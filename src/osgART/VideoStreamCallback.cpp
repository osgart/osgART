/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osg/Version>
#include <osg/NodeVisitor>

#include "osgART/VideoStreamCallback"

namespace osgART {


VideoStreamCallback::VideoStreamCallback(osg::ImageStream* imagestream)
	: _imagestream(imagestream)
	, _framenumber(-1)
{
}

void VideoStreamCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) 
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

