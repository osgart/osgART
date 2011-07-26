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

#include "osgART/Marker"
#include "osgART/Tracker"
#include "osgART/Video"
#include "osgART/Calibration"
#include "osgART/Utils"
#include "osgART/PluginManager"

#include <StbCore/Image.h>
#include <StbCore/Logger.h>
#include <StbTracker/TrackerMain.h>
#include <StbTracker/Base/Logger.h>
#include <StbTracker/Base/TargetSingleMarker.h>
#include <StbTracker/Base/MarkerFrameSimpleId.h>

#include <StbTracker/Features/ThresholderAuto.h>
#include <StbTracker/Features/FiducialDetectorRect.h>
#include <StbTracker/Features/MarkerDetectorSimpleIdFixed.h>
#include <StbTracker/Features/MarkerDetectorFrameSimpleIdFixed.h>
#include <StbTracker/Features/MarkerFollowerRect.h>
#include <StbTracker/Features/PoseFilterDESP.h>
#include <StbTracker/Features/PoseEstimatorGNX.h>
#include <StbTracker/Features/TargetManagerDefault.h>
#include <StbTracker/Features/Camera.h>
#include <StbTracker/Util/ImageTool.h>

#include <sstream>
#include <algorithm>

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
				logger->logErrorEx("(StbTracker) %s", nStr);
				return;
			default:
				logger->logInfoEx("(StbTracker) %s", nStr);
				return;
		}
	}
};

class CalibrationStb : public osgART::Calibration {

private:
	StbTracker::Camera* mCamera;

public:

	inline CalibrationStb() : osgART::Calibration(), mCamera(0L) { }
	inline ~CalibrationStb() { }

	inline StbTracker::Camera* getStbCamera() { 
		return mCamera;
	}

	inline bool load(const std::string& filename){

		// TODO: do I need to destroy the camera if it already exists?
		mCamera = StbTracker::Camera::createFromFile(filename.c_str());
		
		if (mCamera == 0L) {
			osg::notify(osg::FATAL) << "CalibrationStb: Error loading camera calibration from " << filename << std::endl;
			return false;
		}

		osg::notify() << "CalibrationStb: Loaded camera calibration from " << filename << std::endl;

		// TODO: configurable near and far clipping planes
		float P[16];
		mCamera->getProjectionMatrix(0.01f,1000.0f,P);
		_projection.set(P);

		return true;
	}

	inline void setSize(int width, int height) {
		if (mCamera) mCamera->changeFrameSize(StbTracker::Vec2F(width, height));
	}

};


class MarkerStb : public osgART::Marker {

protected:
	StbTracker::Marker* mMarker;
	virtual ~MarkerStb() { }

public:

	MarkerStb(StbTracker::Marker* marker) : osgART::Marker(), mMarker(marker) { }

	inline StbTracker::Marker* getMarker() {
		return mMarker;
	}

	void update(bool found) {
		if (found) {
			float T[16];
			mMarker->getTarget()->getGLMatrix(T, StbTracker::Target::TYPE_RAW);
			updateTransform(osg::Matrix(T));
			_valid = true;
		} else {
			_valid = false;
		}
	}

};


class TrackerStb : public osgART::Tracker {

protected:

	StbTracker::TrackerMain* tracker;
	StbCore::Image* image;
	StbTracker::TargetManagerDefault* targetManager;

	void registerDetectors();

public:

	TrackerStb();
	virtual ~TrackerStb();

	void setImage(osg::Image* image);

	osgART::Marker* addMarker(const std::string& config);

	void update(osg::NodeVisitor* nv);

};

TrackerStb::TrackerStb() : osgART::Tracker() {

	// Main tracker object
	tracker = StbTracker::TrackerMain::create(new StbLogger(StbCore::Logger::getInstance(), StbTracker::Logger::TYPE_ERROR));
	tracker->setDumpData(0);

	_calibration = new CalibrationStb();	

	StbTracker::ThresholderAuto* thresholder = StbTracker::ThresholderAuto::create();
	thresholder->setAutoEnabled(true);
	tracker->registerFeature(thresholder);

	// Detects rectangles inside the thresholded image
	tracker->registerFeature(StbTracker::FiducialDetectorRect::create());
	
	// MarkerFollower (it finds it quickly if it's closer to where it was in the previous frame)
	tracker->registerFeature(StbTracker::MarkerFollowerRect::create());

	// PoseEstimator (the 3d position relative to the camera)
	tracker->registerFeature(StbTracker::PoseEstimatorGNX::create());
	
	// TargetManager
	targetManager = StbTracker::TargetManagerDefault::create();
	tracker->registerFeature(targetManager);

	image = StbCore::Image::create();

}

TrackerStb::~TrackerStb() {

}

inline void TrackerStb::setImage(osg::Image* image) {
	osgART::Tracker::setImage(image);
	this->getOrCreateCalibration()->setSize(*image);
}


void TrackerStb::registerDetectors() {

	for (MarkerList::iterator iter = _markerlist.begin(); iter != _markerlist.end(); iter++) {

		MarkerStb* m = dynamic_cast<MarkerStb*>((*iter).get());
		if (!m) continue;

		StbCore::Base::TYPE markerType = m->getMarker()->getType();

		if (markerType == StbTracker::MarkerSimpleId::getClassType()) {
			
			if (tracker->getFeature(StbTracker::MarkerDetectorSimpleIdFixed::getClassType()) == NULL) {
				osg::notify() << "TrackerStb: Adding a SimpleId Marker Detector" << std::endl;
				StbTracker::MarkerDetectorSimpleIdFixed* markerDetector = StbTracker::MarkerDetectorSimpleIdFixed::create();
				markerDetector->setBorderWidth(0.125f);
				markerDetector->setLearnNewMarkers(false);
				tracker->registerFeature(markerDetector);	
			}

		} else if (markerType == StbTracker::MarkerFrameSimpleId::getClassType()) {
			
			if (tracker->getFeature(StbTracker::MarkerDetectorFrameSimpleIdFixed::getClassType()) == NULL) {
				osg::notify() << "TrackerStb: Adding a FrameSimpleId Marker Detector" << std::endl;
				StbTracker::MarkerDetector* markerDetector = StbTracker::MarkerDetectorFrameSimpleIdFixed::create(); // the frame marker detector has only fixed-point implementation
				//markerDetector->setThreshold(threshold);
				markerDetector->setBorderWidth(0.04545f);
				tracker->registerFeature(markerDetector);
			}
		
		} else {

			osg::notify() << "TrackerStb: Unhandled marker type when choosing a MarkerDetector" << std::endl;
			return;

		}

		

	}

}


inline 
osgART::Marker* TrackerStb::addMarker(const std::string& config) {

	// Format: type,...
	
	// Type=ID
	// Format: ID, id, width

	// Type=Frame
	// Format: Frame, id, width

	// Type=DataMatrix
	// Type=Grid
	// Type=Split

	std::vector<std::string> tokens = osgART::tokenize(config, ";");

	if (tokens.size() < 2) 
	{
		osg::notify(osg::FATAL) << "TrackerStb: Invalid marker configuration string" << std::endl;
		return 0L;
	}
	
	std::string markerType = tokens[0];

	if (markerType == "ID") {

		// TODO: Need to check here whether the tracker we have set up is of the SimpleID type...?

		if (tokens.size() == 3) {

			int id = atoi(tokens[1].c_str());
			float size = atof(tokens[2].c_str());

			std::stringstream ss;
			ss << "SimpleID_" << id;
			std::string markerName = ss.str();

			osg::notify() << "TrackerStb: Adding a SimpleID marker, ID=" << id << ", Size=" << size << " as '" << markerName << "'" << std::endl;

			// StbTracker's marker
			StbTracker::Marker* marker = StbTracker::MarkerSimpleId::create(id, size);
			targetManager->registerSingleMarkerTarget(markerName.c_str(), marker);


			// osgART wrapper around StbTracker's marker
			MarkerStb* markerStb = new MarkerStb(marker);
			_markerlist.push_back(markerStb);

			registerDetectors();

			return markerStb;
		}

	} else if (markerType == "Frame") {

		if (tokens.size() == 3) {

			int id = atoi(tokens[1].c_str());
			float size = atof(tokens[2].c_str());

			std::stringstream ss;
			ss << "FrameSimpleID_" << id;
			std::string markerName = ss.str();

			osg::notify() << "TrackerStb: Adding a FrameSimpleID marker, ID=" << id << ", Size=" << size << " as '" << markerName << "'" << std::endl;

			// StbTracker's marker
			StbTracker::Marker* marker = StbTracker::MarkerFrameSimpleId::create(id, size);
			targetManager->registerSingleMarkerTarget(markerName.c_str(), marker);

			// osgART wrapper around StbTracker's marker
			MarkerStb* markerStb = new MarkerStb(marker);
			_markerlist.push_back(markerStb);
			
			registerDetectors();

			return markerStb;
		
		} else {

			osg::notify() << "TrackerStb: Found " << tokens.size() << " tokens in config string, expecting 3" << std::endl;

		}
	} else if (markerType == "NFT2") {
		
		if (tokens.size() == 3)
		{
			StbTracker::
		}
		
		
	} else if (markerType == "DataMatrix") {

	} else if (markerType == "Grid") {

	} else if (markerType == "Split") {

	} else {

		osg::notify() << "TrackerStb: Marker type " << markerType << " not yet supported" << std::endl;

	}

	return 0L;

}


inline void TrackerStb::update(osg::NodeVisitor* nv) {

	// Assign the camera to the tracker if it isn't already set
	if (CalibrationStb* calib = dynamic_cast<CalibrationStb*>(_calibration.get())) {
		if (calib->getStbCamera()) {
			if (tracker->getFeature(StbTracker::Camera::getClassType()) == NULL) {
				tracker->registerFeature(calib->getStbCamera());
			}
		} else {
			osg::notify() << "TrackerStb: No camera calibration file was loaded" << std::endl;	
			return;
		}
	}
	
	
	// Update the image
	if (!_imagesource.valid()) return;
	
	// use image format to compute components (assume we always use BGR(A) or grayscale)
	switch (osg::Image::computeNumComponents(_imagesource->getPixelFormat()))
	{
		case 1:
			image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_LUM);
			break;
		case 3:
			image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_BGR);
			break;
		case 4:
			image->setPixels(_imagesource->data(), _imagesource->s(), _imagesource->t(), StbCore::PIXEL_FORMAT_BGRA);
			break;
		default:
		break;
	}
	
	// Update the tracker
	if (!tracker->update(image)) 
	{
		osg::notify(osg::WARN) << "TrackerStb: Error updating tracker with image" << std::endl;
		return;
	}

	// Process the found targets
	StbTracker::TargetVector& targets = targetManager->getVisibleTargets();
	for (MarkerList::iterator iter = _markerlist.begin(); iter != _markerlist.end(); iter++) {
		// The current osgART marker is valid if its target is in the "found targets" vector
		if (MarkerStb* ms = dynamic_cast<MarkerStb*>(iter->get())) {
			bool valid = (std::find(targets.begin(), targets.end(), ms->getMarker()->getTarget()) != targets.end());
			ms->update(valid);
		}
	}
	
}

// initializer for dynamic loading
osgART::PluginProxy<TrackerStb> g_stbtracker("tracker_stbtracker");


