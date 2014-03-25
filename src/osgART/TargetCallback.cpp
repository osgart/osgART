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
#include "osgART/Export"
#include "osgART/TargetCallback"



namespace osgART {

	void attachDefaultTargetCallbacks(osg::Node* node, Target* target)
	{
		if (!node) {
			osg::notify() << "attachDefaultTargetCallbacks: Can't attach callbacks to NULL node" << std::endl;
			return;
		}

		if (!target) {
			osg::notify() << "attachDefaultTargetCallbacks: Can't attach callbacks with NULL target" << std::endl;
			return;
		}

		addEventCallback(node, new TargetTransformCallback(target));
		addEventCallback(node, new TargetVisibilityCallback(target));
	}


	SingleTargetCallback::SingleTargetCallback(Target* target) :
		osg::NodeCallback(),
		m_target(target)
	{
	}

	DoubleTargetCallback::DoubleTargetCallback(Target* targetA, Target* targetB) :
		osg::NodeCallback(),
		m_targetA(targetA),
		m_targetB(targetB)
	{
	}


	TargetTransformCallback::TargetTransformCallback(Target *target) :
		SingleTargetCallback(target),
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
				mt->setMatrix(m_target->getMatrix());
			}

			// Handler for osg::PositionAttitudeTransforms
			// TODO: check correct translation/rotation order
			else if (osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(node)) {
				pat->setPosition(m_target->getMatrix().getTrans());
				pat->setAttitude(m_target->getMatrix().getRotate());
				pat->setScale(osg::Vec3(1.0f, 1.0f, 1.0f));
			}

			// TODO: Handle other types of nodes... ?


		}

		// Traverse the Node's subgraph
		traverse(node,nv);

	}

	TargetVisibilityCallback::TargetVisibilityCallback(Target *target) :
		SingleTargetCallback(target),
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
				m_isVisible = m_target->valid();
				break;

			case VISIBILITY_TIMEOUT:

				if (m_target->valid()) {
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

			// _switch->setSingleChildOn(m_target->valid() ? 0 : 1);
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
			// If we make this node invisible (because the target is hidden) then
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


	TargetDebugCallback::TargetDebugCallback(Target *target) :
		SingleTargetCallback(target)
	{
	}

	/*virtual*/
	void TargetDebugCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {

		if (m_target->valid()) {

			// Debug information when target is visible
			std::cout <<
				"Target: " << m_target->getName() << std::endl <<
				"Type: " << typeid(*m_target).name() << std::endl <<
				"Confidence: " << m_target->getConfidence() << std::endl <<
				"Transform: " << std::endl << m_target->getMatrix() << std::endl;

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

		bool bothValid = m_targetA->valid() && m_targetB->valid();
		node->setNodeMask(bothValid ? 0xFFFFFFFF : 0x0);

		if (bothValid) {

			baseMatrix = m_targetA->getMatrix();
			paddleMatrix = m_targetB->getMatrix();
			baseMatrix.invert(baseMatrix);

			if (osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node)) {
				mt->setMatrix(paddleMatrix * baseMatrix);
			}

		}

		// must traverse the Node's subgraph
		traverse(node,nv);
	}



};
