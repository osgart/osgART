/*
 *	osgART/ARTTransformCallback
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

		if (mt) 
		{

			if (m_marker->isValid()) 
			{
				// copy matrix
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
