#include "GazeInteractionSphereTarget.h"


GazeInteractionSphereTarget::GazeInteractionSphereTarget() : GazeInteractionTarget() 
{			
	m_Type = GazeInteractionTarget::GI_SPHERE;
}

GazeInteractionSphereTarget::~GazeInteractionSphereTarget() 
{			
}


bool GazeInteractionSphereTarget::setRadius(int radius)
{
	m_radius = radius;
	return true;
}

GazeInteractionTarget::GITargetType GazeInteractionSphereTarget::getType()
{
	return m_Type;
}

int GazeInteractionSphereTarget::getRadius()
{
	return m_radius;
}