#include "ARToolKitPlusTracker"

/* Versin Check???
#ifndef AR_HAVE_HEADER_VERSION_
#error ARToolKitPlus  v2.1 or later is required to build the OSGART ARToolKit tracker.
#endif
*/

//#include "SingleMarker"
//#include "MultiMarker"

#include <osgART/GenericVideo>
#include "osgART/VideoPlugin"
#include "osgART/VideoConfig"
#include "osgART/Utils"

#include <iostream>
#include <fstream>

#ifdef AR_TOOLKIT_PROFILER
#include <SG_TLS_exceptions.h>
#include <SG_TLS_cl_base_obj.h>
#include "ProfilerTools"
#endif

namespace osgART {

	
DLL_API osgART::GenericTracker* osgart_create_tracker()
{
	return new osgART::ARToolKitTracker_Plus();	
}

OSGART_PLUGIN_ENTRY()


//==============================================================================
//==============================================================================
//==============================================================================
//				tracker definition			
//==============================================================================
//==============================================================================
//==============================================================================

ARToolKitTracker_Plus::ARToolKitTracker_Plus() :
#ifdef AR_TRACKER_PROFILER
	ARToolKitTrackerProfiler<int>(),
#else
	GenericTracker(),
#endif
			m_threshold		(_ART_PLUS_DFLT_THRESHOLD),
			m_debugMode		(false),
			m_marker_num	(0),
			m_FarClip		(_ART_PLUS_DFLT_FARCLIP),
			m_NearClip		(_ART_PLUS_DFLT_NEARCLIP),
			m_markerMode	(_ART_PLUS_DFLT_MARKERMODE),
			m_imageProcMode	(_ART_PLUS_DFLT_IMG_PROC_MODE),
			m_poseEstimMode	(_ART_PLUS_DFLT_POSE_ESTIM),
			m_undistortMode	(_ART_PLUS_DFLT_UNDISTORT_MODE),
			m_AutoThresholdRetriesNbr(_ART_PLUS_DFLT_THRESHOLD_AUTO_RETRY_NBR),
			m_useDetectLite	(_ART_PLUS_DFLT_USE_LITE),
			m_PlusTracker	(NULL)
	{
		//version and name of the tracker
		m_name		= "ARToolkitPlus";
		m_version	= "2.1";

		//attach the field to the corresponding values
		m_fields["nearclip"]	= new TypedField<float>(&m_NearClip);
		m_fields["farclip"]		= new TypedField<float>(&m_FarClip);
		m_fields["markercount"] = new TypedField<int>(&m_marker_num);
		
		//attach the field to the corresponding callbacks
		m_fields["threshold"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getThreshold,
			&ARToolKitTracker_Plus::setThreshold);

		m_fields["autothreshold"] = new CallbackField<ARToolKitTracker_Plus,bool>(this,
			&ARToolKitTracker_Plus::getAutoThreshold,
			&ARToolKitTracker_Plus::setAutoThreshold);

		m_fields["autothreshold_retries"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getAutoThresholdRetries,
			&ARToolKitTracker_Plus::setAutoThresholdRetries);
			
		m_fields["marker_mode"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getMarkerMode,
			&ARToolKitTracker_Plus::setMarkerMode);

		m_fields["img_proc_mode"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getImgProcMode,
			&ARToolKitTracker_Plus::setImgProcMode);

		m_fields["pos_estim_mode"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getPosEstimMode,
			&ARToolKitTracker_Plus::setPosEstimMode);

		m_fields["undistort_mode"] = new CallbackField<ARToolKitTracker_Plus,int>(this,
			&ARToolKitTracker_Plus::getUndistortionMode,
			&ARToolKitTracker_Plus::setUndistortionMode);

		m_fields["use_detect_lite"] = new CallbackField<ARToolKitTracker_Plus,bool>(this,
			&ARToolKitTracker_Plus::getUseDetectLite,
			&ARToolKitTracker_Plus::setUseDetectLite);
		
		m_fields["debug"]		= new CallbackField<ARToolKitTracker_Plus,bool>(this,
			&ARToolKitTracker_Plus::getDebugMode,
			&ARToolKitTracker_Plus::setDebugMode);

		//set the internal format of ARToolkit Plus, defined at compile time
		//see setImageRaw() for conversion
		m_arInternalFormat	= ConvertARTPixelFormatToOSGART( ARToolKitPlus::PIXEL_FORMAT_BGR);
	}

	ARToolKitTracker_Plus::~ARToolKitTracker_Plus()
	{
		if (m_PlusTracker)
			delete m_PlusTracker;
	}	

bool ARToolKitTracker_Plus::CreateTracker(
			ARToolKitPlus::MARKER_MODE _markerMode,
			int _Width, int _Height)
	{
		//create the tracker depending on the mode
		switch(_markerMode)
		{
			case ARToolKitPlus::MARKER_ID_SIMPLE:
				m_PlusTracker = (ARToolKitPlus::TrackerMultiMarker *) (new ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE>());
				break;
			case ARToolKitPlus::MARKER_TEMPLATE:
				m_PlusTracker = (ARToolKitPlus::TrackerMultiMarker *) (new ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE>());//
				break;
			case ARToolKitPlus::MARKER_ID_BCH:
				osg::notify(osg::FATAL) << "ARToolKitPlus::CreateTracker() :  marker mode 'MARKER_ID_BCH' not done yet" << std::endl;	
				exit(-1);
				break;
			default:
				osg::notify(osg::FATAL) << "ARToolKitPlus::CreateTracker() :  unknown marker mode '"<< _markerMode << "'" << std::endl;	
				exit(-1);
		}

		if (!m_PlusTracker)
			return false;

		//size of the marker is done in init function...
		
		printf("ARToolKitPlus compile-time information:\n%s\n\n",	m_PlusTracker->getDescription());	
		return true;
	}

	bool ARToolKitTracker_Plus::init(int _xsize, int _ysize, 
		const std::string& _pattlist_name, 
		const std::string& _camera_name)
	{
		m_width = _xsize;
		m_height = _xsize; 
		if (!CreateTracker	(m_markerMode, _xsize,_ysize))
		{
			osg::notify(osg::FATAL) << "ARToolKitPlus::init() :  could not create Tracker!" << std::endl;	
			exit(-1);
		}


		//init them
			setMarkerMode(m_markerMode);
			setPixelFormat((ARToolKitPlus::PIXEL_FORMAT)
					ConvertOSGARTPixelFormatToART(m_arInternalFormat));
			
			
			//m_PlusTracker->setLoadUndistLUT(true);
			
			switch(m_markerMode)
			{
				case ARToolKitPlus::MARKER_TEMPLATE: setBorderWidth(0.25f); break;
				case ARToolKitPlus::MARKER_ID_SIMPLE:
				default: setBorderWidth(0.125f);
			}
			setPatternWidth(80.);
		

			//just because the tracker need to be refreshed
			setDebugMode(getDebugMode());	
			setThreshold(getThreshold());
			setImgProcMode(getImgProcMode());
			setAutoThreshold(getAutoThreshold());
			setAutoThresholdRetries(getAutoThresholdRetries());
			setPosEstimMode(getPosEstimMode());
			setUseDetectLite(getUseDetectLite());
			setUndistortionMode(getUndistortionMode());
			//==============================================

			//m_PlusTracker->activateIdBasedMarkers(true);	
			
			
		//==========================

		// Set the initial camera parameters.
			m_cparamName = _camera_name;
			m_width = _xsize;
			m_height= _ysize;

			osg::notify() << "Loading camera param file : " << m_cparamName << std::endl;
			
			if (!LoadCameraFile((char*)m_cparamName.c_str(), m_NearClip, m_FarClip))
			{
				osg::notify(osg::FATAL) << "Can not load camera file : "<< m_cparamName << std::endl;	
				exit(-1);
			}

			m_PlusTracker->changeCameraSize(_xsize,_ysize);
			setProjection(m_NearClip, m_FarClip);
		
			if (!setupMarkers(_pattlist_name))
			{
				osg::notify(osg::FATAL) << "Could not load the pattern list file :" << _pattlist_name << std::endl;
			}
		
		//tracker->init("data/LogitechPro4000.dat", 1.0f, 1000.0f);
		return true;
	}

//===================================
//	Accessing Functions Get/Set......
//===================================


/*protected*/
	bool	ARToolKitTracker_Plus::LoadCameraFile(std::string _file, float _nearClip, float _farClip)
	{
		if (m_PlusTracker)
		{
			if (!m_PlusTracker->loadCameraFile((char*)m_cparamName.c_str(), _nearClip, _farClip))
				return false;
		}		
		return true;	
	}
	 
	void	ARToolKitTracker_Plus::setPixelFormat(ARToolKitPlus::PIXEL_FORMAT _format)
	{
		if (m_PlusTracker)
			m_PlusTracker->setPixelFormat(_format);
	}
	 
	void	ARToolKitTracker_Plus::setBorderWidth(float _border)
	{
		if (m_PlusTracker)
			m_PlusTracker->setBorderWidth(_border);
	}
	  	//??, in config file??

	void	ARToolKitTracker_Plus::setPatternWidth(float _width)
	{//???
		m_pattern_width = _width;
		/*if (m_PlusTracker)
			m_PlusTracker->set setPatternWidth(_width);
			*/
	}

	void	ARToolKitTracker_Plus::setUseDetectLite(const bool& _mode)
	{
		m_useDetectLite = _mode;
		if (m_PlusTracker)
			m_PlusTracker->setUseDetectLite(m_useDetectLite);
	}
	bool 	ARToolKitTracker_Plus::getUseDetectLite()const
	{	return m_useDetectLite;	}
	 
	 
	//ARToolKitPlus::MARKER_MODE 
	void ARToolKitTracker_Plus::setMarkerMode(const int &_mode)
	{
		m_markerMode = (ARToolKitPlus::MARKER_MODE )_mode;
		if (m_PlusTracker)
			m_PlusTracker->setMarkerMode(m_markerMode);
	}
/*public*/
	 
	//ARToolKitPlus::MARKER_MODE 
	int ARToolKitTracker_Plus::getMarkerMode() const
	{
		return m_markerMode;	
	}	

	//Auto Threshold
	void	ARToolKitTracker_Plus::setAutoThreshold(const bool &_val)
	{
		if (m_PlusTracker)
			m_PlusTracker->activateAutoThreshold(_val);
	}

	bool	ARToolKitTracker_Plus::getAutoThreshold()const
	{
		return m_PlusTracker->isAutoThresholdActivated();
	}

	//Auto threshold retries
	void	ARToolKitTracker_Plus::setAutoThresholdRetries	(const int & _nbr)
	{
		m_AutoThresholdRetriesNbr = _nbr;
		if (m_PlusTracker)
			m_PlusTracker->setNumAutoThresholdRetries(_nbr);	
	}

	int 	ARToolKitTracker_Plus::getAutoThresholdRetries	()const
	{	return m_AutoThresholdRetriesNbr;	}

	//Threshold
	void ARToolKitTracker_Plus::setThreshold(const int &thresh)	{
		// jcl64: Clamp to 0-255, hse25: use osg func
		m_threshold = osg::clampBetween(thresh,0,255);
		if (m_PlusTracker)
			m_PlusTracker->setThreshold(m_threshold);	
	}

	int ARToolKitTracker_Plus::getThreshold()const
	{ 	return m_threshold;	}

	//undistortion mode
	void	ARToolKitTracker_Plus::setUndistortionMode	(const int & _mode)
	{
		m_undistortMode = (ARToolKitPlus::UNDIST_MODE) _mode;
		if (m_PlusTracker)
			m_PlusTracker->setUndistortionMode(m_undistortMode);	
	}
	
	int ARToolKitTracker_Plus::getUndistortionMode	()const
	{	return m_undistortMode;	}
	

	//?? 
	unsigned char* ARToolKitTracker_Plus::getDebugImage()
	{ 		return m_imageptr; 	}

	//debug mode
	void ARToolKitTracker_Plus::setDebugMode(const bool &d) 
	{ 	m_debugMode = d;}

	bool ARToolKitTracker_Plus::getDebugMode()const  
	{ 	return m_debugMode;}
	 
	void ARToolKitTracker_Plus::setImageRaw(unsigned char * image, PixelFormatType format)
    {//We are only augmenting method in parent class.
		if (format != m_imageptr_format)
		{
			if (m_PlusTracker)
				m_PlusTracker->setPixelFormat((ARToolKitPlus::PIXEL_FORMAT) ConvertOSGARTPixelFormatToART(format));
		}
		
		GenericTracker::setImageRaw(image, format);
    }
	 
	void ARToolKitTracker_Plus::setProjection(const double n, const double f)
	{
		m_NearClip	= n;
		m_FarClip	= f;
		bool res = false;
		ARFloat projectionMatrix_fl[16];//used for the convertion

		switch(m_markerMode)
		{
			case ARToolKitPlus::MARKER_ID_SIMPLE:
				res = ARToolKitPlus::TrackerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE >::calcCameraMatrix(m_cparamName.c_str(), m_width, m_height, n, f, projectionMatrix_fl);
				break;
			case ARToolKitPlus::MARKER_TEMPLATE:
				res = ARToolKitPlus::TrackerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE >::calcCameraMatrix(m_cparamName.c_str(), m_width, m_height, n, f, projectionMatrix_fl);
				break;
				case ARToolKitPlus::MARKER_ID_BCH:
				osg::notify(osg::FATAL) << "ARToolKitPlus::setProjection() :  marker mode 'MARKER_ID_BCH' not done yet" << std::endl;	
				exit(-1);
				break;
			default:
				osg::notify(osg::FATAL) << "ARToolKitPlus::setProjection() :  unknown marker mode '"<< m_markerMode << "'" << std::endl;	
				exit(-1);
		}

		if(!res)
		{
			osg::notify(osg::FATAL) << "Could not get camera calibration : " << m_cparamName << std::endl;
			exit(-1);
		}

		const ARFloat *projectionMatrix_fl2 = m_PlusTracker->getProjectionMatrix();
	
		//convert from float to Double
		for (int i=0;i<16; i++)
			m_projectionMatrix[i] = projectionMatrix_fl[i];

		if (m_debugMode)
			PrintMatrix("SetProjectionMatrix() before conversion", osg::Matrix(m_projectionMatrix));
		

		//convert matrix from left hand to right hand
		//see arglCameraFrustumRH for details
		m_projectionMatrix[0 + 2 *4] *= -1; //	q[0][2] = ((2.0 * p[0][2] / (width - 1))  - 1.0);
		m_projectionMatrix[1 + 1 *4] *= -1; //	q[1][1] = (2.0 * p[1][1] / (height - 1));
		m_projectionMatrix[1 + 2 *4] *= -1; //	q[1][2] = ((2.0 * p[1][2] / (height - 1)) - 1.0);
		m_projectionMatrix[2 + 2 *4] *= -1; //	q[2][3] = -2.0 * focalmax * focalmin / (focalmax - focalmin);
		m_projectionMatrix[3 + 2 *4] *= -1; //	q[3][2] = 1.0;

		if (m_debugMode)
			PrintMatrix("SetProjectionMatrix() aftet right hand conversion", osg::Matrix(m_projectionMatrix));
	}


	//image processing mode
	void	ARToolKitTracker_Plus::setImgProcMode(const int & _imgMode)
	{
		m_imageProcMode = (ARToolKitPlus::IMAGE_PROC_MODE) _imgMode;
		if (m_PlusTracker)
			m_PlusTracker->setImageProcessingMode(m_imageProcMode);
	}

	int ARToolKitTracker_Plus::getImgProcMode()const
	{	return m_imageProcMode;	}


	//pose estimation mode
	void	ARToolKitTracker_Plus::setPosEstimMode(const int & _Mode)
	{
		m_poseEstimMode = (ARToolKitPlus::POSE_ESTIMATOR)_Mode;
		if (m_PlusTracker)
			m_PlusTracker->setPoseEstimator(m_poseEstimMode);
	}

	int ARToolKitTracker_Plus::getPosEstimMode()const
	{	return m_poseEstimMode;	}
	
	void ARToolKitTracker_Plus::PrintOptions()const
	{
		std::cout << "===== TRACKER OPTIONS : " << getLabel() <<   std::endl;
		std::cout << "* Threshold value	: " << m_threshold <<   std::endl;
		std::cout << "* Threshold auto	: " ;
			if (getAutoThreshold())
				std::cout <<"ON"<<  std::endl;
			else
				std::cout <<"OFF"<<  std::endl;
		std::cout << "* Image processing mode : ";
			if (m_imageProcMode)
				std::cout <<"FULL" <<   std::endl;
			else
				std::cout <<"HALF" <<   std::endl;
		std::cout << "* arDetectLite mode : ";
			if (m_useDetectLite)
				std::cout << "LITE"  <<   std::endl;
			else
				std::cout << "NORMAL" <<   std::endl;
		std::cout << "* Pose Estimation : ";
			if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL)
				std::cout << "POSE_ESTIMATOR_ORIGINAL"  <<   std::endl;
			else if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL_CONT)
				std::cout << "POSE_ESTIMATOR_ORIGINAL_CONT"  <<   std::endl;
			else if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_RPP)
				std::cout << "POSE_ESTIMATOR_RPP"  <<   std::endl;
			else
                std::cout << "Unknown" <<   std::endl;
		std::cout << "* Undistortion mode : ";
			if (m_undistortMode == ARToolKitPlus::UNDIST_LUT)
				std::cout << "UNDIST_LUT"  <<   std::endl;
			else if (m_undistortMode == ARToolKitPlus::UNDIST_NONE)
				std::cout << "UNDIST_NONE"  <<   std::endl;
			else if (m_undistortMode == ARToolKitPlus::UNDIST_STD)
				std::cout << "UNDIST_STD"  <<   std::endl;
			else
                std::cout << "Unknown" <<   std::endl;
	}

//++++++++++++++++++++++++++++++++++++
//	Accessing Functions Get/Set.......
//++++++++++++++++++++++++++++++++++++

	bool ARToolKitTracker_Plus::setupMarkers(const std::string& patternListFile)
	{
		osg::notify() << "Setting up markers from file : " << patternListFile << std::endl;
		std::ifstream markerFile;

		// Need to check whether the passed file even exists
		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open())
		{
			osg::notify(osg::WARN) << "File does not exist : " << patternListFile << std::endl;
			return false;
		}

		bool ret = true;

		int patternNum = 0;
		markerFile >> patternNum;

		std::string patternName, patternType;

		// Need EOF checking in here... atm it assumes there are really as many markers as the number says

		for (int i = 0; i < patternNum; i++)
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
				ret = false;
				break;
			}
		}
		markerFile.close();

		return ret;
	}

	//Single
	int ARToolKitTracker_Plus::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		Marker* singleMarker = new SingleMarker(m_PlusTracker);
		if (!static_cast<SingleMarker*>(singleMarker)->initialise(pattFile, width, center))
		{
			singleMarker->unref();
			return -1;
		}

		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;//return the ID of the pattern
	}

	//Multi
	int ARToolKitTracker_Plus::addMultiMarker(const std::string& multiFile) 
	{
		//we add directly the TrackerMultiMarker  object
		Marker* multiMarker = new MultiMarker(m_PlusTracker,m_markerMode );

		if (!static_cast<MultiMarker*>(multiMarker)->initialise(multiFile))
		{
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	 
	int ARToolKitTracker_Plus::ConvertOSGARTPixelFormatToART(PixelFormatType format)const
	{
		using namespace ARToolKitPlus;
		switch (format)
		{
			case VIDEOFORMAT_RGB24: return PIXEL_FORMAT_RGB;
			case VIDEOFORMAT_BGR24:	return PIXEL_FORMAT_BGR;
			case VIDEOFORMAT_BGRA32:return PIXEL_FORMAT_BGRA;
			case VIDEOFORMAT_RGBA32:return PIXEL_FORMAT_RGBA;
			case VIDEOFORMAT_ABGR32:return PIXEL_FORMAT_ABGR;
			case VIDEOFORMAT_Y8:
			case VIDEOFORMAT_GREY8:
									return PIXEL_FORMAT_LUM;
			default:
				osg::notify(osg::WARN) << "ConvertOSGARTPixelFormatToART() : Unknown pixel format!" << std::endl;
				return 0;
		}        
		return 0;
	}

	PixelFormatType ARToolKitTracker_Plus::ConvertARTPixelFormatToOSGART(int format)const
	{
		using namespace ARToolKitPlus;
		switch (format)
		{
			case PIXEL_FORMAT_RGB : return VIDEOFORMAT_RGB24;
			case PIXEL_FORMAT_BGR : return VIDEOFORMAT_BGR24;
			case PIXEL_FORMAT_BGRA :return VIDEOFORMAT_BGRA32;
			case PIXEL_FORMAT_RGBA :return VIDEOFORMAT_RGBA32;
			case PIXEL_FORMAT_ABGR :return VIDEOFORMAT_ABGR32;
			case PIXEL_FORMAT_LUM :return VIDEOFORMAT_Y8;//or VIDEOFORMAT_GREY8:
			default:
				osg::notify(osg::WARN) << "ConvertARTPixelFormatToOSGART() : Unknown pixel format!" << std::endl;
				
		}        
		return VIDEOFORMAT_ANY;
	}

    
	/*virtual*/ 	
	void ARToolKitTracker_Plus::update()
	{	
#if AR_TRACKER_PROFILE
		osg::notify() <<  "->" << m_versionName << "::update()" << std::endl;
		static CL_FUNCT_TRC<CL_TimerVal>	*ThisFct			= this->LocalARTimeTracer->AddFunct	("arDetectMarker_TIME");		
#endif
		
	if (m_imageptr== NULL)
		return;

	ARToolKitPlus::ARMarkerInfo    *marker_info;
	float confidence = 0.0f;  

    // detect the markers in the video frame
	//
#if AR_TRACKER_PROFILE
	AR_BENCH_TIME(ThisFct, 
		if(m_PlusTracker->arDetectMarker(const_cast<unsigned char*>(m_imageptr), m_threshold, &marker_info, &m_marker_num) < 0)
		{
			return;
		}
	, 1, //pattern in memory
	m_versionName, m_marker_num);
#else 
		if(m_PlusTracker->arDetectMarker(const_cast<unsigned char*>(m_imageptr), m_threshold, &marker_info, &m_marker_num) < 0)
		{
			return;
		}
#endif

	osg::notify() << "arDetectMarker() => Markerdetected = " <<m_marker_num<<std::endl;

		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (MarkerList::iterator iter = m_markerlist.begin(); 
				iter != m_markerlist.end(); 
				iter++)		
		{
			SingleMarker*	singleMarker = dynamic_cast<SingleMarker*>((*iter).get());
			MultiMarker*	multiMarker  = dynamic_cast<MultiMarker*>((*iter).get());

			if (singleMarker)
			{
				// find best visible marker
				int j, k = -1;
				for(j = 0; j < m_marker_num; j++)
				{
					if (singleMarker->getPatternID() == marker_info[j].id)
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.
					}
				}

				if(k != -1) 
				{					
					singleMarker->update(&marker_info[k]);
			#if 0//AR_TRACKER_PROFILE // remove the 0 after????
					double Coef = marker_info[k].cf;
					RecordMarkerStats(marker_info[k].id, Coef, singleMarker->m_transform, true);
			#endif// AR_TRACKER_PROFILE
				} 
				else 
				{
					//PROFILE_ENDSEC(profiler, SINGLEMARKER_OVERALL)
					singleMarker->update(NULL);
				}

				confidence = marker_info[k].cf;


				/////////////////////////////////////////////////////////////////////////
				//
				//       corner refinement begin
				//
			/*	if(false)
				{
					const unsigned int roi_radius = 4;
					for(unsigned int i=0; i<4; i++)
					{
						ARFloat edge_x, edge_y;
						int c_ret = refineCorner(edge_x, edge_y,
							marker_info[k].vertex[i][0],
							marker_info[k].vertex[i][1],
							roi_radius, (void*) nImage,
							arCamera->xsize, arCamera->ysize);

						if(c_ret == 1)
						{
							marker_info[k].vertex[i][0] = edge_x;
							marker_info[k].vertex[i][1] = edge_y;
						}
					}
				}*/
				//
				//       corner refinement end
				//
				/////////////////////////////////////////////////////////////////////////
				}
				else if (multiMarker)
				{
					multiMarker->update(marker_info, m_marker_num);
				}
				else {
					osg::notify(osg::WARN)<< "ARToolKitPlusTracker::update() : Unknown marker type id!" << std::endl;
				}
			}
		
#if AR_TRACKER_PROFILE
		osg::notify() << "<-Stop" << m_versionName << "::update()" << std::endl;
#endif//AR_TRACKER_PROFILE
	}

//==============================================================================
//==============================================================================
//==============================================================================
//				SingleMarker definition			
//==============================================================================
//==============================================================================
//==============================================================================
	SingleMarker::SingleMarker(ARToolKitPlus::TrackerMultiMarker * _PlusTracker) 
		:	Marker(),
			m_ParentTracker(_PlusTracker),
			m_patt_border_width(0.25f),
			m_patt_width(80.f),
			m_patt_id(-1)
	{
		if (!m_ParentTracker)
		{
			osg::notify(osg::FATAL) << "Could not create ARToolkitplus SingleMarker, no ARToolkitplus tracker." << std::endl;
			exit(-1);
		}
		m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	SingleMarker::~SingleMarker() {
		// jcl64: Free the pattern
		/*if (m_patt_id > 0) 
			m_ParentTracker->arFreePatt(m_patt_id);
		*///m_ParentTracker sould be a osg_ref...???
	}

	Marker::MarkerType SingleMarker::getType() const {
		return Marker::ART_SINGLE;
	}

	bool SingleMarker::initialise(const std::string& pattFile, double width, double center[2])
	{
		sscanf(pattFile.c_str(), "%d", &m_patt_id);
		if (m_patt_id==-1)
		//template mode
			m_patt_id = m_ParentTracker->arLoadPatt((char *)pattFile.c_str());
			
		//else
		//id mode, nothing to do
		
		if (m_patt_id < 0) return false;
		m_patt_width = width;
		m_patt_center[0] = center[0];
		m_patt_center[1] = center[1];
		setName(pattFile);
		setActive(true);
		return true;
	}

	void SingleMarker::update(ARToolKitPlus::ARMarkerInfo* markerInfo) {		
		if (markerInfo == NULL) {
			m_valid = false;
		} else {
			ARFloat patt_center_fl[2]		= {m_patt_center[0],m_patt_center[1]};
			ARFloat patt_trans_fl[3][4];
			m_ParentTracker->arGetTransMat(markerInfo, patt_center_fl, m_patt_width, patt_trans_fl);
			m_confidence = markerInfo->cf;
			m_ParentTracker->executeSingleMarkerPoseEstimator(markerInfo, patt_center_fl, m_patt_width, patt_trans_fl);	
			m_valid = true;

			double modelView[16];
			
			for (int i=0;i<4; i++)
			{
				modelView[i*4]   = patt_trans_fl[0][i];
				modelView[i*4+1] = patt_trans_fl[1][i];
				modelView[i*4+2] = patt_trans_fl[2][i];
				modelView[i*4+3] = 0;
			}
			modelView[15] = 1;
			
			osg::Matrix tmp(modelView);

			//convert from Left Handed view to Right Handed
			tmp = tmp * osg::Matrixd::scale(-1,-1,1);
			tmp = tmp * osg::Matrixd::rotate(osg::inDegrees(180.), 0.,1.,0.);
			
			updateTransform(tmp);
		}
		
	}

	void SingleMarker::setActive(bool a) {
		m_active = a;
		//no function to activate...
	}

	int SingleMarker::getPatternID() {
		return m_patt_id;
	}

	double SingleMarker::getPatternWidth() {
		return m_patt_width;
	}
		
	double* SingleMarker::getPatternCenter() {
		return m_patt_center;
	}
	

//==============================================================================
//==============================================================================
//==============================================================================
//				MultiMarker definition			
//==============================================================================
//==============================================================================
//==============================================================================	
	
	MultiMarker::MultiMarker(ARToolKitPlus::TrackerMultiMarker * _PlusTracker, ARToolKitPlus::MARKER_MODE markerMode	)
		: Marker(),
		m_ParentTracker(_PlusTracker),
		m_ParentTrackerTemplate(NULL),
		m_ParentTrackerSimple(NULL),
		m_multi(NULL)
	{
		if (!m_ParentTracker)
		{
			osg::notify(osg::FATAL) << "Could not create ARToolkitplus MultiMarker, no ARToolkitplus tracker." << std::endl;
			exit(-1);
		}

		switch (markerMode)
		{	
			case ARToolKitPlus::MARKER_ID_SIMPLE :
				m_ParentTrackerSimple = dynamic_cast< ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE> * >(_PlusTracker);
				break;
			case ARToolKitPlus::MARKER_TEMPLATE :
				m_ParentTrackerTemplate = dynamic_cast< ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE> * >(_PlusTracker);
				break;
			default:
				osg::notify(osg::WARN) << "Unkown marker mode in MultiMarker" << std::endl;
		}
		
//		m_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	MultiMarker::~MultiMarker() {   
		// jcl64: Free the multimarker
		if (m_multi)
		{
	/*		if (ParentTrackerSimple)
				ParentTrackerSimple->arMultiFreeConfig(m_multi);
			else if (ParentTrackerTemplate)
				ParentTrackerTemplate->arMultiFreeConfig(m_multi);
	*///crash always..??	
		}
	}

	/* virtual */
	Marker::MarkerType MultiMarker::getType() const {
		return Marker::ART_MULTI;
	}

	bool 
	MultiMarker::initialise(const std::string& multiFile) {
		
		// Check if multifile exists		
		if (m_ParentTrackerSimple)
			m_multi =  m_ParentTrackerSimple->arMultiReadConfigFile(multiFile.c_str());
		else if (m_ParentTrackerTemplate)
			m_multi =  m_ParentTrackerTemplate->arMultiReadConfigFile(multiFile.c_str());
		else
			osg::notify(osg::WARN) << "No Tracker in MultiMarker, could not initialise." << std::endl;
		
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void
	MultiMarker::setActive(bool a) {
		m_active = a;	
		//no function to activate...
	}

	//get all the marker detected and look if one of them are in the multimarker group.??
	void 
	MultiMarker::update(ARToolKitPlus::ARMarkerInfo* markerInfo, int markerCount) 
	{
		if (markerInfo == NULL) {
			m_valid = false;
		} else {

			m_valid = (m_ParentTracker->arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
			
			double modelView[16];

		//m_multi[]...
		//arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			//ParentTracker->executeMultiMarkerPoseEstimator(markerInfo, markerCount, m_multi, patt_trans_fl);
			
			for (int i=0;i<4; i++)
			{
				modelView[i*4]   = m_multi->trans[0][i];
				modelView[i*4+1] = m_multi->trans[1][i];
				modelView[i*4+2] = m_multi->trans[2][i];
				modelView[i*4+3] = 0;
			}
			modelView[15] = 1;
						
			osg::Matrix tmp(modelView);
			//convert from Left Handed view to Right Handed
			tmp = tmp * osg::Matrixd::scale(-1,-1,1);
			tmp = tmp * osg::Matrixd::rotate(osg::inDegrees(180.), 0.,1.,0.);
			updateTransform(tmp);
		}
	}

}; // namespace osgART