/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osgParticle/PointPlacer>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/ModularProgram>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/AccelOperator>

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osg/MatrixTransform>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <osgART/PluginManager>

class EmitterUpdateCallback : public osg::NodeCallback {

private:

	osgParticle::PointPlacer* placer;
	osg::MatrixTransform* trans;

public:

	EmitterUpdateCallback(osgParticle::PointPlacer* p, osg::MatrixTransform* mt) {
		placer = p;
		trans = mt;
	}

	void operator() (osg::Node* node, osg::NodeVisitor* nv) {
		placer->setCenter(trans->getMatrix().getTrans());
		traverse(node,nv);
	}
};

class SmokeParticleSystem : public osg::Group {
private:
	osgParticle::PointPlacer* placer;
public:
	SmokeParticleSystem(osg::Group* scene, osg::MatrixTransform* mt)  { 
		/* Build the particle system in here ... */
		osgParticle::ParticleSystem* dustParticleSystem = new osgParticle::ParticleSystem;
		osg::Geode *geode = new osg::Geode();
		geode->addDrawable(dustParticleSystem);
		this->addChild(geode);

		dustParticleSystem->setDefaultAttributes("media/images/dust.png", false, false);

		osgParticle::Particle smokeParticle;
		smokeParticle.setSizeRange(osgParticle::rangef(4.0f,20.0f)); // meters
		smokeParticle.setLifeTime(15); // seconds
		smokeParticle.setMass(0.4f);
		smokeParticle.setColorRange(osgParticle::rangev4( 
			osg::Vec4(1, 1.0f, 1.0f, 1.0f),
			osg::Vec4(0, 0, 0, 0.3f)));
		dustParticleSystem->setDefaultParticleTemplate(smokeParticle);

		osgParticle::ParticleSystemUpdater *dustSystemUpdater = new osgParticle::ParticleSystemUpdater;
		dustSystemUpdater->addParticleSystem(dustParticleSystem);
		this->addChild(dustSystemUpdater);

		osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
		emitter->setParticleSystem(dustParticleSystem);
		this->addChild(emitter);

		osgParticle::RandomRateCounter *dustRate = 
			static_cast<osgParticle::RandomRateCounter *>(emitter->getCounter());
		dustRate->setRateRange(40,80);

		osgParticle::PointPlacer* placer = 
			static_cast<osgParticle::PointPlacer *>(emitter->getPlacer());
		this->setUpdateCallback(new EmitterUpdateCallback(placer, mt));

		osgParticle::RadialShooter* shooter =
			static_cast<osgParticle::RadialShooter *>(emitter->getShooter());
		shooter->setThetaRange(0, 3.15149/4.0f ); // radians, relative to Z axis.
		shooter->setInitialSpeedRange(4,5); // meters/second
		emitter->setShooter(shooter);

		osgParticle::ModularProgram *program = new osgParticle::ModularProgram;
		program->setParticleSystem(dustParticleSystem);
		scene->addChild(program); 

		osgParticle::FluidFrictionOperator *airFriction = new osgParticle::FluidFrictionOperator;
		airFriction->setFluidToAir();
		airFriction->setWind(osg::Vec3(0.25, 0, 0));
		program->addOperator(airFriction);

		osgParticle::AccelOperator *accelUp = new osgParticle::AccelOperator;
		accelUp->setToGravity(0.4);
		program->addOperator(accelUp);
	}
};

int main(int argc, char* argv[])  {

	//ARGUMENTS INIT

	osgART::PluginManager::instance()->load("osgart_video_artoolkit");
	osgART::PluginManager::instance()->load("osgart_tracker_artoolkit");

	osgViewer::Viewer viewer;
	
	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);


	osgART::Scene* scene = new osgART::Scene();


	scene->addVideoBackground("osgart_video_artoolkit");
	scene->addTracker("osgart_tracker_artoolkit","data/artoolkit/camera_para.dat");

	osg::MatrixTransform* mt = scene->addTrackedTransform("single;data/artoolkit/patt.hiro;80;0;0");

	/** ... Rotation animation ... **/
	osg::ref_ptr<osg::MatrixTransform> MTrocketA = new osg::MatrixTransform();
	mt->addChild(MTrocketA.get());
	MTrocketA->addUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0,0.0,100.0), 
		osg::Y_AXIS, osg::inDegrees(45.0f)));

	/** ... Intitial translation to orient rocket ... **/
	osg::ref_ptr<osg::MatrixTransform> MTlocalrocketA = 
		new osg::MatrixTransform(osg::Matrixd::rotate(osg::DegreesToRadians(90.0f), 
		osg::Z_AXIS));
	MTlocalrocketA->addChild(osgDB::readNodeFile("media/models/rocket.osg"));
	MTrocketA->addChild(MTlocalrocketA.get());

	osg::ref_ptr<SmokeParticleSystem> smoke = new SmokeParticleSystem(scene, MTrocketA.get());	
	mt->addChild(smoke);

	viewer.setSceneData(scene);

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
