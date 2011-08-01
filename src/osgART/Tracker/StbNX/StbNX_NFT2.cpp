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
				//logger->logErrorEx("(StbTracker) %s", nStr);
				OSG_WARN << nStr << std::endl;
				return;
			default:
				//logger->logInfoEx("(StbTracker) %s", nStr);
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

	StbCV::NFT2::Target* _target;
	
	virtual ~StbNFT2Target() { }

public:

	StbNFT2Target(StbCV::NFT2::Target* target) 
	: osgART::Marker()
	, _target(target) { }

	inline StbCV::NFT2::Target* getStbTarget() {
		return _target;
	}

	void update(bool found) 
	{
		// check if we should query found from target
		if (false && found) 
		{
			
			StbMath::Matrix34F mat34;
		    StbMath::Matrix44F mat44;
		    makeIdent(mat44);
		
			_target->getPose().toMatrix(mat34);
            StbMath::putSlice(mat44, 0,0, mat34);

			updateTransform(osg::Matrix(mat44.get()));
 
			_valid = true;
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
	}
}

	
osgART::Calibration* 
StbNFT2::getOrCreateCalibration() 
{
	if (!_calibration.valid()) _calibration = new StbNFT2Calibration();
	return osgART::Tracker::getOrCreateCalibration();
}

osgART::Marker* 
StbNFT2::addMarker(const std::string& config) {

	
	// Format: type,...
	
	// Type=ID
	// Format: ID, id, width

	// Type=Frame
	// Format: Frame, id, width

	// Type=DataMatrix
	// Type=Grid
	// Type=Split
	


	std::vector<std::string> tokens = osgART::tokenize(config, ",");

	if (tokens.size() < 2) 
	{
		osg::notify(osg::FATAL) << "StbNFT2: Invalid marker configuration string" << std::endl;
		return 0L;
	}
	
	// use the tag
	std::string markerType = tokens[0];

	// nft2;blah;200
	
	OSG_NOTICE << "NFT2 config string '" << config << "'" << std::endl;

	if (markerType == "single") 
	{
		StbCV::NFT2::Target* target = 0;
		StbCore::MemoryInFile memoryFile;

		std::vector<std::string> target_names;
		std::stringstream item;

		//item << "<Tracker><StbTrackerNFT2 database=\"" << tokens[1] << "\" "
		//	<< "target-path=\"" << tokens[2] << "\" "
		//	<< "<Target name=\"" << tokens[3] << "\"" << " ref=\"" << tokens[4] << "\" />"
		//	<< "</Tracker>";
		
		// single,soccer/soccerSet,soccerSet_0,soccer
		
		item << "database = " << tokens[1] << std::endl;
		item << "target" << _markerlist.size() <<"-name = " << tokens[2] << std::endl;
		//item << "target" << _markerlist.size() <<"-size = " << tokens[2] << std::endl;
		
		// 
		memoryFile.addData(item.str().c_str(), item.str().size());
		
		// should map to the target path
		memoryFile.setFileName("virtual_config.txt", tokens[3].c_str());
		
		OSG_NOTICE << "virtual config file '" << item.str() << "'" << std::endl;

		if (_tracker->loadTargets(&memoryFile))
		{
			StbNFT2Target* osgart_nft_target = new StbNFT2Target(target);
			
			return osgart_nft_target;
		}
		
	}
	
	return 0L;

}

void 
StbNFT2::update(osg::NodeVisitor* nv) 
{
	
	return;
	
	// Assign the camera to the tracker if it isn't already set
	if (StbNFT2Calibration* calib = dynamic_cast<StbNFT2Calibration*>(_calibration.get())) {
		if (calib->getStbCamera()) 
		{
			_tracker->setCamera(calib->getStbCamera());
		} else {
			osg::notify() << "StbNFT2: No camera calibration file was loaded" << std::endl;	
			return;
		}
	}
	
	
	// Update the image
	if (!_imagesource.valid()) return;
	
	// use image format to compute components (assume we always use BGR(A) or grayscale)
	switch (osg::Image::computeNumComponents(_imagesource->getPixelFormat()))
	{
		case 1:
			_image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_LUM);
			break;
		case 3:
			_image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_BGR);
			break;
		case 4:
			_image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_BGRA);
			break;
		default:
		break;
	}
	
	_tracker->update(*_image);
	

	// Process the found targets

#if 0
	StbTracker::TargetVector& targets = targetManager->getVisibleTargets();
	for (MarkerList::iterator iter = _markerlist.begin(); iter != _markerlist.end(); iter++) {
		// The current osgART marker is valid if its target is in the "found targets" vector
		if (MarkerStb* ms = dynamic_cast<MarkerStb*>(iter->get())) {
			bool valid = (std::find(targets.begin(), targets.end(), ms->getMarker()->getTarget()) != targets.end());
			ms->update(valid);
		}
	}
	
#endif
	
}

// initializer for dynamic loading
osgART::PluginProxy<StbNFT2> g_stbtracker_nft2("osgart_tracker_stbnx_nft2");

