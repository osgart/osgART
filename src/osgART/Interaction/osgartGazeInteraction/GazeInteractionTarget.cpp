#include "GazeInteractionTarget.h"

GazeInteractionTarget::GazeInteractionTarget() 
{	
	m_position = new osg::Vec3d();
}

GazeInteractionTarget::~GazeInteractionTarget() 
{			
}

bool GazeInteractionTarget::setID(int id){
	m_ID = id;
	return true;
}	

bool GazeInteractionTarget::setName(char* name){
	m_Name = std::string( name );
	return true;
}
bool GazeInteractionTarget::setPosX(double posX){
	m_posX = posX;
	m_position->set(osg::Vec3d(m_posX, m_posY, m_posZ));
	return true;
}
bool GazeInteractionTarget::setPosY(double posY){
	m_posY = posY;
	m_position->set(osg::Vec3d(m_posX, m_posY, m_posZ));
	return true;
}
bool GazeInteractionTarget::setPosZ(double posZ){
	m_posZ = posZ;
	m_position->set(osg::Vec3d(m_posX, m_posY, m_posZ));
	return true;
}
bool GazeInteractionTarget::setosgGeode(osg::Geode* geode){
	m_geode = geode;
	return true;
}

int	GazeInteractionTarget::getID(){
	return m_ID;
}

std::string	GazeInteractionTarget::getName(){
	return m_Name;
}
double GazeInteractionTarget::getPosX(){
	return m_posX;
}
double GazeInteractionTarget::getPosY(){
	return m_posY;
}
double GazeInteractionTarget::getPosZ(){
	return m_posZ;
}
osg::Vec3d* GazeInteractionTarget::getPosition(){
	return m_position;
}

osg::Geode*	GazeInteractionTarget::getosgGeode(){
	return m_geode;
}
