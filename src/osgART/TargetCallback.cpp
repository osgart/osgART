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


#include "osgART/TargetCallback"

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


	void attachDefaultEventCallbacks(osg::Node* node, Target* target)
	{
		if (!node) {
			osg::notify() << "attachDefaultEventCallbacks: Can't attach callbacks to NULL node" << std::endl;
			return;
		}

		if (!target) {
			osg::notify() << "attachDefaultEventCallbacks: Can't attach callbacks with NULL marker" << std::endl;
			return;
		}

		addEventCallback(node, new TargetTransformCallback(target));
		addEventCallback(node, new TargetVisibilityCallback(target));
	}


	SingleTargetCallback::SingleTargetCallback(Target* marker) :
		osg::NodeCallback(),
		m_marker(marker)
	{
	}

	DoubleTargetCallback::DoubleTargetCallback(Target* markerA, Target* markerB) :
		osg::NodeCallback(),
		m_markerA(markerA),
		m_markerB(markerB)
	{
	}


	TargetTransformCallback::TargetTransformCallback(Target *marker) :
		SingleTargetCallback(marker),
		mEnabled(true)
	{
	}

	void TargetTransformCallback::setEnabled(bool e) {
		mEnabled = e;
	}

	bool TargetTransformCallback::getEnabled() {
		return mEnabled;
	}

	/*virtual*/
	void TargetTransformCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

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

	TargetVisibilityCallback::TargetVisibilityCallback(Target *marker) :
		SingleTargetCallback(marker),
		m_visibilityMode(VISIBILITY_NORMAL),
		m_millisecondsToKeepVisible(0.0f),
		m_isVisible(false)
	{

	}

	bool TargetVisibilityCallback::isVisible() {
		return m_isVisible;
	}

	/*virtual*/
	void TargetVisibilityCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		m_isVisible = false;

		switch (m_visibilityMode) {

			case VISIBILITY_NORMAL:
				m_isVisible = m_marker->valid();
				break;

			case VISIBILITY_TIMEOUT:

				if (m_marker->valid()) {
					m_timer.setStartTick();
					m_isVisible = true;
				} else {
					m_isVisible = (m_timer.time_m() < m_millisecondsToKeepVisible);
				}

				break;

			case VISIBILITY_ALWAYS:
				m_isVisible = true;
				break;
			case VISIBILITY_NEVER:
				m_isVisible = false;
				break;

		}

		if (osg::Switch* _switch = dynamic_cast<osg::Switch*>(node))
		{
			// Handle visibilty for switch nodes

			// _switch->setSingleChildOn(m_marker->valid() ? 0 : 1);
			if (m_isVisible) _switch->setAllChildrenOn();
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

			node->setNodeMask(m_isVisible ? 0xFFFFFFFF : 0x0);
		}

		// must traverse the Node's subgraph
		traverse(node,nv);

	}

	void TargetVisibilityCallback::setVisibilityMode(TargetVisibilityCallback::VisibilityMode mode) {
		m_visibilityMode = mode;
	}

	TargetVisibilityCallback::VisibilityMode TargetVisibilityCallback::getVisibilityMode() {
		return m_visibilityMode;
	}


	void TargetVisibilityCallback::setMillisecondsToKeepVisible(double ms) {
		m_millisecondsToKeepVisible = ms;
	}

	double TargetVisibilityCallback::getMillisecondsToKeepVisible() {
		return m_millisecondsToKeepVisible;
	}


	MatrixOffsetCallback::MatrixOffsetCallback(osg::Matrix offset) : osg::NodeCallback(),
		_offset(offset)
	{


	}

	void MatrixOffsetCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{

		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);

		if (mt)
		{
			osg::Matrix m = mt->getMatrix();
			mt->setMatrix(m * _offset);

			std::cout << "--------------------------------------------" << std::endl;

			std::cout << m << std::endl;

			std::cout << _offset << std::endl;

			std::cout << mt->getMatrix() << std::endl;


		}

		traverse(node,nv);
	}

	void MatrixOffsetCallback::setupReflection(bool flipX, bool flipY)
	{
		_offset = osg::Matrix::scale(flipX ? -1 : 1, flipY ? -1 : 1, 1);
	}


	TargetDebugCallback::TargetDebugCallback(Target *marker) :
		SingleTargetCallback(marker)
	{
	}

	/*virtual*/
	void TargetDebugCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		if (m_marker->valid()) {

			// Debug information when marker is visible
			std::cout <<
				"Target: " << m_marker->getName() << std::endl <<
				"Type: " << typeid(*m_marker).name() << std::endl <<
				"Confidence: " << m_marker->getConfidence() << std::endl <<
				"Transform: " << std::endl << m_marker->getTransform() << std::endl;

		}


		// must traverse the Node's subgraph
		traverse(node,nv);
	}

	LocalTransformationCallback::LocalTransformationCallback(Target *base, Target *paddle) :
		DoubleTargetCallback(base, paddle)
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
