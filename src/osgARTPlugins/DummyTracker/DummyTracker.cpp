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

#include <osg/Notify>

#include "osgART/Target"
#include "osgART/VisualTracker"
#include "osgART/Video"
#include "osgART/CameraConfiguration"
#include "osgART/Utils"
#include "osgART/PluginManager"

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/io_utils>

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

class DummyTracker;

//create a Dummy Target if you need specific members for your target
//or specific derived class (e.g. Single, Multi, Mapping Target, etc.)
//otherwise you can use the default Target object

class DummyTarget : public osgART::Target
{

public:

	DummyTarget();

	void init(const std::string& config);

    void update(osg::Matrix mat);

protected:
	virtual ~DummyTarget() { 
		
		//destroy target data 
		; }
};


class DummyTracker : public osgART::VisualTracker
{

public:

	DummyTracker();
	virtual ~DummyTracker();

	osgART::CameraConfiguration* getOrCreateCameraConfiguration();

	void setImage(osg::Image* image,bool useInternalVideo=false);

	osgART::Target* addTarget(const std::string& config);

	bool update(osg::NodeVisitor* nv = 0L);

};



/* Implementation */


DummyTarget::DummyTarget()
	: osgART::Target()
{
	//initialize a target
}


void
DummyTarget::init(const std::string& config)
{

	if (config.empty()) {
		osg::notify() << "osgART::DummyTarget::init() Empty config string" << std::endl;
		return;
	}

	//parse the configuration option of the target
	std::vector<std::string> tokens = osgART::tokenize(config,";");

	//check number of required arguments
	///if (tokens.size() < 2) {
	//	OSG_WARN << "Dummy Tracker invalid config string" << std::endl;
	//	return;
	//}

	if (tokens[0] == "id") {

		//do something

		//in this example we do nothing here
	}
}

void
DummyTarget::update(osg::Matrix mat)
{
	osg::notify(osg::DEBUG_INFO) << "osgART::DummyTarget::update()" << std::endl;

	_transform=mat;
	_valid=true;


}



DummyTracker::DummyTracker() :
	osgART::VisualTracker()
{
	

}

DummyTracker::~DummyTracker()
{
	//clean up the tracker

}

//this call allow to access intrinsics parameters
//for optical tracker
//it's used also for 
inline osgART::CameraConfiguration*
DummyTracker::getOrCreateCameraConfiguration()
{
	//you have two choices here

	//1. you create a specific Camera Configuration class, that can hold
	//more parameters for your tracker
	if (!_cameraconfiguration.valid()) _cameraconfiguration = new DummyCameraConfiguration;

	//2.you only create a default Camera Configuration
	//and you setup the proj matrix
	//_cameraconfiguration=new osgART::CameraConfiguration();
	//_cameraconfiguration._prjection..etc 
	return osgART::Tracker::getOrCreateCameraConfiguration();
}

inline void
DummyTracker::setImage(osg::Image* image,bool useInternalVideo)
{

	if (!image) {
		osg::notify() << "osgART::DummyTracker::setImage() called with NULL image" << std::endl;
		return;
	}

	osgART::VisualTracker::setImage(image);

	this->getOrCreateCameraConfiguration()->setSize(*image);

	//you can initialize debug image here

	// Initialise debug image to match video image		
	//m_debugimage->allocateImage(image->s(), image->t(), 1, image->getPixelFormat(), image->getDataType());


}

inline osgART::Target*
DummyTracker::addTarget(const std::string& config)
{
	//create a new targets
	DummyTarget* target = new DummyTarget();
	target->init(config);

	//you can also have different type of targets such as
	//multitarget, imagetarget, etc
	//and use contentof the config string to define them

	//add it to the list
	_targetlist.push_back(target);

	return target;
}

inline bool
DummyTracker::update(osg::NodeVisitor* nv)
{
	const osg::FrameStamp* framestamp = (nv) ? nv->getFrameStamp() : 0L;

	//check first if we get a new image to update
	//the tracking value
	if (_imagesource->valid() && 
		_imagesource->getModifiedCount() != _modifiedCount && 
		_imagesource->data() != NULL) {

		osg::notify(osg::DEBUG_INFO) << "osgART::DummyTracker::update()" << std::endl;

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

        for (osgART::Tracker::TargetList::iterator iter = _targetlist.begin();
            iter != _targetlist.end();
            iter++)
        {

             if (DummyTarget* target = dynamic_cast<DummyTarget*>((*iter).get())) {

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

              }
		}
		//update the counter of the processed image
		_modifiedCount = _imagesource->getModifiedCount();
	}
	return true;
}


// initializer for dynamic loading
osgART::PluginProxy<DummyTracker> g_dummytracker("osgart_tracker_dummytracker");


