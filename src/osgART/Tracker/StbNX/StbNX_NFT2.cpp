/* -*-c++-*- 
 * 
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 * 
 * This file is part of osgART 2.0
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
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>


#include "osgART/Marker"
#include "osgART/Tracker"
#include "osgART/Video"
#include "osgART/Calibration"
#include "osgART/Utils"
#include "osgART/PluginManager"

#include <StbCore/System.h>
#include <StbCore/Image.h>
#include <StbCore/Logger.h>

#include <StbCore/InFile.h>
#include <StbCore/OutFile.h>
#include <StbCore/FileSystem.h>
#include <StbCore/Video.h>
#include <StbCore/Logger.h>
#include <StbCore/StringTool.h>
#include <StbIO/VideoImage.h>
#include <StbCV/Image/Image.h>
#include <StbCV/Tool/Render.h>
#include <StbIO/ImageTool.h>
#include <StbCV/Polygon.h>
#include <NFT2/NFT2_Tracker.h>
#include <StbCV/Camera.h>

#include <sstream>
#include <algorithm>

#include <StbTracker/Base/System.h>
#include <StbTracker/TrackerMain.h>


class StbLogger : public StbTracker::Logger {

protected:
	StbTracker::Logger::TYPE	level;
	StbCore::Logger*			logger;

public:
	StbLogger(StbCore::Logger* nLogger, StbTracker::Logger::TYPE nLevel) : logger(nLogger), level(nLevel) {}

	StbTracker::Logger::TYPE getLevel() {
		return level;
	}

	void StbTracker_Log(StbTracker::Logger::TYPE nType, const char* nStr) {
		switch(nType) {
			case StbTracker::Logger::TYPE_ERROR:
				OSG_WARN << nStr << std::endl;
				return;
			default:
				OSG_NOTICE << nStr << std::endl;
				return;
		}
	}
};



class StbNFT2Calibration : public osgART::Calibration {

private:
	StbCV::Camera* _camera;
public:

	inline StbNFT2Calibration() 
	: osgART::Calibration()
	, _camera(new StbCV::Camera()) { }
	
	inline ~StbNFT2Calibration() 
	{}

	StbCV::Camera* 
	getStbCamera() 
	{ 
		return _camera;
	}

	inline bool 
	load(const std::string& filename)
	{
		if(!_camera->load(filename.c_str()))
		{
			osg::notify(osg::FATAL) << "CalibrationStb: Error loading camera calibration from " << filename << std::endl;
			return false;
		}

		osg::notify() << "CalibrationStb: Loaded camera calibration from " << filename << std::endl;

		// TODO: configurable near and far clipping planes		
		StbMath::Matrix44F pMatrix;
		_camera->getProjectionMatrix(0.01f,1000.0f,pMatrix);
		_projection.set(pMatrix.get());
		
		OSG_INFO<<"Projection Matrix "<< _projection << std::endl;
		
		return true;
	}

	void 
	setSize(int width, int height) 
	{
		// change to correct size
		if (_camera) _camera->changeFrameSize(StbMath::Vec2I(width, height));
	}


};


class StbNFT2Target : public osgART::Marker {
protected:

	StbCV::NFT2::Target& _target;
	
	virtual ~StbNFT2Target() { }

public:

	StbNFT2Target(StbCV::NFT2::Target& target) 
	: osgART::Marker()
	, _target(target) { }

	StbCV::NFT2::Target& 
	getStbTarget() 
	{
		return _target;
	}

	void update(osgART::Tracker& tracker) 
	{
		
		if (_valid = (_target.getStatus() != StbCV::NFT2::Target::INACTIVE) == true)
		{
			// check if we should query found from target
			StbMath::Matrix34F mat34;
			StbMath::Matrix44F mat44;
			makeIdent(mat44);

			_target.getPose().toMatrix(mat34);
			StbMath::putSlice(mat44, 0,0, mat34);

			updateTransform(osg::Matrix(mat44.get()));
			
			OSG_NOTICE << "Valid: " << _valid << "\n Pose:\n" << osg::Matrix(mat44.get()) << std::endl;
		}
	}
};


class StbNFT2 : public osgART::Tracker {
protected:

//	StbTracker::TrackerMain* _trackerMain;
	StbCV::NFT2::NFTracker2* _tracker;
	StbCV::Image* _image;

public:

	StbNFT2();
	virtual ~StbNFT2();

	void setImage(osg::Image* image);
	
	osgART::Calibration* getOrCreateCalibration();

	osgART::Marker* addMarker(const std::string& config);

	void update(osg::NodeVisitor* nv);

};

StbNFT2::StbNFT2() 
: osgART::Tracker()
//, _trackerMain(StbTracker::TrackerMain::create(new StbLogger(StbCore::Logger::getInstance(), StbTracker::Logger::TYPE_ERROR)))
, _tracker(new StbCV::NFT2::NFTracker2())
, _image(new StbCV::Image())
{
	_tracker->setMode(StbCV::NFT2::NFTracker2::MODE_SINGLE);
}

StbNFT2::~StbNFT2() 
{
}

void 
StbNFT2::setImage(osg::Image* image) 
{
	if (image)
	{
		osgART::Tracker::setImage(image);
		this->getOrCreateCalibration()->setSize(*image);
		
		// locally cached image
		_image->allocPixels(image->s(),image->t(),StbCore::PIXEL_FORMAT_LUM);
		
	}
}

osgART::Calibration* 
StbNFT2::getOrCreateCalibration() 
{
	if (!_calibration.valid()) _calibration = new StbNFT2Calibration();
	return osgART::Tracker::getOrCreateCalibration();
}

osgART::Marker* 
StbNFT2::addMarker(const std::string& config) 
{
	
	std::vector<std::string> tokens = osgART::tokenize(config, ",");

	if (tokens.size() < 2) 
	{
		osg::notify(osg::FATAL) << "StbNFT2: Invalid marker configuration string" << std::endl;
		return 0L;
	}
	
	// use the tag
	std::string markerType = tokens[0];

	//single,soccer/soccerSet,soccer,hyper_FCBarcelona
	//single,multiset,multiset,target_vienna3
		
	OSG_INFO << "NFT2 config string '" << config << "'" << std::endl;

	if (markerType == "single") 
	{
		StbCore::MemoryInFile memoryFile;

		// add target path
		StbCore::StringW path(tokens[2].c_str()), fullPath;
        if(path.length()>0 && path[path.length()-1]!='/' && path[path.length()-1]!='\\')
            path += "/";
        path += "virtual_config.txt";
        StbCore::FileSystem::getInstance()->extendToAbsolutePath(path, fullPath);
		
		// should map to the target path
		memoryFile.setFileName("virtual_config.txt", fullPath);
		
		std::stringstream item;
		item << "database = " << tokens[1] << std::endl;
		item << "target" << _markerlist.size() <<"-name = " << tokens[3] << std::endl;

		memoryFile.addData(item.str().c_str(), item.str().size());
		
		OSG_INFO << "virtual config file " <<  "'" << item.str() << "'" << std::endl;
		
		int oldIndex = _tracker->getNumTargets();
		
		if (_tracker->loadTargets(&memoryFile))
		{
			OSG_INFO << "new target at index " << oldIndex << std::endl;  
		
			_markerlist.push_back( new StbNFT2Target(_tracker->getTarget(oldIndex)) );
			
			return (_markerlist.back());
		}
		
	}
	
	return 0L;

}

void 
StbNFT2::update(osg::NodeVisitor* nv) 
{
	// Update the image
	if (!_imagesource.valid()) return;
	
	// Assign the camera to the tracker if it isn't already set
	if (StbNFT2Calibration* calib = static_cast<StbNFT2Calibration*>(this->getOrCreateCalibration())) {

		if (calib->getStbCamera()) 
		{
			this->getOrCreateCalibration()->setSize(_imagesource->s(),_imagesource->t());
			_tracker->setCamera(calib->getStbCamera());
			
		} else {
			osg::notify() << "StbNFT2: No camera calibration file was loaded" << std::endl;	
			return;
		}
	}

	// use image format to compute components (assume we always use BGR(A) or grayscale)
	switch (osg::Image::computeNumComponents(_imagesource->getPixelFormat()))
	{
		case 1:
			_image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_LUM);
			break;
		case 3:
			// this actually should be BGR2LUm ...
			StbIO::ImageTool::convertImageRGB888toLum(_imagesource->data(),_image->getPixels(),_imagesource->s()*_imagesource->t());
			break;
		// \TODO implement other colorspace models
//		case 4:
//			StbIO::convertImageRGB888toLum(_imagesource->data(),_image->getPixels(),_imagesource->s()*_imagesource->t());
//			_image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_BGRA);
			break;
		default:
			OSG_NOTICE<<"Warning: unsupported colorspace! "<< osg::Image::computeNumComponents(_imagesource->getPixelFormat()) << std::endl;
			break;
	}
	
	_tracker->update(*_image);
	

	// Process the found targets

	for (MarkerList::iterator iter = _markerlist.begin(); iter != _markerlist.end(); iter++) 
	{
		(*iter)->update(*this);
	}
}

// initializer for dynamic loading
osgART::PluginProxy<StbNFT2> g_stbtracker_nft2("osgart_tracker_stbnx_nft2");

