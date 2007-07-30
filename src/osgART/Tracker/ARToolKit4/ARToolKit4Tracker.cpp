/*
 *	osgART/Tracker/ARToolKit4/ARToolKit4Tracker
 *	osgART: AR ToolKit for OpenSceneGraph
 *
 *	Copyright (c) 2005-2007 ARToolworks, Inc. All rights reserved.
 *	
 *	Rev		Date		Who		Changes
 *  1.0   	2006-12-08  ---     Version 1.0 release.
 *
 */
// @@OSGART_LICENSE_HEADER_BEGIN@@
// @@OSGART_LICENSE_HEADER_END@@

#include "ARToolKit4Tracker"

#include <osgART/PluginManager>

// initializer for dynamic loading
osgART::PluginProxy<osgART::ARToolKit4Tracker> g_artoolkit4("tracker_artoolkit4");

#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#ifndef AR_HAVE_HEADER_VERSION_4_1
#error ARToolKit v4.1 or later is required to build the OSGART ARToolKit4 tracker.
#endif

#include "SingleMarker"
#include "MultiMarker"

#include <osgART/GenericVideo>
#include <osg/Notify>

#include <iostream>
#include <fstream>
//#include <stdio.h>

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

using namespace std;

namespace osgART {

	struct ARToolKit4Tracker::CameraParameter 
	{
		ARParam cparam;	
	};

	ARToolKit4Tracker::ARToolKit4Tracker() : GenericTracker(),
		m_debugimage(new osg::Image),
		gARPattHandle(NULL),
		gARHandle(NULL),
		gAR3DHandle(NULL),
		m_cparam(new CameraParameter)
	{
		// Assign version and name of the tracker.
		m_name		= "ARToolKit4";
		char *version;
		arGetVersion(&version);
		if (version) {
			m_version = version;
			free(version);
		}
		
		// create a field for the debug image
		m_fields["debug_image"] = new TypedField<osg::ref_ptr<osg::Image> >(&m_debugimage);
		
				m_fields["patternDetectionMode"]	= new CallbackField<ARToolKit4Tracker,int>(this,
																		   &ARToolKit4Tracker::getPatternDetectionMode,
																		   &ARToolKit4Tracker::setPatternDetectionMode);




	}

	ARToolKit4Tracker::~ARToolKit4Tracker()
	{
		osg::notify() << "ARToolKitTracker::~ARToolKitTracker()"
			<< std::endl;
		if (gARPattHandle) {
			arPattDetach(gARHandle);
			arPattDeleteHandle(gARPattHandle);
		}
		ar3DDeleteHandle(gAR3DHandle);
		arDeleteHandle(gARHandle);
		try {

			delete this->m_cparam;
		}
		catch (...)
		{
			osg::notify() << "ARToolKitTracker::~ARToolKitTracker() D'tor failed to delete"
				<< std::endl;
		}
	}

	bool ARToolKit4Tracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{
		osg::notify() << "ARToolKit4Tracker::init()..." << endl;
		ARParam  wparam;
		
	    // Load the camera parameters, resize for the window and init.
		cparamName = camera_name;
		if (arParamLoad(cparamName.c_str(), 1, &wparam) < 0) {
			osg::notify(osg::FATAL) << "osgART::ARToolKit4Tracker::init : Error: Can't load camera parameters from '"<<
			camera_name <<"'." << std::endl;
			return false;
	    }
		arParamChangeSize(&wparam, xsize, ysize, &(m_cparam->cparam));
		std::cout << "*** Camera Parameter ***" << std::endl;
	    arParamDisp(&(m_cparam->cparam));

		if ((gARHandle = arCreateHandle(&(m_cparam->cparam))) == NULL) {
			osg::notify(osg::FATAL) << "osgART::ARToolKit4Tracker::init : Error: Could not create ARHandle." << endl;
			return (false);
		}
		setProjection(10.0f, 8000.0f);

		m_fields["debug"]		= new CallbackField<ARToolKit4Tracker,bool>(this,
																			&ARToolKit4Tracker::getDebugMode,
																			&ARToolKit4Tracker::setDebugMode);
		setDebugMode(AR_DEFAULT_DEBUG_MODE);
		
		
		m_fields["imageProcMode"]		= new CallbackField<ARToolKit4Tracker,int>(this,
																			&ARToolKit4Tracker::getImageProcMode,
																			&ARToolKit4Tracker::setImageProcMode);
		setImageProcMode(AR_DEFAULT_IMAGE_PROC_MODE);

		m_fields["threshold"]	= new CallbackField<ARToolKit4Tracker,int>(this,
																		   &ARToolKit4Tracker::getThreshold,
																		   &ARToolKit4Tracker::setThreshold);
		setThreshold(AR_DEFAULT_LABELING_THRESH);
		

		setPatternDetectionMode(2);


		if ((gAR3DHandle = ar3DCreateHandle(&(m_cparam->cparam))) == NULL) {
			osg::notify(osg::FATAL) << "osgART::ARToolKit4Tracker::init : Error: Could not create AR3DHandle." << endl;
			return (false);
		}

		if (!setupMarkers(pattlist_name)) {
			osg::notify(osg::FATAL) << "osgART::ARToolKit4Tracker::init : Error: Marker setup failed." << std::endl;
			return (false);
		}
		
		// Success
		return true;
	}

	std::string trim(std::string& s,const std::string& drop = " ")
	{
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}

	bool ARToolKit4Tracker::setupMarkers(const std::string& patternListFile)
	{
		if ((gARPattHandle = arPattCreateHandle()) == NULL) {
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::setupMarkers() : Error: Could not create ARPattHandle." << endl;
			return (false);
		}

		// attach a new field to the name "markercount"
		m_fields["markercount"] = new TypedField<int>(&gARHandle->marker_num);
		
		arPattAttach(gARHandle, gARPattHandle);
		
		
		std::ifstream markerFile;

		// Need to check whether the passed file even exists
		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open()) return false;

		bool ret = true;

		int patternNum = 0;
		markerFile >> patternNum;
		//std::cout << "Loading " << patternNum << " patterns." << std::endl;

		std::string patternName, patternType;

		// Need EOF checking in here... atm it assumes there are really as many markers as the number says

		for (int i = 0; (i < patternNum) && (!markerFile.eof()); i++)
		{
			// jcl64: Get the whole line for the marker file (will handle spaces in filename)
			patternName = "";
			while (trim(patternName) == "" && !markerFile.eof()) {
				getline(markerFile, patternName);
			}			
			
			// Check whether markerFile exists?

			markerFile >> patternType;

			if (patternType == "SINGLE")
			{
				
				double width, center[2];
				markerFile >> width >> center[0] >> center[1];
				if (addSingleMarker(patternName, width, center) == -1) {
					std::cerr << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			}
			else if (patternType == "MULTI")
			{
				if (addMultiMarker(patternName) == -1) {
					std::cerr << "Error adding multi-marker pattern: " << patternName << std::endl;
					ret = false;
					break;
				}

			} 
			else 
			{
				std::cerr << "Unrecognized pattern type: " << patternType << std::endl;
				continue;
			}

			if(i >= AR_PATT_NUM_MAX)
			{
				std::cerr << "Maximum number of pattern reached(" << AR_PATT_NUM_MAX<< "), please change the value AR_PATT_NUM_MAX in 'AR\arConfig.h' header file." << std::endl;
				break;
			}
		}

		markerFile.close();

		return ret;
	}

	int ARToolKit4Tracker::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		std::cout << "Adding new single marker..." << pattFile << std::endl;
		Marker* singleMarker = new SingleMarker(gARPattHandle);
		
		if (!singleMarker)
		{
			osg::notify(osg::WARN) <<  "ARToolKit4Tracker::addSingleMarker() : Could not create singleMarker!" << endl;
			return -1;
		}
		else if (!gARHandle)
		{
			osg::notify(osg::FATAL) <<  "ARToolKit4Tracker::addSingleMarker() : gARHandle is empty, please run init() function before!" <<  endl;
			exit(-1);
		}

		if (!static_cast<SingleMarker*>(singleMarker)->initialise(gARHandle, pattFile, width, center))
		{
			singleMarker->unref();
			osg::notify(osg::WARN) <<  "ARToolKit4Tracker::addSingleMarker() : Could not init singleMarker!" << endl;
			return -1;
		}		
		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;
	}

	int ARToolKit4Tracker::addMultiMarker(const std::string& multiFile) 
	{
		MultiMarker* multiMarker = new MultiMarker();//gARPattHandle);
		if (!multiMarker->initialise(multiFile))
		{
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	void ARToolKit4Tracker::setThreshold(const int& thresh)	{
		if (arSetLabelingThresh(gARHandle, osg::clampBetween(thresh,0,255)) < 0) {
			osg::notify(osg::WARN) << "ARToolKit4Tracker::setThreshold() : error in arSetLabelingThresh()" << endl;
		}
	}

	int ARToolKit4Tracker::getThreshold() const
	{
		return gARHandle->arLabelingThresh;
	}

	void ARToolKit4Tracker::setDebugMode(const bool &d) 
	{
		if (arSetDebugMode(gARHandle, d) < 0) {
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::setDebugMode() : error in arSetDebugMode()" << endl;
		}
	}

	bool ARToolKit4Tracker::getDebugMode()const 
	{
		return arGetDebugMode(gARHandle);
	}

	void ARToolKit4Tracker::setImageProcMode(const int &mode) 
	{
		if (arSetImageProcMode(gARHandle, mode) < 0) {
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::setImageProcMode() : error in arSetImageProcMode()" << endl;
		}
	}

	int ARToolKit4Tracker::getImageProcMode()const 
	{
		return arGetImageProcMode(gARHandle);
	}

	void ARToolKit4Tracker::setPatternDetectionMode(const int &mode) 
	{
		if (arSetPatternDetectionMode(gARHandle, mode) < 0) {
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::setPatternDetectionMode() : error in arSetPatternDetectionMode()" << endl;
		}
	}

	int ARToolKit4Tracker::getPatternDetectionMode()const
	{
		return arGetPatternDetectionMode(gARHandle);
	}

	void ARToolKit4Tracker::update()
	{		
		if (arGetDebugMode(gARHandle)) osg::notify() << endl << "Start->" << getLabel() << "::update()" << endl;

	    int             j, k;

		if (!m_imagesource.valid())
		{
			osg::notify(osg::WARN) << "No connected image source for the tracker" << std::endl;
			return;
		}

		// Do not update with a null image.
		if (!m_imagesource->valid())
		{
			osg::notify(osg::WARN) << "osgart_artoolkit_tracker: received NULL pointer as image"
				<< std::endl;
			return;
		}

		// hse25: performance measurement: only update if the image was modified
		if (m_imagesource->getModifiedCount() == m_lastModifiedCount)
		{
			return; 
		}
		
		// update internal modified count
		m_lastModifiedCount = m_imagesource->getModifiedCount();
		
		arSetPixelFormat(gARHandle, getARPixelFormatForImage(*m_imagesource.get()));

		// Detect the markers in the video frame.
		if (arDetectMarker(gARHandle, m_imagesource->data()) < 0) {
			osg::notify(osg::FATAL) << "Error detecting markers in image." << std::endl;
			return;
		}
	
		if (arGetDebugMode(gARHandle)) osg::notify(osg::NOTICE) << "	arDetectMarker() => Markerdetected = " << gARHandle->marker_num << std::endl;

		// Debug image.
		if (arGetDebugMode(gARHandle)) {
			GLenum internalformat_GL;
			GLenum format_GL;
			GLenum type_GL;
			getGLPixelFormatForARPixelFormat(arGetPixelFormat(gARHandle), &internalformat_GL, &format_GL, &type_GL);
			
			// If no debugimage, or if the ARToolKit pixel format has changed, then (re)allocate.
			if (!m_debugimage->valid() ||
				m_debugimage->getPixelFormat() != format_GL ||
				m_debugimage->getDataType() != type_GL) {
				osg::notify(osg::NOTICE) << "ARToolKit4Tracker::init() Create Debug Image: " << m_cparam->cparam.xsize << " x " << m_cparam->cparam.ysize  << std::endl;
				m_debugimage->allocateImage(m_cparam->cparam.xsize, m_cparam->cparam.ysize, 1, format_GL, type_GL, 1);
			}
			
			m_debugimage->setImage(m_debugimage->s(), m_debugimage->t(), 
								   1, internalformat_GL, format_GL, type_GL, gARHandle->labelInfo.bwImage, 
								   osg::Image::NO_DELETE, 1);
		}
		
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter != m_markerlist.end(); 
			iter++)		
		{
			Marker* currentMarker = (*iter).get();
		//	cout << "	processing marker : "<< currentMarker->getName();

			if (currentMarker->getType() == Marker::ART_SINGLE)
			{
				SingleMarker* singleMarker = static_cast<SingleMarker*>(currentMarker);
				//cout << ". ID " << singleMarker->getPatternID() <<endl;
				k = -1;

				for (j = 0; j < gARHandle->marker_num; j++)	
				{
			//		cout << "	try matching candidate pattern :" << gARHandle->markerInfo[j].id<<endl;
					if (singleMarker->getPatternID() == gARHandle->markerInfo[j].id) {
						if (k == -1) k = j; // First marker detected.
						else if (gARHandle->markerInfo[j].cf > gARHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
					}
				}
					
				if (k != -1) {			
			//		cout << "		candidate " << k<< " match  "<< currentMarker->getName() <<endl;
					singleMarker->update(&gARHandle->markerInfo[k], gAR3DHandle);
				} else {
					singleMarker->update(NULL, NULL);
				}
			}
			else if (currentMarker->getType() == Marker::ART_MULTI)
			{
				static_cast<MultiMarker*>(currentMarker)->update(this->gAR3DHandle, gARHandle->markerInfo, gARHandle->marker_num);
			}
		}
		
		if (arGetDebugMode(gARHandle)) osg::notify() << "<-Stop" << getLabel() << "::update()" << endl;
	}

	void ARToolKit4Tracker::setProjection(const double n, const double f) 
	{
		arglCameraFrustumRH(&(m_cparam->cparam), n, f, m_projectionMatrix);
	}
	
	void ARToolKit4Tracker::createUndistortedMesh(
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

				arParamObserv2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py, m_cparam->cparam.dist_function_version);
				coords->push_back(osg::Vec3(px, py, 0.0f));
				
				u = (c / (float)cols) * maxU;
				v = (1.0f - (r / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));
				
				x = c * colSize;
				y = (r+1) * rowSize;
				
				arParamObserv2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py, m_cparam->cparam.dist_function_version);
				coords->push_back(osg::Vec3(px, py, 0.0f));
				
				u = (c / (float)cols) * maxU;
				v = (1.0f - ((r+1) / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));
				
			}
			
			geometry.addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
														 r * 2 * (cols+1), 2 * (cols+1)));
		}
	}

	int ARToolKit4Tracker::getARPixelFormatForImage(const osg::Image& _image) const
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

	int ARToolKit4Tracker::getGLPixelFormatForARPixelFormat(const int arPixelFormat, GLenum *internalformat_GL, GLenum *format_GL, GLenum *type_GL) const
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
