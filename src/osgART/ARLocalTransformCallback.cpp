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

#include "osgART/ARLocalTransformCallback"

#include <osg/Notify>

namespace osgART {


	ARLocalTransformCallback::ARLocalTransformCallback(Marker* baseMarker, Marker* paddleMarker) : osg::NodeCallback(),
		mBaseMarker(baseMarker), mPaddleMarker(paddleMarker) {		
    };

    /* virtual */
	void 
	ARLocalTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		nv->setNodeMaskOverride(0xFFFFFFFF);

        osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

		if (mt && mBaseMarker.valid() && mPaddleMarker.valid()) {
            
			osg::Matrix baseMatrix, paddleMatrix;

			if (mBaseMarker->valid() && mPaddleMarker->valid()) {
			
				baseMatrix = mBaseMarker->getTransform();
				paddleMatrix = mPaddleMarker->getTransform();

				baseMatrix.invert(baseMatrix);

				mt->setMatrix(paddleMatrix * baseMatrix);

				// Visible
				mt->setNodeMask(0xFFFFFFFF);

			} else {

				// Hidden
				mt->setNodeMask(0x0);
			
			}

		} else {

			osg::notify(osg::WARN) << "osgART::ARLocalTransformCallback(): " <<
				"Invalid references to markers" << std::endl;

		}    
		// must traverse the Node's subgraph            
		traverse(node,nv);

    }
};
