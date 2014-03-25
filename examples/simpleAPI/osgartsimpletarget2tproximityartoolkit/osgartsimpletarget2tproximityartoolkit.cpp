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

// std include

// OpenThreads include

// OSG include
#include <osg/MatrixTransform>

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

// osgART include
#include <osgART/PluginManager>
#include <osgART/Scene>
#include <osgART/GeometryUtils>

// local include



class TargetProximityUpdateCallback : public osg::NodeCallback {
 
	private:
		osg::MatrixTransform* mtA;
		osg::MatrixTransform* mtB;
 
		osg::Switch* mSwitchA;
		osg::Switch* mSwitchB;
 
		float mThreshold;
 
	public:
 
		TargetProximityUpdateCallback(
		osg::MatrixTransform* mA, osg::MatrixTransform* mB, 
			osg::Switch* switchA, osg::Switch* switchB,
			float threshold) : 
			                     osg::NodeCallback(), 
					    mtA(mA), mtB(mB),
					    mSwitchA(switchA), mSwitchB(switchB),
					    mThreshold(threshold) { }
 
 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
 
			/** CALCULATE INTER-TARGET PROXIMITY:
				Here we obtain the current position of each target, and the
				distance between them by examining
				the translation components of their parent transformation 
				matrices **/
			osg::Vec3 posA = mtA->getMatrix().getTrans();
			osg::Vec3 posB = mtB->getMatrix().getTrans();
			osg::Vec3 offset = posA - posB;
			float distance = offset.length();
 
			/** LOAD APPROPRIATE MODELS:
				Here we use each target's OSG Switch node to swap between
				models, depending on the inter-target distance we have just 
				calculated. **/
			if (distance <= mThreshold) {
				if (mSwitchA->getNumChildren() > 1) mSwitchA->setSingleChildOn(1);
				if (mSwitchB->getNumChildren() > 1) mSwitchB->setSingleChildOn(1);
			} else {
				if (mSwitchA->getNumChildren() > 0) mSwitchA->setSingleChildOn(0);
				if (mSwitchB->getNumChildren() > 0) mSwitchB->setSingleChildOn(0);
			}
 
			traverse(node,nv);
 
		}
};




int main(int argc, char* argv[])  {

	//ARGUMENTS INIT

	//VIEWER INIT

	//create a default viewer
	osgViewer::Viewer viewer;

	//setup default threading mode
	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);//stats, press 's'
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);//resize, fullscreen 'f'
	viewer.addEventHandler(new osgViewer::ThreadingHandler);//threading mode, press 't'
	viewer.addEventHandler(new osgViewer::HelpHandler);//help menu, press 'h'

	//AR INIT

	//AR SCENEGRAPH INIT

	osgART::Scene* scene = new osgART::Scene();

	scene->addVideo("osgart_artoolkit","osgart_video_artoolkit");
	scene->addVisualTracker("osgart_artoolkit","osgart_tracker_artoolkit","data/artoolkit/camera_para.dat");
	
	osg::MatrixTransform* mtA = scene->addTrackedTransform("single;data/artoolkit/patt.hiro;80;0;0");
	osg::MatrixTransform* mtB = scene->addTrackedTransform("single;data/artoolkit/patt.kanji;80;0;0");

	osg::ref_ptr<osg::Switch> switchA = new osg::Switch();
	switchA->addChild(osgDB::readNodeFile("media/models/voltmeter_low.osg"), true);
	switchA->addChild(osgDB::readNodeFile("media/models/voltmeter_high.osg"), false);
	mtA->addChild(switchA.get());

	osg::ref_ptr<osg::Switch> switchB = new osg::Switch();
	switchB->addChild(osgDB::readNodeFile("media/models/battery.osg"), true);
	switchB->addChild(osgDB::readNodeFile("media/models/battery_spark.osg"), false);
	mtB->addChild(switchB.get());

	scene->setUpdateCallback(new TargetProximityUpdateCallback(mtA, mtB, 
		switchA.get(), switchB.get(), 200.0f));

	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);

	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
}
