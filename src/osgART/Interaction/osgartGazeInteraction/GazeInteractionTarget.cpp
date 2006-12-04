#include "GazeInteractionTarget.h"

GazeInteractionTarget::GazeInteractionTarget() 
{	

}

GazeInteractionTarget::~GazeInteractionTarget() 
{			
}

bool GazeInteractionTarget::setName(char* name){
	m_Name = std::string( name );
	return true;
}
bool GazeInteractionTarget::setPosX(int posX){
	m_posX = posX;
	return true;
}
bool GazeInteractionTarget::setPosY(int posY){
	m_posY = posY;
	return true;
}
bool GazeInteractionTarget::setPosZ(int posZ){
	m_posZ = posZ;
	return true;
}
bool GazeInteractionTarget::setosgGeode(osg::Geode* geode){
	m_geode = geode;
	return true;
}

std::string	GazeInteractionTarget::getName(){
	return m_Name;
}
int GazeInteractionTarget::getPosX(){
	return m_posX;
}
int GazeInteractionTarget::getPosY(){
	return m_posY;
}
int GazeInteractionTarget::getPosZ(){
	return m_posZ;
}
osg::Geode*	GazeInteractionTarget::getosgGeode(){
	return m_geode;
}
