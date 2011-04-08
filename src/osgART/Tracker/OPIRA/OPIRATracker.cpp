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


#include <osgART/PluginManager>
#include <osgART/Utils>
#include <osgART/Video>
#include <osgART/Marker>
#include <osgART/Calibration>

#include <osg/Notify>
#include <osg/Image>
#include <osg/Timer>
#include <osgDB/FileUtils>

#include <iostream>
#include <fstream>

#include "OPIRATracker"

// initializer for dynamic loading
osgART::PluginProxy<osgART::OPIRATracker> g_opiratracker("tracker_opira");

// Utility for serialization of Fields
inline std::ostream& operator << (std::ostream& output, const osg::ref_ptr<osg::Image> img)
{
	if (img.valid())
		output << "Image: " << img->s() << "x" << img->t();
    return output;     // to enable cascading
}



namespace osgART {


// --------------------------------------------------------------
// OPIRA Marker
// --------------------------------------------------------------

OPIRAMarker::OPIRAMarker(std::string filename) : Marker() 
{
	setName(filename);
	setActive(true);
}


bool OPIRAMarker::isSameKindAs(const osg::Object* obj) const 
{
    return dynamic_cast<const OPIRAMarker*>(obj) != 0L;
}

const char* OPIRAMarker::className() const 
{ 
	return "OPIRAMarker"; 
}

Marker::MarkerType OPIRAMarker::getType() const 
{ 
	return ART_NFT_OPIRA; 
}

void OPIRAMarker::update(OPIRALibrary::MarkerTransform* mt) 
{

	if (mt == NULL) {
		
		_valid = false;

	} else {

		this->_confidence = mt->score;

		if (mt->score < 10) {

			_valid = false;

		} else {

			_valid = true;
			this->updateTransform(osg::Matrix::rotate(osg::PI, osg::X_AXIS) * osg::Matrix(mt->transMat));
		
		}
		
	}

}

OPIRAMarker::~OPIRAMarker() 
{  
}

// --------------------------------------------------------------
// OPIRA Calibration
// --------------------------------------------------------------

OPIRACalibration::OPIRACalibration() : Calibration()
{
	calibParams=0; calibDistortion=0;
}

OPIRACalibration::~OPIRACalibration() 
{
	if (calibParams!=0) cvReleaseMat(&calibParams);
	if (calibDistortion!=0) cvReleaseMat(&calibDistortion);
}


inline bool OPIRACalibration::load(const std::string& filename) 
{
	mCalibFilename = osgDB::findDataFile(filename);

	CvFileStorage* fs = cvOpenFileStorage( mCalibFilename.c_str(), 0, CV_STORAGE_READ );
	if (fs==0) return false; 

	CvFileNode* fileparams;
	//Read the Image Width
	fileparams = cvGetFileNodeByName( fs, NULL, "image_width" );
	calibWidth = cvReadInt(fileparams,-1);
	//Read the Image Height
	fileparams = cvGetFileNodeByName( fs, NULL, "image_height" );
	calibHeight = cvReadInt(fileparams,-1);
	//Read the Camera Parameters
	fileparams = cvGetFileNodeByName( fs, NULL, "camera_matrix" );
	calibParams = (CvMat*)cvRead( fs, fileparams );
	principalX = calibParams->data.db[2]; principalY = calibParams->data.db[5];
	//Read the Camera Distortion 
	fileparams = cvGetFileNodeByName( fs, NULL, "distortion_coefficients" );
	calibDistortion = (CvMat*)cvRead( fs, fileparams );
	cvReleaseFileStorage( &fs );

	/*
	//Initialize Undistortion Maps
	mDistortX = cvCreateMat(captureHeight, captureWidth, CV_32F);
	mDistortY = cvCreateMat(captureHeight, captureWidth, CV_32F);
	cvInitUndistortMap(captureParams, captureDistortion, mDistortX, mDistortY);
	*/

	return true;
}

CvMat* OPIRACalibration::getParameters() { 
	return calibParams;
}

CvMat* OPIRACalibration::getDistortion() {
	return calibDistortion;
}

inline void OPIRACalibration::setSize(int width, int height) 
{
	//Change Principal Point
	calibParams->data.db[2] = width/2.0;
	calibParams->data.db[5] = height/2.0; 

	//Change Scale Factor
	calibParams->data.db[0] *= float(width)/float(calibWidth);
	calibParams->data.db[4] *= float(height)/float(calibHeight);

	double* projMat = OPIRALibrary::calcProjection(getParameters(), getDistortion(), cvSize(width, height), 10.0f, 10000.0f);
	_projection = osg::Matrix(projMat);
	free(projMat);

	_distortion.set(getDistortion()->data.db[0], getDistortion()->data.db[1], getDistortion()->data.db[2], getDistortion()->data.db[3]);

}

void OPIRACalibration::undistort(double x, double y, double* u_x, double* u_y) 
{
	double d[4] = {_distortion[0], _distortion[1], _distortion[2], _distortion[3]};
	observedToIdeal(d, x, y, u_x, u_y, 3);
}
	

// --------------------------------------------------------------
// OPIRA Tracker
// --------------------------------------------------------------

OPIRATracker::OPIRATracker() : Tracker(),
	mRegistration(NULL),
	mFrame(NULL),
	curWidth(0), curHeight(0)
{

	osg::notify(osg::WARN) << "OPIRATracker: Constructor" << std::endl;
	
	_name = "OPIRA";
	_version = "1.0";
	
	_registrationPolicy = "opira";
	_fields["registration_policy"] = new TypedField<std::string>(&_registrationPolicy);

	_featureDetectorType = "OCVSURF";
	_fields["feature_detector"] = new TypedField<std::string>(&_featureDetectorType);

}

inline OPIRATracker::~OPIRATracker() 
{
	
	osg::notify(osg::WARN) << "OPIRATracker: Destructor" << std::endl;

	if (mRegistration) delete mRegistration;

	if (mFrame) cvReleaseImage(&mFrame);

}


TrainingSupport* OPIRATracker::getTrainingSupport() 
{ 
	// Currently there is no training support in the OPIRA plugin
	return NULL; 
}
	
OPIRALibrary::Registration* OPIRATracker::getRegistration() {
	return mRegistration;
}


OPIRALibrary::Registration* OPIRATracker::getOrCreateRegistration() 
{

	if (mRegistration == NULL) 
	{

		OPIRALibrary::RegistrationAlgorithm* featureDetector = NULL;

		if (_featureDetectorType == "OCVSURF") featureDetector = new OCVSurf();
		//else if (_featureDetectorType == "SIFT") featureDetector = new SIFT();
		//else if (_featureDetectorType == "SURF") featureDetector = new SURF();
		else 
		{
			osg::notify(osg::WARN) << "OPIRATracker: Unknown Feature Detector: " << _featureDetectorType << std::endl;
			featureDetector = NULL;
		}

		if (featureDetector == NULL) 
		{
		
			// Error: no feature detector
			osg::notify(osg::WARN) << "OPIRATracker: Cannot create Registration, there is no Feature Detector." << std::endl;
		
		} 
		else 
		{

			// Created Feature Detector, now create Registration
			osg::notify(osg::WARN) << "OPIRATracker: Using Feature Detector: " << _featureDetectorType << std::endl;


			if (_registrationPolicy == "standard") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using standard registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationStandard(featureDetector);

			} 
			else if (_registrationPolicy == "optical_flow") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using optical flow registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOpticalFlow(featureDetector);

			} 
			else if (_registrationPolicy == "opira") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using OPIRA registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOPIRA(featureDetector);

			} 
			else if (_registrationPolicy == "opira_mt") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using OPIRA multithreaded registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOPIRAMT(featureDetector);
				//mRegistration = new OPIRALibrary::RegistrationOPIRAMT(featureDetector);
			} 
			else 
			{
				osg::notify(osg::WARN) << "OPIRATracker: Cannot create Registration, unknown type: " << _registrationPolicy << std::endl;			
				mRegistration = NULL;
			}

		}

		// Configure registration if it has been created
		if (mRegistration) 
		{
			mRegistration->displayImage = false;
		}

	}

	return mRegistration;

}


inline Calibration* OPIRATracker::getOrCreateCalibration()
{
	if (!_calibration.valid()) _calibration = new OPIRACalibration;
	return Tracker::getOrCreateCalibration();
}

inline void OPIRATracker::setImage(osg::Image* image) 
{
	
	Tracker::setImage(image);

	if (image) 
	{
		if (curWidth != image->s() || curHeight !=image->t()) {
			curWidth = image->s(); curHeight = image->t();
			this->getOrCreateCalibration()->setSize(curWidth, curHeight);

			if (mFrame) cvReleaseImage(&mFrame);
			mFrame = cvCreateImage(cvSize(image->s(), image->t()), IPL_DEPTH_8U, 3);

		}

		// if the input image is too big, resize the image to to improve tracking speed
		float aspect = ((float)curWidth/(float)curHeight);
		if (curWidth > 320 && aspect < 1.4f) {
			// image is 4:3 and is too big - scale to 800x600
			curWidth = 320; curHeight = 240;
			this->getOrCreateCalibration()->setSize(curWidth, curHeight);

			if (mFrame) cvReleaseImage(&mFrame);
			mFrame = cvCreateImage(cvSize(curWidth, curHeight), IPL_DEPTH_8U, 3);
		} else if (curWidth > 368 && aspect > 1.65f) {
			// image is 16:9 and is too big - scale to 928x522
			curWidth = 368; curHeight = 207;
			this->getOrCreateCalibration()->setSize(curWidth, curHeight);

			if (mFrame) cvReleaseImage(&mFrame);
			mFrame = cvCreateImage(cvSize(curWidth, curHeight), IPL_DEPTH_8U, 3);
		} else if (curWidth > 360 && aspect > 1.55f && aspect < 1.65f) {
			// image is 16:10 and is too big - scale to 848x565
			curWidth = 360; curHeight = 225;
			this->getOrCreateCalibration()->setSize(curWidth, curHeight);

			if (mFrame) cvReleaseImage(&mFrame);
			mFrame = cvCreateImage(cvSize(curWidth, curHeight), IPL_DEPTH_8U, 3);
		} 
	}
}

Marker* OPIRATracker::addMarker(const std::string& config) 
{
	// format is: "path/to/marker.jpg" or "path/to/marker.jpg;maxLengthSize;maxLengthScale" where maxLengthSize
	// is the integer number of pixels that the marker image's longest edge will be scaled to while maintaining
	// the aspect ratio, and maxLengthScale is the length of the longest side of the physical marker in mm
	std::vector<std::string> _tokens = tokenize(config,";");

	if (_tokens.size() < 1 || _tokens.size() > 3) 
		{
			osg::notify(osg::WARN) << "Invalid configuration string" << std::endl;

			return 0L;
		}
	

	OPIRAMarker* marker = new OPIRAMarker(_tokens[0]);
	_markerlist.push_back(marker);


	if (OPIRALibrary::Registration* r = getOrCreateRegistration()) 
	{

		if (_tokens.size() == 1) {
			r->addMarker(_tokens[0]);
		} else if (_tokens.size() == 2) {
			r->addResizedMarker(_tokens[0], atoi(_tokens[1].c_str()));
		} else if (_tokens.size() == 3) {
			r->addResizedScaledMarker(_tokens[0], atoi(_tokens[1].c_str()), atoi(_tokens[2].c_str()));
		}
		osg::notify(osg::WARN) << "OPIRATracker: Added Marker: '" << _tokens[0] << "'" << std::endl;
	}

	return marker;

}


/*virtual*/
void OPIRATracker::removeMarker(Marker* marker)
{
	if (!marker) return;

	std::vector< osg::ref_ptr<osgART::Marker> >::iterator i = std::find(_markerlist.begin(), _markerlist.end(), marker);

	if (i != _markerlist.end())
	{
		std::string n = marker->getName();

		if (OPIRALibrary::Registration* r = getOrCreateRegistration()) 
		{
			if (!r->removeMarker(n)) 
			{
				// Could not remove marker
			}
		}

		*i = 0L;
		_markerlist.erase(i);
		
		osg::notify(osg::WARN) << "OPIRATracker: Removed marker: " << n << std::endl;

	}
}




	
inline void OPIRATracker::update(osg::NodeVisitor* nv) 
{

	// Ensure there is an OPIRA registration algorithm available
	if (!getOrCreateRegistration()) 
	{
		osg::notify(osg::WARN) << "OPIRATracker: No registration algorithm available. Cannot update tracking" << std::endl;
		return;
	}


		const osg::FrameStamp* framestamp = (nv) ? nv->getFrameStamp() : 0L;
	
		if (!_imagesource.valid()) 
		{
			osg::notify(osg::WARN) << "OPIRATracker: No connected image source for the tracker" << std::endl;
			return;
		}

		// Do not update with a null image.
		if (!_imagesource->valid()) 
		{
			osg::notify(osg::WARN) << "OPIRATracker: received NULL pointer as image" << std::endl;
			return;
		}

		// hse25: performance measurement: only update if the image was modified
		if (_imagesource->getModifiedCount() == m_lastModifiedCount) return; 
		
		// update internal modified count
		m_lastModifiedCount = _imagesource->getModifiedCount();


std::vector<OPIRALibrary::MarkerTransform> detectedMarkerTransforms;

	
		if (Video* video = dynamic_cast<Video*>(_imagesource.get())) {
			
			//osg::notify(osg::WARN) << "OPIRATracker: image update started" << std::endl;

			//if (video) video->getMutex().lock();

			// hse25: above is unsafe! Use below. 
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(video->getMutex());


			// ADDED BY GILES

			//osg::ref_ptr<osg::Image> myImage = (osg::Image*)(_imagesource.get()->clone(osg::CopyOp::SHALLOW_COPY));
			//osg::Image* myImage = (osg::Image*)(_imagesource.get()->clone(osg::CopyOp::DEEP_COPY_ALL));

			/**
			if (myImage) {
				// if the input image is too big, resize the image to to improve tracking speed
				float aspect = ((float)myImage->s()/(float)myImage->t());
				if (myImage->s() > 800 && aspect < 1.4f) {
					// image is 4:3 and it's too big - scale to 800x600
					//image = (osg::Image*)image->clone(osg::CopyOp::DEEP_COPY_ALL);
					myImage->scaleImage(800, 600, myImage->r());
				} else if (myImage->s() > 928 && aspect > 1.65f) {
					// image is is 16:9 and is too big - scale to 928x522
					//image = (osg::Image*)image->clone(osg::CopyOp::DEEP_COPY_ALL);
					myImage->scaleImage(928, 522, myImage->r());
				} else if (myImage->s() > 848 && aspect > 1.55f && aspect < 1.65f) {
					// image is 16:10 and is too big - scale to 848x565
					//image = (osg::Image*)image->clone(osg::CopyOp::DEEP_COPY_ALL);
					myImage->scaleImage(848, 565, myImage->r());
				}
			}
			*/

			// END ADDED BY GILES


			osg::Timer t;
			t.setStartTick();

			// Convert image into RGB/BGR (3 components) rather than BGRA (4 components)
			IplImage *RGBAIm = cvCreateImageHeader(cvSize(_imagesource->s(), _imagesource->t()), IPL_DEPTH_8U, 4);
			RGBAIm->imageData = (char*)_imagesource->data();

			IplImage *RGBIm = cvCreateImage(cvGetSize(RGBAIm), IPL_DEPTH_8U, 3);
			cvCvtColor(RGBAIm, RGBIm, CV_RGBA2RGB);

			cvResize(RGBIm, mFrame);
			//cvCvtColor(RGBAIm, mFrame, CV_RGBA2RGB);
			cvReleaseImage(&RGBIm);
			cvReleaseImageHeader(&RGBAIm);

			// Release mutex
			//if (video) video->getMutex().unlock();

			//osg::notify(osg::WARN) << "OPIRATracker: image update done" << std::endl;


			}

		
		

		if (OPIRACalibration* calib = dynamic_cast<OPIRACalibration*>(this->getOrCreateCalibration())) 
		{
		
			if (calib->getParameters())
			{
				detectedMarkerTransforms = getOrCreateRegistration()->performRegistration(mFrame, calib->getParameters(), calib->getDistortion());
			}

		}

				
		

		
		// Iterator over the markers added to this tracker
		for (MarkerList::iterator iter = _markerlist.begin(); iter != _markerlist.end(); iter++) 
		{

			// Cast to the OPIRAMarker type (should be the only type in the collection)
			if (OPIRAMarker* marker = dynamic_cast<OPIRAMarker*>(iter->get())) 
			{

				// Find the detected transform whose name matches the current OPIRAMarker
				OPIRALibrary::MarkerTransform* matchingTransform = NULL;
				for (std::vector<OPIRALibrary::MarkerTransform>::iterator opiraIter = detectedMarkerTransforms.begin(); opiraIter != detectedMarkerTransforms.end(); opiraIter++) 
				{
					std::string opiraMarkerName = opiraIter->marker.name;
					if (opiraMarkerName == marker->getName()) matchingTransform = &(*opiraIter);
				}

				// Update the marker with matching transform (which could be NULL if none was found, indicating no match/no tracking)
				marker->update(matchingTransform);

			}

		}
		
		// Cleanup
		for (unsigned int i = 0; i< detectedMarkerTransforms.size(); i++) 
		{ 
			detectedMarkerTransforms.at(i).clear();
		}

		detectedMarkerTransforms.clear();


	}

}; // namespace osgART
