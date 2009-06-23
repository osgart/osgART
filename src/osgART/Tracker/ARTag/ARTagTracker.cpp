#include "ARTagTracker"
/*
#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
*/
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif

#include "SingleMarker"
#include "MultiMarker"
#include "osgART/Utils"

#include <osgART/GenericVideo>
#include <osgART/VideoConfig>

#include <iostream>
#include <fstream>


//#include <AR/gsub_lite.h> //add by yannick

//remove it later ????
#define DEFAULT_FOCAL_FX					700//	70 //1150 focal length approx 850-1150 for 640x480 Dragonflies		
#define DEFAULT_FOCAL_FY					700//	70 //about 400 for 320x240 webcam
//=======================

namespace osgART {

	ARTagTracker::ARTagTracker() : 
#if AR_TRACKER_PROFILE
	ARToolKitTrackerProfiler<int>(),
#else
	GenericTracker(),
#endif
			m_debugmode(false)
			//m_cparam(NULL)
			//m_marker_num(0)

	{
		//version and name of the tracker
		m_name		= "ARTag";
		m_version	= "Rev.2.1.1";
		__AR_DO_PROFILE(m_version+="(Prf)");

		//add callback fields
		/*m_fields["threshold"] = new CallbackField<ARTagTracker,int>(this,
			&ARTagTracker::getThreshold,
			&ARTagTracker::setThreshold);
		*/
		m_fields["debugmode"] = new CallbackField<ARTagTracker,bool>(this,
			&ARTagTracker::getDebugMode,
			&ARTagTracker::setDebugMode);

		//add normal fields
		
		//m_fields["markercount"] = new TypedField<int>(&m_marker_num);
	
		//set the default internal format of ARTag
		//see SetImageRaw() for conversion
		m_arInternalFormat	= ConvertARTPixelFormatToOSGART(AR_PIXEL_FORMAT_RGB);

	}

	ARTagTracker::~ARTagTracker()
	{
//		delete m_cparam;
		//arSaveBench((std::string(AR_HEADER_VERSION_STRING) + std::string(".xml")).c_str());
		close_artag();
	}

	bool ARTagTracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{

		m_width = xsize;
		m_height = ysize, 

		//rgb, 3 bytes per pixels??? change with real format..??
		osg::notify() << "Init artag, res : " << xsize << ", " << ysize << std::endl;
		if(init_artag(xsize, ysize, 3))
		{
			osg::notify(osg::FATAL) << "Could not init ARTag!" << std::endl;
			exit(-1);
		}

		//change it with the real camera parameters ???
		float camera_fx,camera_fy,camera_cx,camera_cy;
		camera_fx=DEFAULT_FOCAL_FX;
		camera_fy=DEFAULT_FOCAL_FY;
		camera_cx=(double)xsize/2.0; 
		camera_cy=(double)ysize/2.0;
		//===========================================

		artag_set_camera_params(camera_fx,camera_fy,camera_cx,camera_cy);

		artag_set_switch(ARTAG_SWITCH_USE_FULL_RES, 1);

		osg::notify() << "Camera params: focal fx/fy=" << camera_fx <<
			"/" << camera_fy <<
			" image center cx/cy=" << camera_cx << "/" << camera_cy << std::endl;

		if (!setupMarkers(pattlist_name)) {
			osg::notify(osg::FATAL) << "ERROR: Marker setup failed." << std::endl;
			return false;
		}

		setProjection(10,8000);//..actually do nothing..???

		return true;
	}

	bool ARTagTracker::setupMarkers(const std::string& patternListFile)
	{
		std::ifstream markerFile;

		// Need to check whether the passed file even exists
		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open()) return false;

		bool ret = true;
		int patternNum = 0;
		markerFile >> patternNum;
		std::string patternName, patternType, patternFile;
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
				if (addSingleMarker(patternName) == -1) {//, width, center
					std::cerr << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			}
			else if (patternType == "MULTI")
			{
				patternFile = patternName;
				markerFile >> patternName;
				if (addMultiMarker(patternFile, patternName) == -1) {
					std::cerr << "Error adding multi-marker pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			} 
			else 
			{
				std::cerr << "Unrecognized pattern type: " << patternType << std::endl;
				ret = false;
				break;
			}
		}

		markerFile.close();

		return ret;
	}

	int 
	ARTagTracker::addSingleMarker(const std::string& pattFile)//, double width, double center[2]) 
	{

		SingleMarker* singleMarker = new SingleMarker();
		if (!singleMarker->initialise(pattFile))//, width, center))
		{
			singleMarker->unref();
			return -1;
		}
		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;
	}

	int 
	ARTagTracker::addMultiMarker(const std::string& multiFile, const std::string& multiName) 
	{
		MultiMarker* multiMarker = new MultiMarker();
		
		if (!multiMarker->initialise(multiFile, multiName))
		{
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}
/*
	void ARTagTracker::setThreshold(const int& thresh)	
	{
		m_threshold = osg::clampBetween(thresh,0,255);		
	}

	int ARTagTracker::getThreshold() const 
	{
		return m_threshold;
	}

*/
	unsigned char* ARTagTracker::getDebugImage() {
		return NULL;//arImage;
	}
		
	void ARTagTracker::setDebugMode(const bool &d) 
	{		m_debugmode = d;	}

	bool ARTagTracker::getDebugMode() const
	{		return m_debugmode;	}

	int ARTagTracker::ConvertOSGARTPixelFormatToART(PixelFormatType format)const
	{
		switch (format)
		{
			case VIDEOFORMAT_RGB24: //return AR_PIXEL_FORMAT_RGB;
			case VIDEOFORMAT_BGR24:	//return AR_PIXEL_FORMAT_BGR;
						return AR_PIXEL_FORMAT_RGB;//rgb
			//case VIDEOFORMAT_BGRA32:return AR_PIXEL_FORMAT_BGRA;
			//case VIDEOFORMAT_RGBA32:return AR_PIXEL_FORMAT_RGBA;
			//case VIDEOFORMAT_ARGB32:return AR_PIXEL_FORMAT_ARGB;
			//case VIDEOFORMAT_ABGR32:return AR_PIXEL_FORMAT_ABGR;
			//case VIDEOFORMAT_YUV422:return AR_PIXEL_FORMAT_2vuy;
			case VIDEOFORMAT_Y8:
			case VIDEOFORMAT_GREY8:
									return AR_PIXEL_FORMAT_MONO;
			default:
				osg::notify() << "ConvertOSGARTPixelFormatToART() : Unknown pixel format!" << std::endl;
		}
		return 0;
	}

	PixelFormatType ARTagTracker::ConvertARTPixelFormatToOSGART(int format)const
	{
		switch (format)
		{
			case AR_PIXEL_FORMAT_RGB : return VIDEOFORMAT_RGB24;
			/*case AR_PIXEL_FORMAT_BGR : return VIDEOFORMAT%BGR24;
			case AR_PIXEL_FORMAT_BGRA :return VIDEOFORMAT_BGRA32;
			case AR_PIXEL_FORMAT_RGBA :return VIDEOFORMAT_RGBA32;
			case AR_PIXEL_FORMAT_ARGB :return VIDEOFORMAT_ARGB32;
			case AR_PIXEL_FORMAT_ABGR :return VIDEOFORMAT_ABGR32;
			case AR_PIXEL_FORMAT_2vuy :return VIDEOFORMAT_YUV422;
			*/case AR_PIXEL_FORMAT_MONO :return VIDEOFORMAT_Y8;//or VIDEOFORMAT_GREY8:
			
			default:
				osg::notify() << "ConvertARTPixelFormatToOSGART() : Unknown pixel format!" << std::endl;
		}        
		return VIDEOFORMAT_ANY;
	}

    /*virtual*/
	void ARTagTracker::setImageRaw(unsigned char * image, PixelFormatType format)
    {
		//We are only augmenting method in parent class.
		//m_arInternalFormat	= ConvertARTPixelFormatToOSGART( AR_DEFAULT_PIXEL_FORMAT);

		if (format != m_arInternalFormat)
		{			
			GenericTracker::setImageRaw(
				ConvertImageFormat<unsigned char>(image, m_width, m_height, format, m_arInternalFormat),
				m_arInternalFormat);
		}
		else
			GenericTracker::setImageRaw(image, format);
    }

	void ARTagTracker::update()
	{
		if(m_debugmode)
			osg::notify() << "Start->" << getLabel() << "::update()" << std::endl;

__AR_DO_PROFILE(
		static CL_FUNCT_TRC<CL_TimerVal>	*ThisFct			= this->LocalARTimeTracer->AddFunct		("arDetectMarker_TIME");
);

//		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		int MarkerNum=0;//>????
//	    register int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;

//#if  AR_TRACKER_PROFILE
		AR_BENCH_TIME(ThisFct, 
			// Detect the markers in the video frame.
			artag_find_objects(m_imageptr,ConvertOSGARTPixelFormatToART(m_imageptr_format));//RGB 
			
		, 1 //pattern in memory, change it...???
		, getLabel()
		, MarkerNum//what to put here...????
		);
/*#else
			// Detect the markers in the video frame.
			artag_find_objects(m_imageptr, ConvertOSGARTPixelFormatToART(m_imageptr_format));//RGB
#endif
*/			

		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		int i=0;
		for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter !=  m_markerlist.end(); 
			++iter)		
		{
			if (m_debugmode)
				osg::notify() << std::endl << i++ << std::endl;
            SingleMarker*	singleMarker = dynamic_cast<SingleMarker*>((*iter).get());
			MultiMarker*	multiMarker  = dynamic_cast<MultiMarker*>((*iter).get());

			if (singleMarker)
			{
				if (artag_is_object_found(singleMarker->getPatternID()))
				{
					if (m_debugmode)
						osg::notify() << "Single Marker id : " << singleMarker->getARTagCode() << "found" << std::endl;
					singleMarker->update();
					__AR_DO_PROFILE(
						RecordMarkerStats(singleMarker, true);
						);
				}
			}
			else if (multiMarker)
			{
				if (artag_is_object_found(multiMarker->getMultiPatternID()))
				{
					if (m_debugmode)
						osg::notify() << "Multi Marker id : " << multiMarker->getMultiPatternID() << "found" << std::endl;\
					multiMarker->update();
				}			
			}								
		}		
		if(m_debugmode)
			osg::notify() << "<-Stop" << getLabel() << "::update()" << std::endl;
	}

	void ARTagTracker::setProjection(const double n, const double f) 
	{
		//ARTag does not provide the projection matrix, we calculate it by hand!
		for (int j = 0; j < 16; j++)	
			m_projectionMatrix[j] = 0.;

		//change DEFAULT_FOCAL_FX to a variable
		double camRight,camLeft,camTop,camBottom;

		camRight = (double)m_width / (double)(2.0 * DEFAULT_FOCAL_FX);
		camLeft = -camRight;
		camTop = (double)m_height / (double)(2.0 * DEFAULT_FOCAL_FY);
		camBottom = -camTop;

		//see http://www.mevis.de/opengl/glFrustum.html
		double A, B, C, D;
		A = (camRight+camLeft)/(camRight-camLeft); // 0
		B = (camTop+camBottom)/(camTop-camBottom); // 0
		C = (f+n)/(f-n);							//
		D = (2*f*n)/(f-n);							//

			
		m_projectionMatrix[0 *4 + 0] = 2*n / (camRight - camLeft);
		// =2n/2camRight = n/camRight
		// = n/(width / 2 FOCALX) = 2n/(width/FOCALX) = (2 n FOCALX)/width = 2*10/ (640 / 1100)
		// = (2 n FOCALX)/width = 2.19
		// n = 10
		// width = 640
		// focalX = 2.19* 640 / (2 * 10 ) = 70
		m_projectionMatrix[1 *4 + 1] = 2*n / (camTop - camBottom);
		m_projectionMatrix[2 *4 + 0] = A;
		m_projectionMatrix[2 *4 + 1] = B;
		m_projectionMatrix[2 *4 + 2] = -C;
		m_projectionMatrix[2 *4 + 3] = -1;
		m_projectionMatrix[3 *4 + 2] = -D;


		m_projectionMatrix[0 *4 + 0] *= 4.37/21.875;//2*n / (camRight - camLeft);
		m_projectionMatrix[1 *4 + 1] *= 5.83/29.167;//2*n / (camTop - camBottom);
		m_projectionMatrix[2 *4 + 0] = 0;
		m_projectionMatrix[2 *4 + 1] = 0;
		m_projectionMatrix[2 *4 + 2] = -1;
		m_projectionMatrix[2 *4 + 3] *= -2/-1;
	//	m_projectionMatrix[3 *4 + 2] *= -1/-20.025;

		osg::Matrix		tmp(m_projectionMatrix);
		if (m_debugmode)
		{
			osgART::PrintMatrix("ARTag projection Matrix :", tmp);
			osg::notify() << "CamRight/left :" <<  camRight << "/"<<camLeft << std::endl;
			osg::notify() << "CamTop/Bottom :" <<  camTop	<< "/"<<camBottom << std::endl;
		}
	}

	void ARTagTracker::createUndistortedMesh(
		int width, int height,
		float maxU, float maxV,
		osg::Geometry &geometry)
	{
/*
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
*/	}

}; // namespace osgART
