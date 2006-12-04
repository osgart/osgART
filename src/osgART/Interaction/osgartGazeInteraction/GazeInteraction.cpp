#include "GazeInteraction.h"
//#include "GazeInteractionSphereTarget.h"

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
}

GazeInteraction::~GazeInteraction() 
{			
}

bool GazeInteraction::init()
{	
	// load intersection objects/areas
	if (!readAreaMap("data/GazeInteractionAreaMap.txt")) exit(0);

	// setup osg target objects
	setupTargetObjects(m_GITargetList);

	// line segment from camera center along negative z-axis, length INTERSECT_RAY_LENGTH
	m_lineSegment = new osg::LineSegment(osg::Vec3(0,0,0),osg::Vec3(0,0,-INTERSECT_RAY_LENGTH));
	
	return true;
}

void GazeInteraction::addTargets2Scene(osg::Group* target_scene){

	m_target_scene = target_scene;

	// add all target objects to target scene
	if (!m_GITargetList.empty()){
		for (int i = 0; i < m_GITargetList.size(); i++){
			GazeInteractionTarget* pGITarget = m_GITargetList[i];
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
	osgUtil::IntersectVisitor::HitList& hits=intersector.getHitList(m_lineSegment.get());

	if (!hits.empty()) {
		osgUtil::Hit& hit = hits.front();
		osg::NodePath& nodePath = hit._nodePath;
		osg::Node* node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
		printf("Picked: %s\n", node->getName().c_str());
		return node;
	}
	else{
		return NULL;
	}
	return NULL;
}



// debug

void GazeInteraction::outputGInteractionTargets(std::vector<GazeInteractionTarget*> list)
{
	GazeInteractionTarget* GITarget;
	GazeInteractionSphereTarget* GISphereTarget;

	std::cout << std::endl << "GazeInteraction: Begin Debug GazeInteractionTargetList" << std::endl;
	for (int i = 0; i < list.size(); i++)
	{
		GITarget = (GazeInteractionTarget*) list[i];
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
	
	// go read
	while (!fp_in.eof()){
		getline(fp_in,line);			// get next line
		if (line.find("//") != 0){		// examine lines that do not contain comments "//"
			if (!line.empty() ){		// examine non-empty lines only
				if (!line.compare("GI_SPHERE")) {							// found sphere target definition
					GISphereTarget = new GazeInteractionSphereTarget();	// create new Sphere Interaction Target
					getline(fp_in,line);								// read name
					GISphereTarget->setName((char*)line.c_str());	
					getline(fp_in,line);								// read posX 
					GISphereTarget->setPosX(atoi(line.c_str()));			
					getline(fp_in,line);								// read posY
					GISphereTarget->setPosY(atoi(line.c_str()));
					getline(fp_in,line);								// read posZ
					GISphereTarget->setPosZ(atoi(line.c_str()));
					getline(fp_in,line);								// read radius
					GISphereTarget->setRadius(atoi(line.c_str()));
					
					m_GITargetList.push_back(GISphereTarget);			// add to target list
				}
				// complain about errors
				else {
					std::cerr << "osgart_GazeInteraction Error: AreaMap: " << filename << "; unsupported entry: " << line << std::endl;
				}
			}
		}
	}
	
	if (m_GITargetList.size() > 0)
	{
		// report successful import
		std::cout << "GazeInteraction: Loaded " << m_GITargetList.size() << " interaction target objects" << std::endl;
		// debug
		outputGInteractionTargets(m_GITargetList);
		return true;	
	}
	else{
		// no valid entries in file
		std::cerr << "GazeInteraction: Couldn't load any target object from file: " << filename << std::endl;
		return false;
	}
}

bool GazeInteraction::setupTargetObjects(std::vector<GazeInteractionTarget*> list){
	
	GazeInteractionTarget* pGITarget;
	GazeInteractionSphereTarget* pGISphereTarget;
	osg::ShapeDrawable* sd;
	osg::Geode* geode;
	
	for (int i = 0; i < list.size(); i++) {
		
		pGITarget = list[i];
		
		// set properties specific to spheres
		if (pGITarget->getType() == GazeInteractionTarget::GI_SPHERE){
			pGISphereTarget = (GazeInteractionSphereTarget*) list[i];
			sd =	new osg::ShapeDrawable(
					new osg::Sphere(
					osg::Vec3(pGISphereTarget->getPosX(), pGISphereTarget->getPosY(), pGISphereTarget->getPosZ()),pGISphereTarget->getRadius()));
		}
		
		// set common properties
		sd->setColor(osg::Vec4(0, 0, 1, 1));		// color blue
		osg::Geode* geode = new osg::Geode();		
		geode->addDrawable(sd);
		geode->setName(pGITarget->getName().c_str());
		pGITarget->setosgGeode(geode);
	}
	return true;
}
