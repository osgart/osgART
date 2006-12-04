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
	bool			setPosX(int posX);
	bool			setPosY(int posY);
	bool			setPosZ(int posZ);
	bool			setosgGeode(osg::Geode* geode);
	
	std::string				getName();
	virtual GITargetType	getType() = 0;
	int						getPosX();
	int						getPosY();
	int						getPosZ();
	osg::Geode*		getosgGeode();


protected:

	std::string	m_Name;
	GITargetType m_Type;
	int		m_posX;
	int		m_posY;
	int		m_posZ;
	osg::Geode* m_geode;


private:
	
};