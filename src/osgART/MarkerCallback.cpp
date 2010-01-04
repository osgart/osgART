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


#include "osgART/MarkerCallback"

#include <osg/Switch>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

namespace osgART {


	void addEventCallback(osg::Node* node, osg::NodeCallback* cb)
	{
		/* paranoia check */
		if (!node) return;

		/* add initial callback */
		if (!node->getEventCallback())
		{		
			node->setEventCallback(cb);

		} else if (cb)
		{
			node->getEventCallback()->addNestedCallback(cb);
		}
	}
	

	
	void removeEventCallback(osg::Node* node, osg::NodeCallback* cb) {

       if (!node || !cb) return;    // Sanity check

       osg::NodeCallback* n = node->getEventCallback();
       if (!n) return;                // There is no callback list

       // Check the first callback
       if (n == cb) {
           // The first callback matches, so remove it, and reattach its child (which might be NULL)
           node->setEventCallback(n->getNestedCallback());
           return;
       }

       // Check nested callbacks
       while (n) {

           osg::NodeCallback* nested = n->getNestedCallback();                      
		   if (!nested) return;        // Run out of children

           if (nested == cb) {
               // The callback matches, so remove it, and reattach its child (which might be NULL)
               n->setNestedCallback(nested->getNestedCallback());
               return;
           }

           n = nested; // Move to next callback

       }

   } 


	void attachDefaultEventCallbacks(osg::Node* node, Marker* marker)
	{
		if (!node) {
			osg::notify() << "attachDefaultEventCallbacks: Can't attach callbacks to NULL node" << std::endl;
			return;
		}
		
		if (!marker) {
			osg::notify() << "attachDefaultEventCallbacks: Can't attach callbacks with NULL marker" << std::endl;
			return;
		}

		addEventCallback(node, new MarkerTransformCallback(marker));
		addEventCallback(node, new MarkerVisibilityCallback(marker));
	}


	SingleMarkerCallback::SingleMarkerCallback(Marker* marker) : 
		osg::NodeCallback(),
		m_marker(marker)
	{
	}

	DoubleMarkerCallback::DoubleMarkerCallback(Marker* markerA, Marker* markerB) : 
		osg::NodeCallback(),
		m_markerA(markerA),
		m_markerB(markerB)
	{
	}


	MarkerTransformCallback::MarkerTransformCallback(Marker* marker) : 
		SingleMarkerCallback(marker),
		mEnabled(true)
	{
	}

	void MarkerTransformCallback::setEnabled(bool e) {
		mEnabled = e;	
	}

	bool MarkerTransformCallback::getEnabled() {
		return mEnabled;
	}

	/*virtual*/ 
	void MarkerTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		if (mEnabled) {

			// Handler for osg::MatrixTransforms
			if (osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node)) {
				mt->setMatrix(m_marker->getTransform());
			}
			
			// Handler for osg::PositionAttitudeTransforms
			// TODO: check correct translation/rotation order
			else if (osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(node)) {
				pat->setPosition(m_marker->getTransform().getTrans());
				pat->setAttitude(m_marker->getTransform().getRotate());
				pat->setScale(osg::Vec3(1.0f, 1.0f, 1.0f));
			}

			// TODO: Handle other types of nodes... ?


		}

		// Traverse the Node's subgraph
		traverse(node,nv);

	}

	MarkerVisibilityCallback::MarkerVisibilityCallback(Marker* marker) : 
		SingleMarkerCallback(marker),
		m_visibilityMode(VISIBILITY_NORMAL),
		m_millisecondsToKeepVisible(0.0f)
	{

	}

	/*virtual*/ 
	void MarkerVisibilityCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		bool enableNode = false;

		switch (m_visibilityMode) {

			case VISIBILITY_NORMAL:
				enableNode = m_marker->valid();
				break;
			
			case VISIBILITY_TIMEOUT:

				if (m_marker->valid()) {
					m_timer.setStartTick();
					enableNode = true;
				} else {
					enableNode = (m_timer.time_m() < m_millisecondsToKeepVisible);
				}

				break;

			case VISIBILITY_ALWAYS:
				enableNode = true;
				break;
			case VISIBILITY_NEVER:
				enableNode = false;
				break;

		}

		if (osg::Switch* _switch = dynamic_cast<osg::Switch*>(node)) 
		{
			// Handle visibilty for switch nodes

			// _switch->setSingleChildOn(m_marker->valid() ? 0 : 1);	
			if (enableNode) _switch->setAllChildrenOn();
			else _switch->setAllChildrenOff();
		} 

		/*
		else if (  ) {

			// Potentially handle visibility on other types of node...

		}
		*/
		else 
		{

			// This method will work for any node.

			// Make sure the visitor will return to invisible nodes
			// If we make this node invisible (because the marker is hidden) then
			// we need to be able to return and update it later, or it will remain
			// hidden forever. 
			nv->setNodeMaskOverride(0xFFFFFFFF);

			node->setNodeMask(enableNode ? 0xFFFFFFFF : 0x0);
		}

		// must traverse the Node's subgraph            
		traverse(node,nv);

	}

	void MarkerVisibilityCallback::setVisibilityMode(MarkerVisibilityCallback::VisibilityMode mode) {
		m_visibilityMode = mode;
	}
		
	MarkerVisibilityCallback::VisibilityMode MarkerVisibilityCallback::getVisibilityMode() {
		return m_visibilityMode;
	}


	void MarkerVisibilityCallback::setMillisecondsToKeepVisible(double ms) {
		m_millisecondsToKeepVisible = ms;
	}

	double MarkerVisibilityCallback::getMillisecondsToKeepVisible() {
		return m_millisecondsToKeepVisible;
	}



	MarkerDebugCallback::MarkerDebugCallback(Marker* marker) : 
		SingleMarkerCallback(marker)
	{
	}

	/*virtual*/ 
	void MarkerDebugCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		if (m_marker->valid()) {

			// Debug information when marker is visible
			std::cout << 
				"Marker: " << m_marker->getName() << std::endl <<
				"Type: " << typeid(*m_marker).name() << std::endl << 
				"Confidence: " << m_marker->getConfidence() << std::endl << 
				"Transform: " << std::endl << m_marker->getTransform() << std::endl;

		}


		// must traverse the Node's subgraph            
		traverse(node,nv);
	}

	LocalTransformationCallback::LocalTransformationCallback(Marker* base, Marker* paddle) :
		DoubleMarkerCallback(base, paddle) 
	{
	}


	/*virtual*/ 
	void LocalTransformationCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
    
		nv->setNodeMaskOverride(0xFFFFFFFF);

		osg::Matrix baseMatrix, paddleMatrix;

		bool bothValid = m_markerA->valid() && m_markerB->valid();
		node->setNodeMask(bothValid ? 0xFFFFFFFF : 0x0);

		if (bothValid) {
			
			baseMatrix = m_markerA->getTransform();
			paddleMatrix = m_markerB->getTransform();
			baseMatrix.invert(baseMatrix);

			if (osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node)) {
				mt->setMatrix(paddleMatrix * baseMatrix);
			} 

		}

		// must traverse the Node's subgraph            
		traverse(node,nv);
	}



};
