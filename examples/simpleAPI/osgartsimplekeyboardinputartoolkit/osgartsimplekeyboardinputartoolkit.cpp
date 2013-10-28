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

#include <osg/MatrixTransform>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgART/Scene>
#include <osgART/PluginManager>
#include <osgART/GeometryUtils>


class KeyboardEventHandler : public osgGA::GUIEventHandler {
 
protected:
	osg::MatrixTransform* _driveCar;

public:
    KeyboardEventHandler(osg::MatrixTransform* drivecar) : osgGA::GUIEventHandler() {_driveCar=drivecar; }      
 
 
    /**
        OVERRIDE THE HANDLE METHOD:
        The handle() method should return true if the event has been dealt with
        and we do not wish it to be handled by any other handler we may also have
        defined. Whether you return true or false depends on the behaviour you 
        want - here we have no other handlers defined so return true.
    **/
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, 
                        osg::Object* obj, osg::NodeVisitor* nv) { 
 
        switch (ea.getEventType()) {
 
 
            /** KEY EVENTS:
                Key events have an associated key and event names.
                In this example, we are interested in keys up/down/right/left arrow
                and space bar.
                If we detect a press then we modify the transformation matrix 
                of the local transform node. **/
            case osgGA::GUIEventAdapter::KEYDOWN: {
 
                switch (ea.getKey()) {
 
                    case osgGA::GUIEventAdapter::KEY_Up: // Move forward 5mm
                        _driveCar->preMult(osg::Matrix::translate(0, -5, 0));
                        return true;
 
                    case osgGA::GUIEventAdapter::KEY_Down: // Move back 5mm
                        _driveCar->preMult(osg::Matrix::translate(0, 5, 0));
                        return true; 
 
                    case osgGA::GUIEventAdapter::KEY_Left: // Rotate 10 degrees left
                        _driveCar->preMult(osg::Matrix::rotate(osg::DegreesToRadians(10.0f),  
                                                       osg::Z_AXIS));
                        return true;
 
                    case osgGA::GUIEventAdapter::KEY_Right: // Rotate 10 degrees right
                        _driveCar->preMult(osg::Matrix::rotate(osg::DegreesToRadians(-10.0f),  
                                                       osg::Z_AXIS));
                        return true;
 
                    case ' ': // Reset the transformation
                       _driveCar->setMatrix(osg::Matrix::identity());
                        return true;
                }
 
                   default: return false;
            }
 
 
        }
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

	//AR SCENEGRAPH INIT

	osgART::Scene* scene = new osgART::Scene();

	scene->addVideoBackground("osgart_artoolkit","osgart_video_artoolkit");
	scene->addVisualTracker("osgart_artoolkit","osgart_tracker_artoolkit","data/artoolkit/camera_para.dat");

	osg::ref_ptr<osg::MatrixTransform> mt = scene->addTrackedTransform("single;data/artoolkit/patt.hiro;80;0;0");
	
	osg::ref_ptr<osg::MatrixTransform> driveCar = new osg::MatrixTransform();
	driveCar->addChild(osgDB::readNodeFile("media/models/car.ive"));
	mt->addChild(driveCar.get());

	//add our keyboard event handler
	viewer.addEventHandler(new KeyboardEventHandler(driveCar));

	//APPLICATION INIT

	//BOOTSTRAP INIT

	viewer.setSceneData(scene);

	//MAIN LOOP & EXIT CLEANUP

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
