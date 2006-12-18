#ifndef GAZEINTERACTIONTARGET_H
#define GAZEINTERACTIONTARGET_H

#include <osg/ShapeDrawable>
#include <string>



class GazeInteractionTarget {

public:
		
	enum GITargetType 
		{
			GI_UNKNOWN = 0,	// unknown type
			GI_SPHERE				
		};

	GazeInteractionTarget();

	/**
		* \brief destructor.
		*/
	~GazeInteractionTarget();

	bool			setName(char* name);
	bool			setID(int id);
	bool			setPosX(double posX);
	bool			setPosY(double posY);
	bool			setPosZ(double posZ);
	bool			setPosition(osg::Vec3d* position);
	bool			setosgGeode(osg::Geode* geode);
	
	int						getID();
	std::string				getName();
	virtual GITargetType	getType() = 0;
	double					getPosX();
	double					getPosY();
	double					getPosZ();
	osg::Vec3d*				getPosition();
	osg::Geode*				getosgGeode();


protected:

	int m_ID;
	std::string	m_Name;
	GITargetType m_Type;
	double		m_posX;
	double		m_posY;
	double		m_posZ;
	osg::Vec3d*	m_position;
	osg::Geode* m_geode;


private:
	
};
#endif