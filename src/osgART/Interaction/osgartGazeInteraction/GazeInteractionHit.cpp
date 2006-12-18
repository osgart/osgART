#include "GazeInteractionHit.h"

GazeInteractionHit::GazeInteractionHit() 
{	
	this->setGITarget(NULL);
	this->setLocalIntersectPoint(osg::Vec3(0,0,0));
	this->setWorldIntersectPoint(osg::Vec3(0,0,0));
	this->m_isValid = false;
}

GazeInteractionHit::~GazeInteractionHit() 
{			
}


void GazeInteractionHit::setGITarget(GazeInteractionTarget* target){
	m_target = target;
}
void GazeInteractionHit::setLocalIntersectPoint(osg::Vec3d local_intersect){
	// set new intersect point
	m_LocalIntersectPoint = new osg::Vec3d(local_intersect);
	
	if (m_target != NULL){
		// distance from intersection to target's origin (3D)
		osg::Vec3d i_pos3 = osg::Vec3d(m_LocalIntersectPoint->x(),m_LocalIntersectPoint->y(),m_LocalIntersectPoint->z());
		osg::Vec3d t_pos3 = osg::Vec3d(m_target->getPosition()->x(),m_target->getPosition()->y(),m_target->getPosition()->z());
		m_LocalCntr2IntersectDist = (i_pos3 - t_pos3).length();

		// distance from intersection to target's origin (2D)
		osg::Vec2d i_pos2 = osg::Vec2d(m_LocalIntersectPoint->x(),m_LocalIntersectPoint->y());
		osg::Vec2d t_pos2 = osg::Vec2d(m_target->getPosition()->x(),m_target->getPosition()->y());
		m_LocalCntr2IntersectDistProj = (i_pos2 - t_pos2).length();
	}
}
void GazeInteractionHit::setWorldIntersectPoint(osg::Vec3d world_intersect){
	m_WorldIntersectPoint = new osg::Vec3d(world_intersect);
}

GazeInteractionTarget*	GazeInteractionHit::getGITarget(){
	return m_target;
}
osg::Vec3d* GazeInteractionHit::getLocalIntersectPoint(){
	return m_LocalIntersectPoint;
}
osg::Vec3d* GazeInteractionHit::getWorldIntersectPoint(){
	return m_WorldIntersectPoint;
}
double GazeInteractionHit::getLocalCntr2IntersectDist(){	
	return m_LocalCntr2IntersectDist;
}
double GazeInteractionHit::getLocalCntr2IntersectDistProj(){
	return m_LocalCntr2IntersectDistProj;
}

bool GazeInteractionHit::isValid(){
	return m_isValid;
}
bool GazeInteractionHit::isValid(bool state){
	m_isValid = state;
	return m_isValid;
}