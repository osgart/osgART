/*
 *	osgART/ARLocalTransformCallback
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include "osgART/ARLocalTransformCallback"
#include "osgART/TrackerManager"

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

			if (mBaseMarker->isValid() && mPaddleMarker->isValid()) {
			
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
