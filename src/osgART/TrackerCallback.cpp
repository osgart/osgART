/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2014 Raphael Grasset, Julian Looser, Hartmut Seichter
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
#include "osgART/TrackerCallback"



namespace osgART {


	TrackerUpdateCallback::TrackerUpdateCallback(Tracker* tracker): _tracker(tracker), _framenumber(-1) {

	}

	/* static */
	TrackerUpdateCallback*
		TrackerUpdateCallback::addOrSet(osg::Node* node, osgART::Tracker* tracker)
	{
		TrackerUpdateCallback *callback = new TrackerUpdateCallback(tracker);

		node->getEventCallback() ? node->getEventCallback()->addNestedCallback(callback)
			: node->setEventCallback(callback);

		return callback;

	}

	void TrackerUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (_tracker.valid())
		{
			if (nv->getFrameStamp()->getFrameNumber() != _framenumber)
			{
				_tracker->update(nv);

				_framenumber = nv->getFrameStamp()->getFrameNumber();
			}
		}

		// must traverse the Node's subgraph
		traverse(node,nv);
	}

};
