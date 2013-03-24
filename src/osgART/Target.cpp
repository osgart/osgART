/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
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
