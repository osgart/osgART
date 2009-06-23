#ifndef GAZEINTERACTIONHIT_H
#define GAZEINTERACTIONHIT_H

#include "GazeInteractionTarget.h"



class GazeInteractionHit {

public:
		
	GazeInteractionHit();

	/**
		* \brief destructor.
		*/
	~GazeInteractionHit();

	void			setGITarget(GazeInteractionTarget* target);
	void			setLocalIntersectPoint(osg::Vec3d local_intersect);
	void			setWorldIntersectPoint(osg::Vec3d world_intersect);
	
	
	GazeInteractionTarget*		getGITarget();
	osg::Vec3d*					getLocalIntersectPoint();
	osg::Vec3d*					getWorldIntersectPoint();
	double						getLocalCntr2IntersectDist();
	double						getLocalCntr2IntersectDistProj();

	bool						isValid();
	bool						isValid(bool);

protected:

	GazeInteractionTarget*	m_target;
	osg::Vec3d*				m_LocalIntersectPoint;
	osg::Vec3d*				m_WorldIntersectPoint;
	double					m_LocalCntr2IntersectDist;
	double					m_LocalCntr2IntersectDistProj;
	bool					m_isValid;

private:
	
};
#endif