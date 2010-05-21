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

#include <osg/Notify>

#include "osgART/TransformFilterCallback"
#include "osgART/Marker"

namespace osgART {


	Marker::Marker() : osg::Referenced() 
	{

		setName("Marker");
		setActive(false);
		_valid = false;

	}

	Marker::Marker( const Marker& container, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/ ) 
	{

		setName(container._name);
		setActive(container._active);
		_valid = container._valid;

	}


	Marker::~Marker() 
	{
	}

	/*virtual*/ 
	Marker::MarkerType
	Marker::getType() const
	{
		return ART_UNKNOWN;
	}

	const 
	osg::Matrix& Marker::getTransform() const 
	{
		return _transform;
	}

	/*virtual */
	bool
	Marker::valid() const
	{
		return _valid;
	}

	/*virtual*/ 
	bool
	Marker::active() const 
	{
		return _active;
	}

	void
	Marker::setName(const std::string& name) 
	{
		_name = name;
	}

	const std::string&
	Marker::getName() const 
	{
		return _name;
	}

	void 
	Marker::updateTransform(const osg::Matrix& transform) 
	{
		_transform = transform;
	}

	
	/*virtual */
	void 
	Marker::setActive(bool active /*= true*/)
	{
		_active = active;
	}

	Marker& Marker::operator=( const Marker & )
	{
		return *this;
	}

};
