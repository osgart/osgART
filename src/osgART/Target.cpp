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

#include <osg/Notify>

#include "osgART/Target"
#include "osgART/TransformFilterCallback"

namespace osgART {


	Target::Target()
		: osg::Object()
		, _valid(false)
		, _active(false)
	{
	}

	Target::Target(const Target& target, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/)
		: osg::Object(target,copyop)
	{
		if (this != &target)
		{
			_active = target.active();
			_valid = target.valid();
		}
	}

	Target::~Target()
	{
	}

	/*virtual*/
	Target::TargetType
	Target::getType() const
	{
		return ART_UNKNOWN;
	}

	const
	osg::Matrix& Target::getTransform() const
	{
		return _transform;
	}

	/*virtual */
	bool
	Target::valid() const
	{
		return _valid;
	}

	/*virtual*/
	bool
	Target::active() const
	{
		return _active;
	}

	void
	Target::updateTransform(const osg::Matrix& transform)
	{
		_transform = transform;
	}


	/*virtual */
	void
	Target::setActive(bool active /*= true*/)
	{
		_active = active;
	}

}
