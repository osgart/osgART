/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include "osgART/PluginManager"
#include "osgART/Utils"

#include "ARToolKitTrainingSupport"
#include "ARToolKitTracker"


#include "ARToolKitVideo"



#include <osg/Image>
#include <osg/Timer>

#include <osgDB/FileUtils>

#include <iostream>

// initializer for dynamic loading
osgART::PluginProxy<osgART::ARToolKitTracker> g_artoolkittracker("osgart_tracker_artoolkit2");

	   
const int		osgART::ARToolKitTracker::ARTOOLKIT_DEFAULT_THRESHOLD = 100;
const float		osgART::ARToolKitTracker::ARTOOLKIT_DEFAULT_NEAR_PLANE = 10.0f;
const float		osgART::ARToolKitTracker::ARTOOLKIT_DEFAULT_FAR_PLANE = 5000.0f;


// Utility for serialization of Fields
inline std::ostream& operator << (std::ostream& output, const osg::ref_ptr<osg::Image> img)
{
	if (img.valid())
		output << "Image: " << img->s() << "x" << img->t();
    return output;     // to enable cascading
}



#include <AR/config.h>
//#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the osgART ARToolKit tracker.
#endif

#include "SingleTarget"
#include "MultiTarget"

#include <osgART/Video>
#include <osgART/Calibration>

#include <osg/Notify>

#include <iostream>
#include <fstream>



// Make sure that required OpenGL constant definitions are available at compile-time.
// N.B. These should not be used unless the renderer indicates (at run-time) that it supports them.
// Define constants for extensions (not yet core).
#ifndef GL_APPLE_ycbcr_422
#  define GL_YCBCR_422_APPLE				0x85B9
#  define GL_UNSIGNED_SHORT_8_8_APPLE		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_APPLE	0x85BB
#endif
#ifndef GL_EXT_abgr
#  define GL_ABGR_EXT						0x8000
#endif
#ifndef GL_MESA_ycbcr_texture
#  define GL_YCBCR_MESA						0x8757
#  define GL_UNSIGNED_SHORT_8_8_MESA		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_MESA	0x85BB
#endif




namespace osgART {


	class ARToolKitCalibration : public Calibration
	{
		ARParam cparam;
		ARParam wparam;

	public:
		
		inline bool load(const std::string& filename)
		{

			std::string actualFileName = osgDB::findDataFile(filename);

			if(arParamLoad((char*)actualFileName.c_str(), 1, &wparam) < 0) 
			{
				osg::notify(osg::FATAL) << "osgART::ARToolKitCalibration::init : Error: Can't load camera parameters from '"<<
						filename <<"'." << std::endl;
			
				return false;
			}

			return true;			
		}

		inline void setSize(int width, int height)
		{
			GLdouble temp[16];

			arParamChangeSize(&wparam, width, height, &cparam);

	    	arInitCparam(&cparam);

			arglCameraFrustumRH(&cparam, 
				ARToolKitTracker::ARTOOLKIT_DEFAULT_NEAR_PLANE, ARToolKitTracker::ARTOOLKIT_DEFAULT_FAR_PLANE, 
				temp );

			arParamDisp(&cparam);

			_projection.set(&temp[0]);
			_distortion.set(cparam.dist_factor[0],cparam.dist_factor[1],cparam.dist_factor[2],cparam.dist_factor[3]);
		}


		virtual void undistort(double x, double y, double* u_x, double* u_y) {

			double d[4] = {_distortion[0], _distortion[1], _distortion[2], _distortion[3]};

			observedToIdeal(d, x, y, u_x, u_y);
		}


	};
	


	struct ARToolKitTracker::CameraParameter 
	{
		ARParam cparam;	
	};

	ARToolKitTracker::ARToolKitTracker() : Tracker(),
		m_debugimage(new osg::Image),
		m_threshold(ARTOOLKIT_DEFAULT_THRESHOLD),
		m_target_num(0),
		m_cparam(new CameraParameter)
	{
		
		// Assign version and name of the tracker.
		_name = "ARToolKit";
		char *version = NULL;
		arGetVersion(&version);
		if (version) {
			_version = version;
			free(version);
		} else {
			osg::notify() << "ARToolKitTracker: Could not get version number from ARToolKit" << std::endl;
		}
		
		/*
		// create a new field 
		_fields["threshold"] = new CallbackField<ARToolKitTracker,int>(this,
			&ARToolKitTracker::getThreshold,
			&ARToolKitTracker::setThreshold);

		// create a field for the debug image
		_fields["debug_image"] = new TypedField<osg::ref_ptr<osg::Image> >(&m_debugimage);
		
		// attach a new field to the name "debug"
		_fields["debug"] = new CallbackField<ARToolKitTracker,bool>
			(this,
			&ARToolKitTracker::getDebugMode,
			&ARToolKitTracker::setDebugMode);

		// for statistics
		_fields["targetcount"] = new TypedField<int>(&m_target_num);

		_fields["use_history"] = new TypedField<bool>(&m_useHistory);
		*/
		// Training support
		mTrainer = new ARToolKitTrainingSupport(this);

	}

	inline ARToolKitTracker::~ARToolKitTracker()
	{
		osg::notify() << "ARToolKitTracker::~ARToolKitTracker()" << std::endl;
		
		try 
		{
			delete this->m_cparam;
		}
		catch (...)
		{
			osg::notify() << "ARToolKitTracker::~ARToolKitTracker() D'tor failed to delete" << std::endl;
		}
	}


	TrackerTrainingSupport* ARToolKitTracker::getTrainingSupport() { 
		return mTrainer.get(); 
	}


	inline Calibration* ARToolKitTracker::getOrCreateCalibration() 
	{
		if (!_calibration.valid()) _calibration = new ARToolKitCalibration;

		return Tracker::getOrCreateCalibration();
	}

	inline void ARToolKitTracker::setImage(osg::Image* image)
	{
		Tracker::setImage(image);

		arFittingMode = AR_FITTING_TO_IDEAL;
	    arImageProcMode = AR_IMAGE_PROC_IN_FULL;

		if (image) {
			
			this->getOrCreateCalibration()->setSize(*image);
	
			// Initialise debug image to match video image		
			m_debugimage->allocateImage(image->s(), image->t(), 1, image->getPixelFormat(), image->getDataType());
		}

	}


	bool ARToolKitTracker::setupTargets(const std::string& patternListFile)
	{

		// Need to check whether the passed file even exists
		if (!osgDB::fileExists(patternListFile)) {
			osg::notify(osg::WARN) << "ARToolKitTracker: Can not find pattern file '" << patternListFile << "'" << std::endl;
			return false;
		}

		// Open the specified file
		std::ifstream targetFile;
		targetFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!targetFile.is_open()) 
		{
			osg::notify(osg::WARN) << "ARToolKitTracker: Can not load Pattern file '" << patternListFile << "'" << std::endl;
			return true;
		}

		bool ret = true;
		int patternNum = 0;
		targetFile >> patternNum;
		
		osg::notify() << "ARToolKitTracker::setupTargets() Loading '" << patternNum << "' patterns." << std::endl;

		std::string patternName, patternType;

		// Need EOF checking in here... atm it assumes there are really as many targets as the number says

		for (int i = 0; (i < patternNum) && (!targetFile.eof()); i++)
		{
			// jcl64: Get the whole line for the target file (will handle spaces in filename)
			patternName = "";
			while (trim(patternName) == "" && !targetFile.eof()) {
				getline(targetFile, patternName);
			}
			
			
			// Check whether targetFile exists?

			targetFile >> patternType;

			if (patternType == "SINGLE")
			{

				osg::notify(osg::WARN) << "Loading single pattern: '" << patternName << "'." << std::endl;
				
				double width, center[2];
				targetFile >> width >> center[0] >> center[1];
				if (addSingleTarget(patternName, width, center) == -1) {
					osg::notify(osg::WARN) << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			}
			else if (patternType == "MULTI")
			{
				if (addMultiTarget(patternName) == -1) {
					osg::notify(osg::WARN) << "Error adding multi-target pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			} 
			else 
			{
				osg::notify(osg::WARN) << "Unrecognized pattern type: " << patternType << std::endl;
				
				continue;
			}
		}

		targetFile.close();

		return ret;
	}

	int 
	ARToolKitTracker::addSingleTarget(const std::string& pattFile, double width, double center[2]) {

		SingleTarget* singleTarget = new SingleTarget();

		if (!singleTarget->initialise(pattFile, width, center))
		{
			singleTarget->unref();
			return -1;
		}

		_targetlist.push_back(singleTarget);

		// Return the index of the target just added
		return _targetlist.size() - 1;
	}

	int 
	ARToolKitTracker::addMultiTarget(const std::string& multiFile) 
	{
		MultiTarget* multiTarget = new MultiTarget();
		
		if (!multiTarget->initialise(multiFile))
		{
			multiTarget->unref();
			return -1;
		}

		_targetlist.push_back(multiTarget);

		// Return the index of the target just added
		return _targetlist.size() - 1;

	}

	/*virtual*/
	Target* ARToolKitTracker::addTarget(const std::string& config)
	{
		/* format is 
		
		single;data/pattern.dat;80;0;0 
		multi;data/multifile.dat

		*/
		std::vector<std::string> _tokens = tokenize(config,";");

		if (_tokens.size() < 2) 
		{
			osg::notify(osg::WARN) << "Invalid configuration string (too few tokens)" << std::endl;

			return 0L;
		}
		
		if (_tokens[0] == "single")
		{
			osg::notify(osg::INFO) << "Loading type:'" << _tokens[0] << "' Target" << std::endl;

			if (_tokens.size() < 5)
			{
				osg::notify(osg::WARN) << "Invalid configuration string" << std::endl;
				return 0L;
			}

			double _center[2];
			double _size = atof(_tokens[2].c_str());			 
			_center[0] = atof(_tokens[3].c_str());
			_center[1] = atof(_tokens[4].c_str());

			SingleTarget* singleTarget = new SingleTarget();
            
			if (!singleTarget->initialise(_tokens[1], _size, _center))
			{
				singleTarget->unref();
				return 0L;
			}

			osg::notify(osg::INFO) << "Added Target: '" << _tokens[1] << "'" << std::endl;

			_targetlist.push_back(singleTarget);

			return singleTarget;

		}

		if (_tokens[0] == "multi")
		{
			MultiTarget* multiTarget = new MultiTarget();
	
			if (!multiTarget->initialise(_tokens[1]))
			{
				multiTarget->unref();
				return 0L;
			}

			_targetlist.push_back(multiTarget);

			return multiTarget;
		}

		return 0L;
	}


	/*virtual*/
	void ARToolKitTracker::removeTarget(Target* target)
	{
		if (!target) return;

		std::vector< osg::ref_ptr<osgART::Target> >::iterator i = std::find(_targetlist.begin(), _targetlist.end(), target);

		if (i != _targetlist.end()){
			std::string n = target->getName();
			*i = 0L;
			_targetlist.erase(i);
			osg::notify(osg::INFO) << "Removed target: " << n << std::endl;
		}
	}


	inline void ARToolKitTracker::setThreshold(const int& thresh)	
	{
		m_threshold = osg::clampBetween<int>(thresh, 0, 255);		
	}

	inline int ARToolKitTracker::getThreshold() const 
	{
		return m_threshold;
	}

	inline void ARToolKitTracker::setDebugMode(const bool& b)
	{
		arDebug = (int)b;
	}
	
	inline bool ARToolKitTracker::getDebugMode() const
	{
		return (arDebug == 1);
	}
	
	inline void ARToolKitTracker::update()
	{
	}


    Tracker::Traits ARToolKitTracker::getTraits()
    {
        return NoTraits;
    }

    Video* ARToolKitTracker::createVideo() {
        return new ARToolKitVideo();
    }


    inline void ARToolKitTracker::updateCallback(osg::NodeVisitor* nv)
	{

		const osg::FrameStamp* framestamp = (nv) ? nv->getFrameStamp() : 0L;

		

		// Pointer to array holding the details of detected targets.
		ARMarkerInfo *target_info;

	    register int j, k;

		if (!_imagesource.valid())
		{
			osg::notify(osg::WARN) << "ARToolKitTracker: No connected image source for the tracker" << std::endl;
			return;
		}

		// Do not update with a null image.
		if (!_imagesource->valid())
		{
			osg::notify(osg::WARN) << "ARToolKitTracker: received NULL pointer as image" << std::endl;
			return;
		}

		// hse25: performance measurement: only update if the image was modified
		if (_imagesource->getModifiedCount() == _modifiedCount)
		{
			return; 
		}
		
		// update internal modified count
		_modifiedCount = _imagesource->getModifiedCount();

		// \TODO: hse25: check here for the moment, the function needs to be extended
		if (AR_DEFAULT_PIXEL_FORMAT != getARPixelFormatForImage(*_imagesource.get()))
		{
			osg::notify(osg::WARN) << "ARToolKitTracker::update() Incompatible pixelformat!" << std::endl;
			return;
		}
	
		// lock agains video updates
		Video* video = dynamic_cast<Video*>(_imagesource.get());		
		if (video)
		{
			video->getMutex().lock();
		}

		// hse25: above is unsafe! Use below. 
		//OpenThreads::ScopedLock<OpenThreads::Mutex> lock


		osg::Timer t;

		t.setStartTick();

		// Detect the targets in the video frame.
		if (arDetectMarker(_imagesource->data(), m_threshold, &target_info, &m_target_num) < 0) 
		{
			osg::notify(osg::FATAL) << "ARToolKitTracker: Error detecting targets in image." << std::endl;
			// TODO: unlock the mutex for a graceful shutdown
			return;
		}

		if (framestamp && _stats.valid())
		{
			_stats->setAttribute(framestamp->getFrameNumber(),
				"arDetectTarget time taken", t.time_m());

			_stats->setAttribute(framestamp->getFrameNumber(),
				"Possible candidates", m_target_num);
		}

		// Do training if enabled
		if (mTrainer.valid() && mTrainer->isEnabled()) {
			mTrainer->processTargets(target_info, m_target_num);
		}

		// Debug Image
		if (arDebug && arImage && m_debugimage.valid()) {

			m_debugimage->setImage(
				_imagesource->s(), _imagesource->t(), 1, 
				_imagesource->getInternalTextureFormat(), 
				_imagesource->getPixelFormat(), 
				_imagesource->getDataType(), 
				arImage, 
				osg::Image::NO_DELETE, 1);

		}

		TargetList::iterator _end = _targetlist.end();
	

		// Check through the target_info array for highest confidence
		// visible target matching our preferred pattern.
		for (TargetList::iterator iter = _targetlist.begin(); iter != _end; iter++)		
		{

			std::string tag = std::string("Target update ");

			if (SingleTarget* singleTarget = dynamic_cast<SingleTarget*>((*iter).get()))
			{			



				k = -1;
				for (j = 0; j < m_target_num; j++)	
				{
					if (singleTarget->getPatternID() == target_info[j].id) 
					{
						if (k == -1) k = j; // First target detected.
						else if (target_info[j].cf > target_info[k].cf) k = j; // Higher confidence target detected.
					}
				}

				t.setStartTick();
					
				if(k != -1) 
				{

					singleTarget->update(&target_info[k], m_useHistory); 
				} 
				else 
				{
					singleTarget->update(NULL);
				}

				if (framestamp && _stats.valid())
				{
					_stats->setAttribute(framestamp->getFrameNumber(),
						tag + "'" + singleTarget->getName() + "'" + " pose time taken", t.time_m());
				}

			}
			else if (MultiTarget* multiTarget = dynamic_cast<MultiTarget*>((*iter).get()))
			{
				t.setStartTick();

				multiTarget->update(target_info, m_target_num);

				if (framestamp && _stats.valid())
				{
					_stats->setAttribute(framestamp->getFrameNumber(),
						tag + "'" + multiTarget->getName() + "'" + " pose time taken", t.time_m());
				}
				
			} else {
				
				osg::notify(osg::WARN) << "ARToolKitTracker::update() : Unknown target type id!" << std::endl;

				continue;
			}
		}

		if (video)
		{
			video->getMutex().unlock();
		}

	}

	//inline void ARToolKitTracker::setProjection(const double n, const double f) 
	//{
	//	arglCameraFrustumRH(&(m_cparam->cparam), n, f, _calibration._projection);
	//}

	/*inline void ARToolKitTracker::createUndistortedMesh(
		int width, int height,
		float maxU, float maxV,
		osg::Geometry &geometry)
	{

		osg::Vec3Array *coords = dynamic_cast<osg::Vec3Array*>(geometry.getVertexArray());
		osg::Vec2Array* tcoords = dynamic_cast<osg::Vec2Array*>(geometry.getTexCoordArray(0));
						
		unsigned int rows = 20, cols = 20;
		float rowSize = height / (float)rows;
		float colSize = width / (float)cols;
		double x, y, px, py, u, v;

		for (unsigned int r = 0; r < rows; r++) {
			for (unsigned int c = 0; c <= cols; c++) {

				x = c * colSize;
				y = r * rowSize;

				Observer2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - (r / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

				x = c * colSize;
				y = (r+1) * rowSize;

				Observer2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - ((r+1) / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

			}

			geometry.addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
				r * 2 * (cols+1), 2 * (cols+1)));
		}
	}*/

	inline int ARToolKitTracker::getARPixelFormatForImage(const osg::Image& _image) const
	{
		int format = 0, size = 0;
		
		if (_image.valid()) {
			switch (_image.getPixelFormat()) {
				case GL_RGBA:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_RGBA;
						size = 4;
					}
					break;
				case GL_ABGR_EXT:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_ABGR;
						size = 4;
					}
					break;
				case GL_BGRA:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_BGRA;
						size = 4;
					}
	#ifdef AR_BIG_ENDIAN
					else if (_image.getDataType() == GL_UNSIGNED_INT_8_8_8_8_REV) {
						format = AR_PIXEL_FORMAT_ARGB;
						size = 4;
					}
	#else
					else if (_image.getDataType() == GL_UNSIGNED_INT_8_8_8_8) {
						format = AR_PIXEL_FORMAT_ARGB;
						size = 4;
					}
	#endif
					break;
				case GL_RGB:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_RGB;
						size = 3;
					}
					break;
				case GL_BGR:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_BGR;
						size = 3;
					}
					break;
				case GL_YCBCR_422_APPLE:
				case GL_YCBCR_MESA:
	#ifdef AR_BIG_ENDIAN
					if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_REV_APPLE) {
						format = AR_PIXEL_FORMAT_2vuy; // N.B.: GL_UNSIGNED_SHORT_8_8_REV_APPLE = GL_UNSIGNED_SHORT_8_8_REV_MESA
						size = 2;
					} else if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_APPLE) {
						format = AR_PIXEL_FORMAT_yuvs; // GL_UNSIGNED_SHORT_8_8_APPLE = GL_UNSIGNED_SHORT_8_8_MESA
						size = 2;
					}
	#else
					if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_APPLE) {
						format = AR_PIXEL_FORMAT_2vuy;
						size = 2;
					} else if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_REV_APPLE) {
						format = AR_PIXEL_FORMAT_yuvs;
						size = 2;
					}
	#endif
					break;
				case GL_LUMINANCE:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = AR_PIXEL_FORMAT_MONO;
						size = 1;
					}
					break;
				default:
					break;
			}
		}
		return (format);
	}

	inline int ARToolKitTracker::getGLPixelFormatForARPixelFormat(const int arPixelFormat, GLenum *internalformat_GL, GLenum *format_GL, GLenum *type_GL) const
	{
		// Translate the internal pixelformat to an OpenGL texture2D triplet.
		switch (arPixelFormat) {
			case AR_PIXEL_FORMAT_RGB:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_RGB;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_BGR:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_BGR;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_RGBA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_RGBA;
				*type_GL = GL_UNSIGNED_BYTE;
			case AR_PIXEL_FORMAT_BGRA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_ARGB:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
				*type_GL = GL_UNSIGNED_INT_8_8_8_8;
#endif
				break;
			case AR_PIXEL_FORMAT_ABGR:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_ABGR_EXT;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_MONO:
				*internalformat_GL = GL_LUMINANCE8;
				*format_GL = GL_LUMINANCE;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case AR_PIXEL_FORMAT_2vuy:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
#endif
				break;
			case AR_PIXEL_FORMAT_yuvs:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
#endif
				break;
			default:
				return (-1);
				break;
		}
		return (0);
	}
	
}; // namespace osgART
