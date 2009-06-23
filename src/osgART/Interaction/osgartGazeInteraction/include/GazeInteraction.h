#ifndef GAZEINTERACTION_H
#define GAZEINTERACTION_H

#include <osg/Matrix>
#include <osgUtil/SceneView>
#include <osgUtil/IntersectVisitor>

#include "GazeInteractionTarget.h"
#include "GazeInteractionSphereTarget.h"
#include "GazeInteractionHit.h"

#include <string>
#include <Map>

#define INTERSECT_RAY_LENGTH 10000


class GazeInteraction {

public:

	typedef std::map<std::string, GazeInteractionTarget*> GITargetMap;
		
	GazeInteraction();
	~GazeInteraction();

	bool								init();
	void								addTargets2Scene(osg::Group* target_scene);
	void								setIntersectionScene(osg::Group* intersect_scene);
	osg::Node*							update();
	GazeInteraction::GITargetMap		getGITargetMap();
	osgUtil::IntersectVisitor::HitList	getHitList();
	osgUtil::Hit						getHit();
	osg::Geode*							getHitNode();
	GazeInteractionHit*					getGIHit();

	// debug
	void								outputGInteractionTargets(GITargetMap t_map);

protected:

private:
	
	bool GazeInteraction::readAreaMap(char* filename);

	bool setupTargetObjects(GITargetMap t_map);

	GITargetMap m_GITargetMap;

	osg::Group* m_intersect_scene;
	osg::Group* m_target_scene;

	GazeInteractionHit* m_GIHit;

	osg::ref_ptr<osg::LineSegment> m_lineSegment;

	osgUtil::IntersectVisitor::HitList m_hitList;
	osgUtil::Hit m_hit;
	osg::Node* m_hitNode;
	
};
#endif