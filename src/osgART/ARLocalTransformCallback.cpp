#include "ARLocalTransformCallback"


#include "osgART/TrackerManager"


namespace osgART {


	ARLocalTransformCallback::ARLocalTransformCallback(int baseMarker, int paddleMarker) : osg::NodeCallback() {
		baseMarkerId = baseMarker;
		paddleMarkerId = paddleMarker;
    };

    /* virtual */
	void 
	ARLocalTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		nv->setNodeMaskOverride(0xFFFFFFFF);

        osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

        if (mt) {
            
			osg::Matrix baseMatrix, paddleMatrix;

			Marker* baseMarker = TrackerManager::getInstance()->getTracker(0)->getMarker(baseMarkerId);
			Marker* paddleMarker = TrackerManager::getInstance()->getTracker(0)->getMarker(paddleMarkerId);

			if (baseMarker->isValid() && paddleMarker->isValid()) {
			
				baseMatrix = baseMarker->getTransform();
				paddleMatrix = paddleMarker->getTransform();

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