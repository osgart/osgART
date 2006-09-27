#include "osgART/ARTTransformCallback"


namespace osgART {


	ARTTransformCallback::ARTTransformCallback(Marker* marker) : 
		osg::NodeCallback(),
		m_marker(marker)
	{
	}
	
	/*virtual*/ 
	void ARTTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		// Make sure the visitor will return to invisible nodes
		// If we make this node invisible (because the marker is hidden) then
		// we need to be able to return and update it later, or it will remain
		// hidden forever. 
		nv->setNodeMaskOverride(0xFFFFFFFF);

		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

		if (mt) {

			if (m_marker->isValid()) 
			{
				mt->setMatrix(m_marker->getTransform());
				// Visible
				mt->setNodeMask(0xFFFFFFFF);
			} 
			else 
			{
				// Hidden
				mt->setNodeMask(0x0);
			}

		}
	
		// must traverse the Node's subgraph            
		traverse(node,nv);

	}

};