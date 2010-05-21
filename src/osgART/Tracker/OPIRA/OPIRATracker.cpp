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

OPIRACalibration::OPIRACalibration() : Calibration(),
	mCapture(new Capture()) 
{

}

OPIRACalibration::~OPIRACalibration() 
{
	if (mCapture) delete mCapture;
}

Capture* OPIRACalibration::getCapture() 
{
	return mCapture;
}


inline bool OPIRACalibration::load(const std::string& filename) 
{
	mCalibFilename = osgDB::findDataFile(filename);
	return mCapture->loadCaptureParams((char*)mCalibFilename.c_str());
}

inline void OPIRACalibration::setSize(int width, int height) 
{

	// Load capture parameters and scale them to match new image size
	if (!mCapture->loadCaptureParams((char*)mCalibFilename.c_str())) {//, true, width, height)) {
		// Error loading capture parameters
		return;
	}

	CvMat* parameters = mCapture->getParameters();
	CvMat* distortion = mCapture->getDistortion();

	double* projMat = OPIRALibrary::calcProjection(parameters, distortion, cvSize(width, height), 10.0f, 10000.0f);
	_projection = osg::Matrix(projMat);
	free(projMat);

	_distortion.set(distortion->data.db[0], distortion->data.db[1], distortion->data.db[2], distortion->data.db[3]);

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
	mFrame(NULL) 
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
				mRegistration = new OPIRALibrary::RegistrationStandard(std::vector<std::string>(), featureDetector);

			} 
			else if (_registrationPolicy == "optical_flow") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using optical flow registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOpticalFlow(std::vector<std::string>(), featureDetector);

			} 
			else if (_registrationPolicy == "opira") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using OPIRA registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOPIRA(std::vector<std::string>(), featureDetector);

			} 
			else if (_registrationPolicy == "opira_mt") 
			{

				osg::notify(osg::WARN) << "OPIRATracker: Using OPIRA multithreaded registration" << std::endl;
				mRegistration = new OPIRALibrary::RegistrationOPIRAMT(std::vector<std::string>(), featureDetector);
				//mRegistration = new OPIRALibrary::RegistrationOPIRAMT("MagicLand.bmp", featureDetector);

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
		this->getOrCreateCalibration()->setSize(*image);

		if (mFrame) cvReleaseImage(&mFrame);
		mFrame = cvCreateImage(cvSize(image->s(), image->t()), IPL_DEPTH_8U, 3);

	}
}

Marker* OPIRATracker::addMarker(const std::string& config) 
{
	
	OPIRAMarker* marker = new OPIRAMarker(config);
	_markerlist.push_back(marker);

	if (OPIRALibrary::Registration* r = getOrCreateRegistration()) 
	{
		r->addMarker(config);
		osg::notify(osg::WARN) << "OPIRATracker: Added Marker: '" << marker->getName() << "'" << std::endl;
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

			osg::Timer t;
			t.setStartTick();

			// Convert image into RGB/BGR (3 components) rather than BGRA (4 components)
			for (int y = 0; y < _imagesource->t(); y++) 
			{
				for (int x = 0; x < _imagesource->s(); x++) 
				{
					int i = y * _imagesource->s() + x;
					mFrame->imageData[i * 3 + 0] = _imagesource->data()[i * 4 + 0];
					mFrame->imageData[i * 3 + 1] = _imagesource->data()[i * 4 + 1];
					mFrame->imageData[i * 3 + 2] = _imagesource->data()[i * 4 + 2];
				}
			}

			// Release mutex
			//if (video) video->getMutex().unlock();

			//osg::notify(osg::WARN) << "OPIRATracker: image update done" << std::endl;


			}

		
		

		if (OPIRACalibration* calib = dynamic_cast<OPIRACalibration*>(this->getOrCreateCalibration())) 
		{
		
			CvMat* captureParams = calib->getCapture()->getParameters();
			CvMat* captureDistortion = calib->getCapture()->getDistortion();
			
			if (captureParams && captureDistortion) 
			{
				detectedMarkerTransforms = getOrCreateRegistration()->performRegistration(mFrame, captureParams, captureDistortion);
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
