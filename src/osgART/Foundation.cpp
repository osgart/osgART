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


#include <osg/Version>
#include <osg/Notify>

#include <osgDB/Registry>
#include <osgDB/FileUtils>

#include "osgART/Foundation"
#include "osgART/PluginManager"

namespace osgART {

	class Initializer
	{
	public:

		Initializer()
		{

			osg::notify() << "osgART::Initializer()" << std::endl;

		}

		~Initializer()
		{
			// Delete the Plugin Manager
			osgART::PluginManager::instance(true);
		}

	};

}

/* global initializer */
static osgART::Initializer g_Initializer;
osgART::Initializer* p_gInitializer = &g_Initializer;


