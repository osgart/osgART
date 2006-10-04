#include "osgART/ARLocalTransformCallback"


#include "osgART/TrackerManager"


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

		}
    
		// must traverse the Node's subgraph            
		traverse(node,nv);

    }
};