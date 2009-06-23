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

	void update( const sstt_calibration* calib );

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
	std::vector<std::string> tokens = osgART::tokenize(config,";");

	int idx(0);

	float width, height;

	for(std::vector<std::string>::iterator i = tokens.begin();
		i != tokens.end();
		i++, idx++)
	{

		switch (idx)
		{
			case 0 : {

				sstt_target_create( &_target, SSTT_TARGET_TEXTURERECTANGLE );

				//osg::notify() 
				std::cout << "SSTT Target texture '" << (*i) << "'" << std::endl;

				/* create a parameter list - it is ok to be on the stack
				 * sstt will copy the parameters as needed
				 */
				const char* target_params[] = { (*i).c_str(), 0L }; 

				sstt_target_init( _target, target_params );

			}
			break;
			
			case 1:
				width = ::atof((*i).c_str());
				break;
			case 2:
				height = ::atof((*i).c_str());
				break;
			case 3:
				/* optional parameter - thresholding the template confidence */
				sstt_target_set_parameter(_target, SSTT_TARGET_CONFIDENCE_THRESHOLD, 
					::atof((*i).c_str()));
				break;
			default:
				break;
		}
	}

	/* update the physical target size */
	sstt_target_set_size(_target, width, height);

	/* set some sane default parameters */
	sstt_target_set_parameter(_target, SSTT_TARGET_KALMANCORNER, 1.0);
	sstt_target_set_parameter(_target, SSTT_TARGET_KALMANPOSE, 0.0);
	sstt_target_set_parameter(_target, SSTT_TARGET_SUBPIXREFINE, 1.0);
	sstt_target_set_parameter(_target, SSTT_TARGET_EQUALIZEHISTOGRAM, 0.0);
	sstt_target_set_parameter(_target, SSTT_TARGET_BOUNDARY, 1.0);

	osg::notify() << "SSTT Target size " << width << " x " << height << std::endl;

}

void 
SSTT_Target::update( const sstt_calibration* calib )
{
	/* we need a info struct to probe into the SSTT API */
	sstt_target_info *target_info(0L);
	
	/* calculate the pose */
	sstt_target_pose( _target, (sstt_calibration*)calib );

	/* get information about the target */
	sstt_target_get_info( _target, &target_info );

	if ( target_info ) 
	{
		/* set the confidence of the marker: normalised 0-1 */
		_confidence = target_info->confidence;

		/* parameterize it with our own confidence threshold */
		m_valid = target_info->confidence > target_info->confidence_threshold;

		//osg::notify() 
		std::cout << "SSTT Target confidence " 
			<< target_info->confidence << "/" 
			<< target_info->confidence_threshold 
			<< std::endl;

		/* set the magic four values */
		_border.set( target_info->border[0], target_info->border[1], target_info->border[2], target_info->border[3] );
		

		/* use the model view to set the marker transformation matrix */
		m_transform.set( &target_info->modelview[0] );

		/* reset the statistical model of SSTT - in the newer SSTT API this might change */
		if ( target_info->confidence > 0.0 ) 
		{
			/* the confidence has a linear degredation of 8% per frame */
			sstt_target_set_parameter(_target, SSTT_TARGET_CONFIDENCE, target_info->confidence - 0.08);
		}

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

	/* create a tracker object */
	sstt_tracker_create( &_tracker );

	/* we use whatever we can get */
	sstt_capture_create( &_capture, SSTT_CAPTURE_EXTERNAL );

	/* yes its 1 - buy a license if your need something else ;) */
	sstt_tracker_set_parameter( _tracker, SSTT_TRACKER_PREPROCESS, 1);
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
	osgART::Tracker::setImage(image);

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

	if (m_imagesource->valid() && 
		m_imagesource->getModifiedCount() != _modifiedCount)
	{

		osg::notify() << "SSTT_Tracker::update()" << std::endl;

		/* we need to inject the osg::Image into the tracking pipeline 
		 * thus we need a temporary image */
		sstt_image image;

		/* set the image data pointer to the source pointer */
		image.data = (char*)m_imagesource->data();

		/* set the dimensions */
		image.dimension[0] = m_imagesource->s();
		image.dimension[1] = m_imagesource->t();
		
		/* this converts the image from the GL version to its internal format 
		 * and attaches it to a capture object - assumption here: its BGR format
		 * with an unaligned buffer
		 */
		sstt_capture_set_image( _capture, &image );

		/* now we can update the tracker from the capture object */
		sstt_tracker_update( _tracker, _capture );

		/* do the detection */
		sstt_tracker_detect( _tracker );

		/* get the calibration */
		SSTT_Calibration* calib = dynamic_cast<SSTT_Calibration*>(this->getOrCreateCalibration());

		/* now sync all osgART markers with their SSTT counterparts */
		for(osgART::Tracker::MarkerList::iterator iter = m_markerlist.begin(); 
			iter != m_markerlist.end(); 
			iter++)		
		{
			SSTT_Target* target = dynamic_cast<SSTT_Target*>((*iter).get());

			if (target && calib)
			{
				target->update( calib->getCalibration() );
			}
		}
		
		_modifiedCount = m_imagesource->getModifiedCount();
	}
}


// initializer for dynamic loading
osgART::PluginProxy<SSTT_Tracker> g_sstttracker("tracker_sstt");


