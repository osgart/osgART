#include "osgART/Utils"

namespace osgART{

std::string trim(std::string& s,const std::string& drop/* = " "*/)
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}
	
void PrintMatrix(std::string name, const osg::Matrix &Transform)
{
		// Print it out
		std::cout << "Matrix : " << name << std::endl;
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++) 
			{
				std::cout << std::setw(10) << std::setprecision(5) << Transform(i, j) << "  ";
			}
			std::cout  << std::endl;
		}
		osg::notify() << " length : " << Transform.getTrans().length() << std::endl;
		osg::notify() << std::endl;
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
		//osg::Matrix tempmatt;
		osg::Matrix Base;
		Base.invert(_base);
		return _m2 * Base;
	}

	void PrintVector3D	(std::string name, const osg::Vec3d &Vec3d)
	{
		// Print it out
		osg::notify() << "Vector3d : " << name <<  "=> ";
		for (int j = 0; j < 3; j++) 
			osg::notify() << std::setw(10) << std::setprecision(5) << Vec3d._v[j] << "  ";
			
		osg::notify() << std::endl;
	}
};//namespace osgART