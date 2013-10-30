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

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osg/MatrixTransform>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <osgART/PluginManager>


/**
MOUSE PICKING EVENT HANDLER:
To handle keyboard and mouse events in OSG, we need to define a subclass
of osgGA::GUIEventHandler, and override the handle() method with custom code.
**/
class MousePickingEventHandler : public osgGA::GUIEventHandler {
 
    protected:
 
    float _mX, _mY;

	osgART::Scene* _scene;
 
    public:
 
    MousePickingEventHandler(osgART::Scene* scene) : osgGA::GUIEventHandler(),_scene(scene) {}
 
 
    /** OVERRIDE THE HANDLE METHOD:
        The handle() method should return true if the event has been dealt with
        and no if it's passed to other handlers.
		We do not wish it to be handled by any other handler we may also have
        defined. Whether you return true or false depends on the behaviour you
        want - here we have no other handlers defined so return true. **/
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) {
 

        switch (ea.getEventType()) {
 
 
            /** HANDLE MOUSE EVENTS:
                Mouse events have associated event names, and mouse
                position co-ordinates. **/
            case osgGA::GUIEventAdapter::RELEASE: {
 
				//SOLUTION: you do the picking in another method of your event handler
				//osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
				//pick(ea,viewer);

				//SOLUTION 2:
				//you do the picking directly here in the release event

                /** PERFORM SOME ACTION:
                    Once we have received the mouse event, we can
                    perform some action. Here we perform mouse picking.
 
                    Mouse picking can be thought of as shooting a "ray"
                    from the mouse position into our scene, and determining
                    which parts of the scene intersect with the ray.
                    Typically we will only be interested in the first false
                    (if any) object that the ray hits. We can then perform
                    some action.
 
                    In this example, we are going to interact with our
                    car model - if the mouse has "picked" it, we will
                    start a simple rotation on it. **/
 
 
 
                /** PERFORM MOUSE PICKING:
                    In OSG, mouse picking can be done in a few slightly
                    different ways, however, the OSG documentation recommends
                    using the following steps: 
					(see for example osgintersection example)
				**/
 
                /** 1. Create either a PolytopeIntersector, or a LineIntersector
                    using the normalized mouse co-ordinates.**/
                osg::ref_ptr<osgUtil::LineSegmentIntersector> lI = new
                    osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION,
                        ea.getXnormalized(), ea.getYnormalized());
 
                /** 2. Create an IntersectionVisitor, passing the
                    Intersector as parameter to the constructor. **/
                osgUtil::IntersectionVisitor iV(lI);
 
                /** 3. Launch the IntersectionVisitor on the root node of
                    the scene graph. In an OSGART application, we can
                    launch it on the osgART::Scene node. **/
                _scene->accept(iV);
 
                /** 4. If the Intersector contains any intersections
                    obtain the NodePath and search it to find the
                    Node of interest. **/
 
                if (lI->containsIntersections()) {
                    osg::NodePath nodePath = lI->getFirstIntersection().nodePath;
  
                    /** Here we search the NodePath for the node of interest.
                        This is where we can make use of our node naming.
 
                        If we find the Transform node named "CAR", we obtain its
                        AnimationPathCallback - if it is currently paused
                        we "un-pause" it, and vice-versa. **/
                    for (unsigned int i = 0; i <= (nodePath.size()-1); i++) {
						std::cout<<i<<std::endl;
						std::cout<<nodePath[i]->getName()<<std::endl;
                        if (nodePath[i]->getName() == "CAR") {
							std::cout<<"Hit the car !!"<<std::endl;
                            osg::AnimationPathCallback* cb = 
                                dynamic_cast<osg::AnimationPathCallback*>(nodePath[i]->getUpdateCallback());
 
                            if (cb->getPause()==true)
                                cb->setPause(false);
                            else cb->setPause(true);

							return true;
                         }
                    }
                }
            } 
            default: 
				return false;
        } 
    } 

	/*
	void pick(const osgGA::GUIEventAdapter& ea, osgViewer::Viewer* viewer) {

		// 1. Create either a PolytopeIntersector, or a LineIntersector
        //    using the normalized mouse co-ordinates.
        osg::ref_ptr<osgUtil::LineSegmentIntersector> lI = new
            osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION,
                ea.getXnormalized(), ea.getYnormalized());
 
        // 2. Create an IntersectionVisitor, passing the
        //    Intersector as parameter to the constructor.
        osgUtil::IntersectionVisitor iV(lI);
 
        // 3. Launch the IntersectionVisitor on the root node of
        //    the scene graph. In an OSGART application, we can
        //    launch it on the osgART::Scene node. 
        _scene->accept(iV);
 
        // 4. If the Intersector contains any intersections
        //    obtain the NodePath and search it to find the
        //    Node of interest. 
 
        if (lI->containsIntersections()) {
            osg::NodePath nodePath = lI->getFirstIntersection().nodePath;
  
            // Here we search the NodePath for the node of interest.
            //  This is where we can make use of our node naming.
 
           //    If we find the Transform node named "CAR", we obtain its
           //    AnimationPathCallback - if it is currently paused
           //    we "un-pause" it, and vice-versa. 
            for (unsigned int i = 0; i <= (nodePath.size()-1); i++) {
				std::cout<<i<<std::endl;
				std::cout<<nodePath[i]->getName()<<std::endl;
                if (nodePath[i]->getName() == "CAR") {
					std::cout<<"Hit the car !!"<<std::endl;
                    osg::AnimationPathCallback* cb = 
                        dynamic_cast<osg::AnimationPathCallback*>(nodePath[i]->getUpdateCallback());
 
                    if (cb->getPause()==true)
                        cb->setPause(false);
                    else cb->setPause(true);

					return true;
                    }
            } //for node path
      
		} //if intersection
	}*/
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

	//create an osgART::Scene
	osgART::Scene* scene = new osgART::Scene();

	//add a video background (video plugin name, video configuration)
	scene->addVideo("osgart_video_dummyvideo","osgart_video_dummyvideo","Data/dummyvideo/dummyvideo.png");
	//add a tracker (tracker plugin name,calibration configuration, tracker configuration)
	scene->addVisualTracker("osgart_tracker_dummytracker","osgart_tracker_dummytracker","","mode=0;");

	osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("test.pattern;35.2;22.0;0.3");

	osg::ref_ptr<osg::MatrixTransform> carMT = new osg::MatrixTransform();
	carMT->addChild(osgART::scaleModel(osgDB::readNodeFile("media/models/car.ive"),0.5));
	carMT->addUpdateCallback(new osg::AnimationPathCallback(osg::Vec3(0.0,0.0,0.0),
		osg::Z_AXIS,
		osg::inDegrees(45.0f)));

	mt->addChild(carMT.get());

	osg::AnimationPathCallback* cb =
		dynamic_cast<osg::AnimationPathCallback*>(carMT->getUpdateCallback());

	cb->setPause(true);

	carMT->setName("CAR");
	//you can also use the description for this purpose
	//carMT->addDescription("CAR");
	
	//add our handler
	viewer.addEventHandler(new MousePickingEventHandler(scene)); // Our handler

	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);

	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();	
}
