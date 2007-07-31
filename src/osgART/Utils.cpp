/*
 *	osgART/Foundation
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

#include "osgART/Utils"

namespace osgART{

	std::string trim(std::string& s,const std::string& drop/* = " "*/)
	{
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}

	std::vector<std::string> tokenize(const std::string& str, const std::string& delimiters)
	{
		std::vector<std::string> tokens;
	    	
		// skip delimiters at beginning.
		std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	    	
		// find first "non-delimiter".
		std::string::size_type pos = str.find_first_of(delimiters, lastPos);

		while (std::string::npos != pos || std::string::npos != lastPos)
    	{
        	// found a token, add it to the vector.
        	tokens.push_back(str.substr(lastPos, pos - lastPos));
		
        	// skip delimiters.  Note the "not_of"
        	lastPos = str.find_first_not_of(delimiters, pos);
		
        	// find next "non-delimiter"
        	pos = str.find_first_of(delimiters, lastPos);
    	}

		return tokens;
	}
	

	osg::Matrix GenerateMatrix(const osg::Vec3d & _trans, const osg::Vec3d & _rot)
	{
		osg::Matrix tempmatt;
		tempmatt = osg::Matrixd::rotate(
			_rot[0], osg::Vec3f(1,0,0),
			_rot[1], osg::Vec3f(0,1,0),
			_rot[2], osg::Vec3f(0,0,1));

		tempmatt *= osg::Matrixd::translate(_trans);
		return tempmatt;
	}

	osg::Matrix DiffMatrix(const osg::Matrix & _base, const osg::Matrix & _m2)
	{
		osg::Matrix Base;
		Base.invert(_base);
		return _m2 * Base;
	}

};//namespace osgART