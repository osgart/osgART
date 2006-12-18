#include "GazeInteraction.h"

#include "GL/glut.h"

#include <osg/io_utils>

#include <iostream>
#include <fstream>   // file I/O
#include <iomanip>   // format manipulation
#include <string>
#include <sstream>
#include <stdio.h>



/* 
*	public
*
*/

GazeInteraction::GazeInteraction() 
{
	m_GIHit = new GazeInteractionHit();
}

GazeInteraction::~GazeInteraction() 
{			
}

bool GazeInteraction::init()
{	
	// load intersection objects/areas
	if (!readAreaMap("data/GazeInteractionAreaMap.txt")) exit(0);

	// setup osg target objects
	setupTargetObjects(m_GITargetMap);

	// line segment from camera center along negative z-axis, length INTERSECT_RAY_LENGTH
	m_lineSegment = new osg::LineSegment(osg::Vec3(0,0,0),osg::Vec3(0,0,-INTERSECT_RAY_LENGTH));
	
	return true;
}

void GazeInteraction::addTargets2Scene(osg::Group* target_scene){

	m_target_scene = target_scene;

	if (m_GITargetMap.size()){
		
		GITargetMap::iterator iter;
		for (iter = m_GITargetMap.begin(); iter != m_GITargetMap.end(); iter++) {
			// add all target objects to target scene
			GazeInteractionTarget* pGITarget = iter->second;
			m_target_scene->addChild(pGITarget->getosgGeode());
		}
	}
	// weird, no targets to add to scene ?
	else{
		std::cerr << "GazeInteraction: WARNING: No targets to add to intersection scene" << std::endl;
	}
}

void GazeInteraction::setIntersectionScene(osg::Group* intersect_scene)
{
	// store pointer to target scene
	m_intersect_scene = intersect_scene;
}

osg::Node* GazeInteraction::update()
{
	// intersect with scene
	osgUtil::IntersectVisitor intersector;
	intersector.addLineSegment(m_lineSegment.get());
	intersector.setTraversalMask( 0x1 );
	m_intersect_scene->accept(intersector);
	m_hitList = intersector.getHitList(m_lineSegment.get());

	if (!m_hitList.empty()) {
		m_hit = m_hitList.front();
		osg::NodePath& nodePath = m_hit._nodePath;
		m_hitNode = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
		
		//std::cout << "GazeInteraction:: Picked: " << m_hitNode->getName() << std::endl;
		//std::cout << "GazeInteraction:: Intersect: " << m_hit.getLocalIntersectPoint() << std::endl;
		//std::cout << "GazeInteraction:: Intersect: " << m_hit.getWorldIntersectPoint() << std::endl;
		
		m_hitNode = m_GITargetMap[m_hitNode->getName()]->getosgGeode();

		m_GIHit = new GazeInteractionHit();
		m_GIHit->setGITarget(m_GITargetMap[m_hitNode->getName()]);
		m_GIHit->setLocalIntersectPoint(m_hit.getLocalIntersectPoint());
		m_GIHit->setWorldIntersectPoint(m_hit.getWorldIntersectPoint());
		m_GIHit->isValid(true);
		
		return m_hitNode;
	}
	else{
		m_GIHit->setGITarget(NULL);
		m_GIHit->isValid(false);
		return NULL;
	}
	return NULL;
}

GazeInteraction::GITargetMap GazeInteraction::getGITargetMap(){
	return m_GITargetMap;
}


osgUtil::IntersectVisitor::HitList GazeInteraction::getHitList(){
	return m_hitList;
}

osgUtil::Hit GazeInteraction::getHit(){
	return m_hit;
}

osg::Geode* GazeInteraction::getHitNode(){
	
	if (m_hitNode != NULL){
		return (osg::Geode*)m_hitNode;
	}
	return NULL;
}

GazeInteractionHit* GazeInteraction::getGIHit(){
	return m_GIHit;
}


// debug

void GazeInteraction::outputGInteractionTargets(GITargetMap t_map)
{
	GazeInteractionTarget* GITarget;
	GazeInteractionSphereTarget* GISphereTarget;

	std::cout << std::endl << "GazeInteraction: Begin Debug GazeInteractionTargetMap" << std::endl;
	GITargetMap::iterator iter;

	for (iter = t_map.begin(); iter != t_map.end(); iter++){
	
		GITarget = iter->second;
		std::cout << std::endl;

		// output specific properties
		if (GITarget->getType() == GazeInteractionTarget::GI_SPHERE) {		
			GISphereTarget = (GazeInteractionSphereTarget*)GITarget;
			std::cout << "GI Target Type: GI_SPHERE"<< std::endl;
			std::cout << "GI Target radius: " << GISphereTarget->getRadius() << std::endl;
		}

		// output common properties
		std::cout << "GI Target Name: " << GITarget->getName() << std::endl;
		std::cout << "GI Target posX: " << GITarget->getPosX() << std::endl;
		std::cout << "GI Target posY: " << GITarget->getPosY() << std::endl;
		std::cout << "GI Target posZ: " << GITarget->getPosZ() << std::endl;

		// output specific properties
		if (GITarget->getType() == GazeInteractionTarget::GI_SPHERE) {		
			GISphereTarget = (GazeInteractionSphereTarget*)GITarget;
			std::cout << "GI Target radius: " << GISphereTarget->getRadius() << std::endl;
		}
	}
	std::cout << "GazeInteraction: End Debug GazeInteractionTargetList" << std::endl;
}





/* 
*	private
*
*/

bool GazeInteraction::readAreaMap(char* filename){

	// so far only spheres are supported as target objects
	GazeInteractionSphereTarget* GISphereTarget;

	// open file stream
	std::ifstream fp_in;
	fp_in.open(filename, std::ios::in);
	
	// check if valid
	if (!fp_in.is_open()) {
		std::cerr << "osgart_GazeInteraction Error: cannot open AreaMap: " << filename << std::endl;
		return false;
	}
	
	// read line by line
	std::string line;
	int count = 0;
	// go read
	while (!fp_in.eof()){
		getline(fp_in,line);			// get next line
		if (line.find("//") != 0){		// examine lines that do not contain comments "//"
			if (!line.empty() ){		// examine non-empty lines only
				if (!line.compare("GI_SPHERE")) {	// found sphere target definition
					GISphereTarget = new GazeInteractionSphereTarget();	// create new Sphere Interaction Target
					GISphereTarget->setID(count++);						// give each target an ID
					getline(fp_in,line);								// read name
					GISphereTarget->setName((char*)line.c_str());	
					getline(fp_in,line);								// read posX 
					GISphereTarget->setPosX(atof(line.c_str()));			
					getline(fp_in,line);								// read posY
					GISphereTarget->setPosY(atof(line.c_str()));
					getline(fp_in,line);								// read posZ
					GISphereTarget->setPosZ(atof(line.c_str()));
					getline(fp_in,line);								// read radius
					GISphereTarget->setRadius(atof(line.c_str()));
					
					m_GITargetMap[GISphereTarget->getName()] = GISphereTarget;	// add to target map
				}
				// complain about errors
				else {
					std::cerr << "osgart_GazeInteraction Error: AreaMap: " << filename << "; unsupported entry: " << line << std::endl;
				}
			}
		}
	}

	if (m_GITargetMap.size() > 0)
	{
		// report successful import
		std::cout << "GazeInteraction: Loaded " << m_GITargetMap.size() << " interaction target objects" << std::endl;
		// debug
		outputGInteractionTargets(m_GITargetMap);
		return true;	
	}
	else{
		// no valid entries in file
		std::cerr << "GazeInteraction: Couldn't load any target object from file: " << filename << std::endl;
		return false;
	}
}

bool GazeInteraction::setupTargetObjects(GITargetMap t_map){
	
	GazeInteractionTarget* pGITarget;
	GazeInteractionSphereTarget* pGISphereTarget;
	osg::ShapeDrawable* sd;
	osg::Geode* geode;
	
	std::string drawable_prefix = "sd_";
	std::string geode_prefix = "geode_";

	GITargetMap::iterator iter;
	for (iter =  m_GITargetMap.begin(); iter != m_GITargetMap.end(); iter++){

		pGITarget = iter->second;

		drawable_prefix = "sd_";
		
		// set properties specific to spheres
		if (pGITarget->getType() == GazeInteractionTarget::GI_SPHERE){
			pGISphereTarget = (GazeInteractionSphereTarget*) pGITarget;
			sd =	new osg::ShapeDrawable(
					new osg::Sphere(
					osg::Vec3(pGISphereTarget->getPosX(), pGISphereTarget->getPosY(), pGISphereTarget->getPosZ()),pGISphereTarget->getRadius()));
		}
		
		// set common properties
		sd->setColor(osg::Vec4(0, 0, 1, 1));		// color blue
		sd->setName( (drawable_prefix.append(pGITarget->getName().c_str())).c_str() );
		geode = new osg::Geode();		
		geode->addDrawable(sd);
		geode->setName(pGITarget->getName().c_str());
		pGITarget->setosgGeode(geode);
	}
	return true;
}
