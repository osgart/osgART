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

#include <sstt/sstt.h>

#include <osg/Notify>

#include "osgART/Marker"
#include "osgART/Tracker"
#include "osgART/Video"
#include "osgART/Calibration"
#include "osgART/Utils"
#include "osgART/PluginManager"

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/io_utils>

class SSTT_Calibration : public osgART::Calibration
{

	sstt_calibration* _calibration;

public:

	inline SSTT_Calibration() : _calibration(0L)
	{
		sstt_calibration_create( &_calibration );
	}

	inline ~SSTT_Calibration()
	{
		sstt_calibration_destroy( &_calibration );
	}

	inline const sstt_calibration* getCalibration() const
	{
		return _calibration;
	}

	inline bool load(const std::string& filename)
	{
		/* 0 = loading default camera parameters  
		 * this is a sane choice for most webcams */
		sstt_calibration_load( _calibration, 0 );

		return true;
	}

	inline void setSize(int width, int height)
	{
		osg::notify() << "SSTT Calibration resize '" << 
			width << " x " << height << std::endl;

		/* set the camera image size */
		sstt_calibration_set_size( _calibration, width, height );

		/* first a temporary copy */
		osg::Matrixf temp_projection;

		/* update the projection matrix */
		sstt_calibration_gl_projection( _calibration, temp_projection.ptr() );

		_projection = temp_projection;

	}

};


class SSTT_Tracker;


class SSTT_Target : public osgART::Marker
{
	sstt_target *_target;

	osg::Vec4f _border;
	
public:

	SSTT_Target();

	void init(const std::string& config);

	void update();

	inline const sstt_target* getTarget() const 
	{
		return _target;
	}

protected:
	virtual ~SSTT_Target() { if ( _target ) sstt_target_destroy( &_target ); }

};


class SSTT_Tracker : public osgART::Tracker
{
	sstt_tracker* _tracker;
	sstt_capture* _capture;
	unsigned int _modifiedCount;
	
public:

	SSTT_Tracker();
	virtual ~SSTT_Tracker();
	
	osgART::Calibration* getOrCreateCalibration();

	void setImage(osg::Image* image);

	osgART::Marker* addMarker(const std::string& config);

	void update(osg::NodeVisitor* nv);

};



/* Implementation */


SSTT_Target::SSTT_Target() 
	: osgART::Marker(), _target(0L)
{
}


void
SSTT_Target::init(const std::string& config)
{

	if (config.empty()) {
		osg::notify() << "SSTT Tracker: Empty config string" << std::endl;
		return;
	}

	std::vector<std::string> tokens = osgART::tokenize(config,";");

	if (tokens.size() < 3) {
		osg::notify() << "SSTT Tracker: Config string does not contain mandatory fields" << std::endl;
	}

	std::string targetImage = tokens[0];
	float width = atof(tokens[1].c_str());
	float height = atof(tokens[2].c_str());

	osg::notify() << "SSTT Tracker: Attempting to use target image " << targetImage << ", " << width << "x" << height << std::endl;

	std::string filename = osgDB::findDataFile(targetImage);
	if (filename.empty()) {
		osg::notify() << "SSTT Tracker: Could not locate target image " << targetImage << std::endl;
		return;
	} else {
		osg::notify() << "SSTT Tracker: Using target image " << targetImage << std::endl;
	}

	osg::Image* inputImage = osgDB::readImageFile(filename);
	if (!inputImage) {
		osg::notify() << "SSTT Tracker: Error loading target image from " << targetImage << std::endl;
		return;
	} else {
		osg::notify() << "SSTT Tracker: Loaded target image from " << targetImage << std::endl;
		osg::notify() << "SSTT Tracker: Target image is " << inputImage->s() << "x" << inputImage->t() << ", " << osg::Image::computeNumComponents(inputImage->getPixelFormat()) << " components"  << std::endl;

	}

	if (sstt_target_create( &_target, SSTT_TARGET_TEXTURERECTANGLE ) != 0) {
		osg::notify() << "SSTT Tracker: Error in sstt_target_create" << std::endl;
		return;
	}
	
	if (sstt_target_init( _target) != 0) {
		osg::notify() << "SSTT Tracker: Error in sstt_target_init" << std::endl;
		return;
	}

	if (sstt_target_set_size(_target, width, height) != 0) {
		osg::notify() << "SSTT Tracker: Error in sstt_target_init" << std::endl;
		return;
	}

	/* set some sane default parameters */
	sstt_target_set_parameter(_target, SSTT_TARGET_KALMANCORNER, 1.0);
	//sstt_target_set_parameter(_target, SSTT_TARGET_KALMANPOSE, 0.0);
	sstt_target_set_parameter(_target, SSTT_TARGET_SUBPIXREFINE, 1.0);
	//sstt_target_set_parameter(_target, SSTT_TARGET_EQUALIZEHISTOGRAM, 0.0);
	//sstt_target_set_parameter(_target, SSTT_TARGET_BOUNDARY, 1.0);

	if (tokens.size() > 3) {
		sstt_target_set_parameter(_target, SSTT_TARGET_CONFIDENCE_THRESHOLD, atof(tokens[3].c_str()));
	}

	sstt_image in_img;
	in_img.data = inputImage->data();
	in_img.width = inputImage->s();
	in_img.height = inputImage->t();
	in_img.channels = 3;
	in_img.stride = inputImage->s() * in_img.channels;
	
	if (sstt_target_set_image(_target, &in_img) != 0) {
		osg::notify() << "SSTT Tracker: Error in sstt_target_set_image" << std::endl;
		return;
	}

}

void 
SSTT_Target::update()
{
	/* we need a info struct to probe into the SSTT API */
	sstt_target_info *target_info(0L);
	sstt_target_get_info( _target, &target_info );

	if ( target_info ) 
	{

		/* set the confidence of the marker: normalised 0-1 */
		m_confidence = target_info->confidence;

		/* parameterize it with our own confidence threshold */
		m_valid = target_info->confidence > target_info->confidence_threshold;

		if (m_valid) {
			std::cout << "Visible" << std::endl;
			
			/* use the model view to set the marker transformation matrix */
			m_transform.set( &target_info->modelview[0] );
			std::cout << m_transform << std::endl;

			/* set the magic four values */
			_border.set( target_info->border[0], target_info->border[1], target_info->border[2], target_info->border[3] );

		} else {
			std::cout << "Not visible" << std::endl;
		}

		//osg::notify() 
		std::cout << "SSTT Target confidence " 
			<< target_info->confidence << "/" 
			<< target_info->confidence_threshold 
			<< std::endl;

		/*
		// reset the statistical model of SSTT - in the newer SSTT API this might change
		if ( target_info->confidence > 0.0 ) 
		{
			// the confidence has a linear degredation of 8% per frame
			sstt_target_set_parameter(_target, SSTT_TARGET_CONFIDENCE, target_info->confidence - 0.08);
		}
		*/

	}

}



SSTT_Tracker::SSTT_Tracker() : 
	osgART::Tracker(), 
	_tracker(0L),
	_capture(0L),
	_modifiedCount(0)
{

	/* initialize the SSTT API - it is save to call this multiple times */
	sstt_init(0,0);

}

SSTT_Tracker::~SSTT_Tracker() 
{
	/* clear up the tracker */
	sstt_tracker_destroy( &_tracker );
}


inline osgART::Calibration* 
SSTT_Tracker::getOrCreateCalibration() 
{
	if (!_calibration.valid()) _calibration = new SSTT_Calibration;

	return osgART::Tracker::getOrCreateCalibration();
}

inline void 
SSTT_Tracker::setImage(osg::Image* image)
{

	if (!image) {
		osg::notify() << "SSTT Tracker: setImage called with NULL image" << std::endl;
		return;
	}

	osgART::Tracker::setImage(image);

	if (_tracker == 0L) {
		/* create a tracker object */
		sstt_tracker_create( &_tracker );
		sstt_capture_create( &_capture, SSTT_CAPTURE_EXTERNAL );
		
		if (sstt_tracker_init(_tracker, image->s(), image->t()) != 0) {
			osg::notify() << "SSTT Tracker: Error in sstt_tracker_init" << std::endl;
		}

		sstt_tracker_set_parameter( _tracker, SSTT_TRACKER_PREPROCESS, 1);

	}

	this->getOrCreateCalibration()->setSize(*image);

}

inline osgART::Marker* 
SSTT_Tracker::addMarker(const std::string& config)
{

	SSTT_Target* target = new SSTT_Target;

	target->init(config);

	m_markerlist.push_back(target);

	/* 
	 * make sure the target is attached to the tracker,
	 * normally SSTT can share targets between multiple trackers
 	 */
	sstt_target_attach( (sstt_target*) target->getTarget(), _tracker );

	return target;
}


inline void 
SSTT_Tracker::update(osg::NodeVisitor* nv)
{
	if (!m_imagesource.valid())
		return;

	if (m_imagesource->valid() && m_imagesource->getModifiedCount() != _modifiedCount && m_imagesource->data() != NULL) {

		osg::notify() << "SSTT_Tracker::update()" << std::endl;

		std::cout << "Image: " << m_imagesource->s() << "x" << m_imagesource->t() << ", " << osg::Image::computeNumComponents(m_imagesource->getPixelFormat()) << std::endl;

		/* we need to inject the osg::Image into the tracking pipeline 
		 * thus we need a temporary image */
		sstt_image image;
		image.width = m_imagesource->s();
		image.height = m_imagesource->t();
		image.stride = m_imagesource->s() * 3;
		image.channels = 3;

		image.data = new unsigned char[m_imagesource->s() * m_imagesource->t() * 3];

		for (int y = 0; y < m_imagesource->t(); y++) {
			for (int x = 0; x < m_imagesource->s(); x++) {
			
				unsigned char* src = m_imagesource->data(x, y);
				unsigned char* dst = &(image.data[(y * m_imagesource->s() + x) * 3]);

				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];

			}
		}

		/* this converts the image from the GL version to its internal format 
		 * and attaches it to a capture object - assumption here: its BGR format
		 * with an unaligned buffer
		 */
		sstt_capture_set_image( _capture, &image );

		


		/* now we can update the tracker from the capture object */
		if (sstt_tracker_update( _tracker, _capture ) != 0) {
			osg::notify() << "SSTT Tracker: Error in sstt_tracker_update" << std::endl;
		}

		if (sstt_tracker_detect( _tracker ) != 0) {
			osg::notify() << "SSTT Tracker: Error in sstt_tracker_detect" << std::endl;
		}
		
		if (SSTT_Calibration* calib = dynamic_cast<SSTT_Calibration*>(this->getOrCreateCalibration())) {

			sstt_tracker_pose( _tracker, (sstt_calibration*)calib->getCalibration() );

			/* now sync all osgART markers with their SSTT counterparts */
			for(osgART::Tracker::MarkerList::iterator iter = m_markerlist.begin(); iter != m_markerlist.end(); iter++) {
				
				if (SSTT_Target* target = dynamic_cast<SSTT_Target*>((*iter).get())) {
						
					std::cout << "target" << rand() << std::endl;
					target->update();

				}

			}

		}
		
		delete[] image.data;
		
		_modifiedCount = m_imagesource->getModifiedCount();
	}
}


// initializer for dynamic loading
osgART::PluginProxy<SSTT_Tracker> g_sstttracker("tracker_sstt");


