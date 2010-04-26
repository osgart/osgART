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


	Marker::Marker() : osg::Referenced(), 
		m_valid(false),
		m_active(false),
		m_name("Marker")		
	{
		// hse25: No fields within osgART
		//m_fields["name"] = new TypedField<std::string>(&m_name);
		//m_fields["active"] = new TypedField<bool>(&m_active);
	}

	Marker::Marker( const Marker& container, const osg::CopyOp& copyop /*= osg::CopyOp::SHALLOW_COPY*/ ) :
		m_valid(container.m_valid),
		m_active(container.m_active),
		m_name(container.m_name)
	{
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
		return m_transform;
	}

	/*virtual */
	bool
	Marker::valid() const
	{
		return m_valid;
	}

	/*virtual*/ 
	bool
	Marker::active() const 
	{
		return m_active;
	}

	void
	Marker::setName(const std::string& name) 
	{
		m_name = name;
	}

	const std::string&
	Marker::getName() const 
	{
		return m_name;
	}

	void 
	Marker::updateTransform(const osg::Matrix& transform) 
	{
		m_transform = transform;
	}

	
	/*virtual */
	void 
	Marker::setActive(bool active /*= true*/)
	{
		this->m_active = active;
	}

	Marker& Marker::operator=( const Marker & )
	{
		return *this;
	}

};
