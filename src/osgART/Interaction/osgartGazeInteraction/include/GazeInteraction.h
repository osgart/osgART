#include <osg/Matrix>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

#include <AR/ar.h>		// ARUint8, AR_PIXEL_FORMAT
#include <AR/param.h>	
#include <AR/gsub_lite.h>

#include "GazeInteractionSphereTarget.h"

#include <string>

#define INTERSECT_RAY_LENGTH 10000

class GazeInteraction {

public:
		
	GazeInteraction();
	~GazeInteraction();

	bool init();
	
	void addTargets2Scene(osg::Group* target_scene);

	void setIntersectionScene(osg::Group* intersect_scene);

	osg::Node* update();

	// debug
	void outputGInteractionTargets(std::vector<GazeInteractionTarget*> list);

protected:

private:
	
	bool GazeInteraction::readAreaMap(char* filename);

	bool setupTargetObjects(std::vector<GazeInteractionTarget*> list);

	/**
		* type for a vector of reference counted GazeInteraction Target objects.
		*/
	typedef std::vector<GazeInteractionTarget*> GITargetList;
	/**
		* A list of GazeInteraction Target objects associated with this GazeInteraction.
		*/ 
	GITargetList m_GITargetList;

	osg::Group* m_intersect_scene;
	osg::Group* m_target_scene;

	osg::ref_ptr<osg::LineSegment> m_lineSegment;
	
};