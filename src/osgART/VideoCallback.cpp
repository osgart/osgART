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

// std include

// OpenThreads include

// OSG include
#include <osg/Switch>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

// local include
#include "osgART/VideoCallback"



namespace osgART {

	VideoUpdateCallback::VideoUpdateCallback(Video* video): _video(video), _framenumber(-1) {

	}

	/* static */
	VideoUpdateCallback*
		VideoUpdateCallback::addOrSet(osg::Node* node, osgART::Video* video)
	{
		VideoUpdateCallback *callback = new VideoUpdateCallback(video);

		node->getEventCallback() ? node->getEventCallback()->addNestedCallback(callback)
			: node->setEventCallback(callback);

		return callback;

	}

	void VideoUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (_video.valid())
		{
			if (nv->getFrameStamp()->getFrameNumber() != _framenumber)
			{
				_video->update(nv);

				_framenumber = nv->getFrameStamp()->getFrameNumber();
			}
		}

		// must traverse the Node's subgraph
		traverse(node,nv);
	}

    VideoStartCallback::VideoStartCallback(osgART::Video* Video) : _video(Video), _oneshot(false) {}


    /* static */
    VideoStartCallback*
        VideoStartCallback::addOrSet(osg::Node* node, osgART::Video* video)
    {
        VideoStartCallback *callback = new VideoStartCallback(video);

        node->getEventCallback() ? node->getEventCallback()->addNestedCallback(callback)
            : node->setEventCallback(callback);

        return callback;

    }

    void VideoStartCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
    {

        if (!_oneshot)
        {
            _video->start();
            _oneshot=true;
        }

        traverse(node,nv);
    }


    VideoStartAndUpdateCallback::VideoStartAndUpdateCallback(osgART::Video* Video) : _video(Video), _oneshot(false), _framenumber(-1)  {}

    /* static */
    VideoStartAndUpdateCallback*
        VideoStartAndUpdateCallback::addOrSet(osg::Node* node, osgART::Video* video)
    {
        VideoStartAndUpdateCallback *callback = new VideoStartAndUpdateCallback(video);

        node->getEventCallback() ? node->getEventCallback()->addNestedCallback(callback)
            : node->setEventCallback(callback);

        return callback;

    }

    void VideoStartAndUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
    {

        if (!_oneshot)
        {
            _video->start();
            _oneshot=true;
        }
        else
        {
            if (_video.valid())
            {
                if (nv->getFrameStamp()->getFrameNumber() != _framenumber)
                {
                    _video->update(nv);

                    _framenumber = nv->getFrameStamp()->getFrameNumber();
                }
            }
        }

        traverse(node,nv);
    }
};
