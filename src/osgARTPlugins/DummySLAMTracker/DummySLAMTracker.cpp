/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2014 osgART Development Team
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

#include <cstring>

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/io_utils>
#include <osg/ShapeDrawable>

#include "osgART/Target"
#include "osgART/VisualTracker"
#include "osgART/Video"
#include "osgART/CameraConfiguration"
#include "osgART/Utils"
#include "osgART/PluginManager"


//create a DummyCameraConfiguration if you need to parse specific
//format file for your camera calibration
//otherwise you can use the default CameraConfiguration

class DummyCameraConfiguration : public osgART::CameraConfiguration
{

public:

	inline DummyCameraConfiguration()
	{
	}

	inline ~DummyCameraConfiguration()
	{
	}

	inline const DummyCameraConfiguration* getCameraConfiguration() const
	{
		return this;
	}

	inline bool load(const std::string& filename)
	{
		//specific load of camera parameters that can be used with this
		//tracker

		//in this example we do nothing here

		osg::notify() << "Dummy Camera Configuration load " <<std::endl;

		osg::Matrix dummy_projection_matrix_test;
		//dummy_projection_matrix_test.makePerspective(40.,1.3,1.,10000.);

		dummy_projection_matrix_test.set(
			2, 0, 0, 0,
			0, 2.66667, 0, 0,
			0, 0, -1.0002, -1,
			0, 0, -0.20002, 0);

		_projection.set(dummy_projection_matrix_test);

		//you can also setup the distorsion parameters
		//_distortion.set(val1,val2,val3,val4);

	
		return true;
	}

	inline void setSize(int width, int height)
	{
		osg::notify() << "Dummy Camera Configuration resize '" <<
			width << " x " << height << std::endl;

		//you can adjust the image resolution keeping similar projection matrix
		//you need to modify your camera projection matrix


	}

};

class DummySLAMTracker;

//create a Dummy Target if you need specific members for your target
//or specific derived class (e.g. Single, Multi, Mapping Target, etc.)
//otherwise you can use the default Target object

class DummySLAMTarget : public osgART::SLAMTarget
{

public:

	DummySLAMTarget();

	void init(const std::string& config);

    void update(osg::Matrix mat);

	//function use from the tracker to create geometry in a target
	bool updateGeometry(/*pass arguments you want */);

protected:
	virtual ~DummySLAMTarget() { 
		
		//destroy target data 
		; }
		
	//here to define your vertex, color, normal arrays
	//primitive set
	//produced by your mapping
	osg::Vec3Array* vertices;
	osg::Vec4Array* colors;
    osg::Geometry* _geometry;
};


class DummySLAMTracker : public osgART::VisualTracker
{

public:

	DummySLAMTracker();

	DummySLAMTracker(const DummySLAMTracker &, const osg::CopyOp& copyop);

	virtual ~DummySLAMTracker();

	osgART::CameraConfiguration* getOrCreateCameraConfiguration();

	void setImage(osg::Image* image,bool useInternalVideo=false);

	osgART::Target* addTarget(const std::string& config);

	bool update(osg::NodeVisitor* nv = 0L);

};



/* Implementation */


DummySLAMTarget::DummySLAMTarget()
	: osgART::SLAMTarget()
{
	//initialize a target
	//parent class initialize the geometry.
}

void
DummySLAMTarget::init(const std::string& config)
{

	//if (config.empty()) {
	//	osg::notify() << "osgART::DummySLAMTarget::init() Empty config string" << std::endl;
	//	return;
	//}

	//parse the configuration option of the target
	//std::vector<std::string> tokens = osgART::tokenize(config,";");

	//check number of required arguments
	///if (tokens.size() < 2) {
	//	OSG_WARN << "Dummy Tracker invalid config string" << std::endl;
	//	return;
	//}

	//if (tokens[0] == "id") {

		//do something

		//in this example we do nothing here
	//}
	
	//create necessary container for image
	
	//instantiate geometry

	//parent class already instantiate _geometry 
	
    _mapData=new osgART::MapData();

    _geometry=new osg::Geometry();
    _mapData->setGeometry(_geometry);

	//create content of geometry
	vertices = new osg::Vec3Array();
	colors = new osg::Vec4Array();
	
	vertices->clear();
	colors->clear();
	//if you want to use VBO
	//_geometry->setUseDisplayList(false);
	//_geometry->setUseVertexBufferObjects(true);

	//osg::VertexBufferObject* verticesVBObject = new osg::VertexBufferObject;
	//vertices->setVertexBufferObject(verticesVBObject);
	
	//osg::VertexBufferObject* colorVBObject = new osg::VertexBufferObject;
	//vertices->setVertexBufferObject(colorVBObject);
	
	_geometry->setVertexArray(vertices);
	_geometry->setColorArray(colors);
	_geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	
	//create empty primitive set
	_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

}

void
DummySLAMTarget::update(osg::Matrix mat)
{
	osg::notify(osg::DEBUG_INFO) << "osgART::DummySLAMTarget::update()" << std::endl;

	_transform=mat;
	_valid=true;


}

bool
DummySLAMTarget::updateGeometry(/*pass arguments you want */)
{
	osg::notify(osg::DEBUG_INFO) << "osgART::DummySLAMTarget::updateGeometry()" << std::endl;


	static float dummy_counter=0.;
	
	//here you can update your geometry
	//updating vertex array for points, color, normals, create 
	
	//first we clear from previously
	colors->clear();
	vertices->clear();
		    
	
    vertices->push_back(osg::Vec3f(0.,0.,5.));
    vertices->push_back(osg::Vec3f(5.,0.,5.));

    //vertices->push_back(osg::Vec3f(5.,-5.,5.)); 
    //for dummy example, we just move the last point
    vertices->push_back(osg::Vec3f(dummy_counter,-5.,5.));
    dummy_counter+=0.1;
	if (dummy_counter>20.) 
		dummy_counter=0.;

    colors->push_back(osg::Vec4f(1.,0.,0.,1.));
    colors->push_back(osg::Vec4f(0.,1.,0.,1.));
    colors->push_back(osg::Vec4f(0.,0.,1.,1.));
    
    //for dummy example, we just 
    
    //when you update your geometry
	//you need to refresh the arrays (so any attached geode will update)

	vertices->dirty();
	colors->dirty();

	_geometry->removePrimitiveSet(0,1);
	
	//create list of elements
	//osg::DrawElementsUInt* _geoelements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    //_geoelements->push_back(0);
    //_geoelements->push_back(1);
    //_geoelements->push_back(4);
    // _geometry->addPrimitiveSet(pyramidFaceOne);
   
    //or use draw arrays
	//such as point
	//_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));
	
	//or triangles
	_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));

	_geometry->dirtyDisplayList();
    //or if you use VBO:
    //geometry->dirty()
    
    _geometry->dirtyBound();

	return true;
}

DummySLAMTracker::DummySLAMTracker() :
	osgART::VisualTracker()
{
	

}

DummySLAMTracker::DummySLAMTracker(const DummySLAMTracker &, const osg::CopyOp& copyop): osgART::VisualTracker()
{

}

DummySLAMTracker::~DummySLAMTracker()
{
	//clean up the tracker

}

//this call allow to access intrinsics parameters
//for optical tracker
//it's used also for 
inline osgART::CameraConfiguration*
DummySLAMTracker::getOrCreateCameraConfiguration()
{
	//you have two choices here

	//1. you create a specific Camera Configuration class, that can hold
	//more parameters for your tracker
	if (!_cameraConfiguration.valid()) _cameraConfiguration = new DummyCameraConfiguration;

	//2.you only create a default Camera Configuration
	//and you setup the proj matrix
	//_cameraconfiguration=new osgART::CameraConfiguration();
	//_cameraconfiguration._prjection..etc 
	return osgART::Tracker::getOrCreateCameraConfiguration();
}

inline void
DummySLAMTracker::setImage(osg::Image* image,bool useInternalVideo)
{

	if (!image) {
		osg::notify() << "osgART::DummySLAMTracker::setImage() called with NULL image" << std::endl;
		return;
	}

	osgART::VisualTracker::setImage(image);

	this->getOrCreateCameraConfiguration()->setSize(*image);

	//you can initialize debug image here

	// Initialise debug image to match video image		
	//m_debugimage->allocateImage(image->s(), image->t(), 1, image->getPixelFormat(), image->getDataType());


}

inline osgART::Target*
DummySLAMTracker::addTarget(const std::string& config)
{
	//create a new targets
	DummySLAMTarget* target = new DummySLAMTarget();
	target->init(config);

	//you can also have different type of targets such as
	//multitarget, imagetarget, etc
	//and use contentof the config string to define them

	//add it to the list
	_targetList.push_back(target);

	return target;
}

inline bool
DummySLAMTracker::update(osg::NodeVisitor* nv)
{
	const osg::FrameStamp* framestamp = (nv) ? nv->getFrameStamp() : 0L;

	//check first if we get a new image to update
	//the tracking value
	if (_imagesource->valid() && 
		_imagesource->getModifiedCount() != _modifiedCount && 
		_imagesource->data() != NULL) {

		osg::notify(osg::DEBUG_INFO) << "osgART::DummySLAMTracker::update()" << std::endl;

		osg::Timer t;

		t.setStartTick();

		{
			//lock the video image
			//OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(_imagesource->getMutex());

			//potentially buffer it to a temporary data structure
			//and update your tracker

			//my_external_code->update(_imagesource->data());

		}

		//detect, compute pose of your targets
		//

		//report stats
		if (framestamp && _stats.valid())
		{
			_stats->setAttribute(framestamp->getFrameNumber(),
				"Tracking time taken", t.time_m());

			//_stats->setAttribute(framestamp->getFrameNumber(),
			//	"Possible candidates", m_target_num);
		}

		//

		//once it's done, you can update your specific targets with processed information

        for (osgART::Tracker::TargetListType::iterator iter = _targetList.begin();
            iter != _targetList.end();
            iter++)
        {

             if (DummySLAMTarget* target = dynamic_cast<DummySLAMTarget*>((*iter).get())) {

				 //for this example we create a dummy transformation
				 //and pass that as argument
				 osg::Matrix dummy_mat_for_test;


				 dummy_mat_for_test.identity();
				 dummy_mat_for_test.set(
					 -0.931019,-0.155908,0.329994,0,
					  0.224063,0.46955, 0.854,0,
				     -0.288095,0.86903,-0.40223,0,
					  3.29111,4.70607,-74.1661,1);
			    // dummy_mat_for_test.translate(0.,0.,1.);

				 dummy_mat_for_test=osg::Matrix::rotate(osg::Quat(osg::DegreesToRadians(-90.0),osg::Vec3(1,0,0)))*dummy_mat_for_test;


                 target->update(dummy_mat_for_test);
                 
                 //as a slam tracker
                 //we also update any geometry
                 
                 target->updateGeometry(/* right now we are dummy so we don't do much */);

              }
		}
		//update the counter of the processed image
		_modifiedCount = _imagesource->getModifiedCount();
	}
	return true;
}


// initializer for dynamic loading
osgART::PluginProxy<DummySLAMTracker> g_DummySLAMTracker("osgart_tracker_dummyslamtracker");


