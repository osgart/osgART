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
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

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
