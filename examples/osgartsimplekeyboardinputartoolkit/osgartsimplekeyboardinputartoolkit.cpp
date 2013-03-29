/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osg/MatrixTransform>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <osgART/PluginManager>


class MyKeyboardEventHandler : public osgGA::GUIEventHandler {
 
protected:
	osg::MatrixTransform* _driveCar;

public:
    MyKeyboardEventHandler(osg::MatrixTransform* drivecar) : osgGA::GUIEventHandler() {_driveCar=drivecar; }      
 
 
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

	osgART::PluginManager::instance()->load("osgart_video_artoolkit2");
	osgART::PluginManager::instance()->load("osgart_tracker_artoolkit2");

	osgViewer::Viewer viewer;
	
	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
	viewer.addEventHandler(new osgViewer::ThreadingHandler);
	viewer.addEventHandler(new osgViewer::HelpHandler);


	osgART::Scene* scene = new osgART::Scene();

	scene->addVideoBackground("osgart_video_artoolkit2");
	scene->addTracker("osgart_tracker_artoolkit2","data/artoolkit2/camera_para.dat");

	osg::MatrixTransform* mt = scene->addTrackedTransform("single;data/artoolkit2/patt.hiro;80;0;0");
	
	osg::ref_ptr<osg::MatrixTransform> driveCar = new osg::MatrixTransform();
	driveCar->addChild(osgDB::readNodeFile("media/models/car.ive"));
	mt->addChild(driveCar.get());

	viewer.addEventHandler(new MyKeyboardEventHandler(driveCar)); // Our handler

	viewer.setSceneData(scene);

	//run call is equivalent to a while loop with a viewer.frame call
	return viewer.run();
	
}
