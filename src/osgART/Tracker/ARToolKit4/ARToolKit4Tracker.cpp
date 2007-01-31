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

#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#ifndef AR_HEADER_VERSION_MAJOR
#error ARToolKit v4.0 or later is required to build the OSGART ARToolKit tracker.
#endif

#include "SingleMarker"
#include "MultiMarker"

#include <osgART/GenericVideo>

#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

namespace osgART {

#define AR4_DFLT_THRESHOLD 100
#define AR4_DFLT_DEBUGMODE false

	ARToolKit4Tracker::ARToolKit4Tracker() : 
#if  AR_TRACKER_PROFILE

	ARToolKitTrackerProfiler<int>(),

#else

	GenericTracker(),

#endif
		MainAR4_PattList(CreateARPattHandle()),
		gARHandle(NULL),
		gAR3DHandle(NULL),
		m_debugmode(false)
	{
		//version and name of the tracker

		m_name		= "ARToolkit";

		m_version	= AR_HEADER_VERSION_STRING;

		__AR_DO_PROFILE(m_version+="(Prf)");
		//other attached field are set into the createARHandle()
	}

	ARToolKit4Tracker::~ARToolKit4Tracker()
	{
		DeleteARPattHandle(MainAR4_PattList);
		arMainPattListDetach();
		ar3DDeleteHandle(gAR3DHandle);
		arDeleteHandle(gARHandle);
	}

	ARHandle * ARToolKit4Tracker::CreateARHandle(ART4_ARParam  *wparam)
	{
		gARHandle = arCreateHandle (wparam);
		if (!gARHandle)
		{
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::CreateARHandle() : Could not create ARHandle." << endl;
			exit(-1);
		}

		//callback fields

		m_fields["threshold"]	= new CallbackField<ARToolKit4Tracker,int>(this,

			&ARToolKit4Tracker::getThreshold,

			&ARToolKit4Tracker::setThreshold);
		m_fields["debug"]		= new CallbackField<ARToolKit4Tracker,bool>(this,

			&ARToolKit4Tracker::getDebugMode,

			&ARToolKit4Tracker::setDebugMode);
		
		// attach a new field to the name "markercount"
		m_fields["markercount"] = new TypedField<int>(&gARHandle->marker_num);

		//attach the main PatternList to the ARhandle
		arMainPattListAttach();
		return gARHandle;
	}

	ARPattHandle * ARToolKit4Tracker::CreateARPattHandle()
	{
		return arPattCreateHandle();
	}

	int ARToolKit4Tracker::DeleteARPattHandle(ARPattHandle *_Hdle)
	{
		return arPattDeleteHandle(_Hdle);
	}

	int ARToolKit4Tracker::arMainPattListAttach()
	{
		return arPattAttach(gARHandle, MainAR4_PattList);
	}	
	
	int ARToolKit4Tracker::arMainPattListDetach()
	{
		return arPattDetach(gARHandle);
	}

	bool ARToolKit4Tracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{
		

		m_width = xsize;

		m_height = ysize, 


		cout << "ARToolKit4Tracker::init()..." << endl;
		ART4_ARParam  wparam;
		
	    // Set the initial camera parameters.
		cparamName = camera_name;
	   
		if(arParamLoad(cparamName.c_str(), 1, &wparam) < 0) {
			std::cerr << "ERROR: Camera parameter load error." << std::endl;
			return false;
	    }

		arParamChangeSize(&wparam, xsize, ysize,(ART4_ARParam*)&m_cparam);
	    arParamDisp((ARParam*)&m_cparam);
		//--------------------------------------
		
		//create main ARhandle
		CreateARHandle (&m_cparam);
		
		//arFittingMode = AR_FITTING_TO_IDEAL;
	    //arImageProcMode = AR_IMAGE_PROC_IN_FULL;

		setProjection(10.0f, 8000.0f);
		setDebugMode(AR4_DFLT_DEBUGMODE);
		setThreshold(AR4_DFLT_THRESHOLD);

		if (!setupMarkers(pattlist_name)) {
			std::cerr << "ERROR: Marker setup failed." << std::endl;
			return false;
		}

		if ((gAR3DHandle = ar3DCreateHandle(&m_cparam)) == NULL) {
			fprintf(stderr, "setupCamera(): Error: ar3DCreateHandle.\n");
			return (FALSE);
		}

		// Success
		return true;
	}

	bool ARToolKit4Tracker::setupMarkers(const std::string& patternListFile)
	{
		std::ifstream markerFile;

		// Need to check whether the passed file even exists
		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open()) return false;

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

	int ARToolKit4Tracker::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		std::cout << "Adding new single marker..." << pattFile << std::endl;
		Marker* NewSingleMarker = new SingleMarker(MainAR4_PattList);
		
		if (!NewSingleMarker)
		{
			osg::notify(osg::WARN) <<  "ARToolKit4Tracker::addSingleMarker() : Could not create singleMarker!" << endl;
			return -1;
		}
		else if (!gARHandle)
		{
			osg::notify(osg::FATAL) <<  "ARToolKit4Tracker::addSingleMarker() : gARHandle is empty, please run init() function before!" <<  endl;
			exit(-1);
		}

		if (!static_cast<SingleMarker*>(NewSingleMarker)->initialise(gARHandle, pattFile, width, center))
		{
			NewSingleMarker->unref();
			osg::notify(osg::WARN) <<  "ARToolKit4Tracker::addSingleMarker() : Could not init singleMarker!" << endl;
			return -1;
		}		
		m_markerlist.push_back(NewSingleMarker);

		return m_markerlist.size() - 1;
	}

	int ARToolKit4Tracker::addMultiMarker(const std::string& multiFile) 
	{
		MultiMarker* multiMarker = new MultiMarker();//MainAR4_PattList);
		if (!multiMarker->initialise(multiFile))
		{
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	void ARToolKit4Tracker::setThreshold(const int& thresh)	{
		// jcl64: Clamp to 0-255, hse25: use osg func
		if (!gARHandle)
		{	
			osg::notify(osg::FATAL) << "gARHandle empty, please call the init function before accessing members" << endl; 
			exit(-1);
		}

		if (arSetLabelingThresh(gARHandle, osg::clampBetween(thresh,0,255)) < 0)
		{
			osg::notify(osg::WARN) << "ARToolKit4Tracker::setThreshold() : error in arSetLabelingThresh()" << endl;
		}
	}

	int ARToolKit4Tracker::getThreshold()const {
		if (!gARHandle)
		{	osg::notify(osg::FATAL) << "gARHandle empty, please call the init function before accessing members" << endl; exit(-1);}

		return gARHandle->arLabelingThresh;
	}

	unsigned char* ARToolKit4Tracker::getDebugImage() {
		return m_imageptr;//????
	}
		
	void ARToolKit4Tracker::setDebugMode(const bool &d) 
	{
		m_debugmode = d;
		if (!gARHandle)
		{	osg::notify(osg::FATAL) << "gARHandle empty, please call the init function before accessing members" << endl; exit(-1);}

		if (arSetDebugMode(gARHandle, d) < 0)
		{
			osg::notify(osg::FATAL) << "ARToolKit4Tracker::setDebugMode() : error in arSetDebugMode()" << endl;
		}
	}

	bool ARToolKit4Tracker::getDebugMode()const 
	{
		return gARHandle->arDebug;
	}
	

	int ARToolKit4Tracker::ConvertOSGARTPixelFormatToART(PixelFormatType format)const

	{

		switch (format)

		{

			case VIDEOFORMAT_RGB24: return AR_PIXEL_FORMAT_RGB;

			case VIDEOFORMAT_BGR24:	return AR_PIXEL_FORMAT_BGR;

			case VIDEOFORMAT_BGRA32:return AR_PIXEL_FORMAT_BGRA;

			case VIDEOFORMAT_RGBA32:return AR_PIXEL_FORMAT_RGBA;

			case VIDEOFORMAT_ARGB32:return AR_PIXEL_FORMAT_ARGB;

			case VIDEOFORMAT_ABGR32:return AR_PIXEL_FORMAT_ABGR;

			case VIDEOFORMAT_YUV422:return AR_PIXEL_FORMAT_2vuy;

			case VIDEOFORMAT_Y8:

			case VIDEOFORMAT_GREY8:

									return AR_PIXEL_FORMAT_MONO;

			default:

				osg::notify() << "ConvertOSGARTPixelFormatToART() : Unknown pixel format!" << std::endl;

				return 0;

		}        

		return 0;

	}



	PixelFormatType ARToolKit4Tracker::ConvertARTPixelFormatToOSGART(int format)const

	{

		switch (format)

		{

			case AR_PIXEL_FORMAT_RGB : return VIDEOFORMAT_RGB24;

			case AR_PIXEL_FORMAT_BGR : return VIDEOFORMAT_BGR24;

			case AR_PIXEL_FORMAT_BGRA :return VIDEOFORMAT_BGRA32;

			case AR_PIXEL_FORMAT_RGBA :return VIDEOFORMAT_RGBA32;

			case AR_PIXEL_FORMAT_ARGB :return VIDEOFORMAT_ARGB32;

			case AR_PIXEL_FORMAT_ABGR :return VIDEOFORMAT_ABGR32;

			case AR_PIXEL_FORMAT_2vuy :return VIDEOFORMAT_YUV422;

			case AR_PIXEL_FORMAT_MONO :return VIDEOFORMAT_Y8;//or VIDEOFORMAT_GREY8:

			default:

				osg::notify() << "ConvertARTPixelFormatToOSGART() : Unknown pixel format!" << std::endl;

		}        

		return VIDEOFORMAT_ANY;

	}

	 /*virtual*/ 
	void ARToolKit4Tracker::setImageRaw(unsigned char * image, PixelFormatType format)
    {	
		// We are only augmenting method in parent class.
		if (format != m_imageptr_format)

		{

			arSetPixelFormat(gARHandle, ConvertOSGARTPixelFormatToART(format));

			osg::notify() <<  "osgart_artoolkit_tracker::setImageRaw() Incompatible pixelformat! Changing to a compatible format." << endl;

		}

		GenericTracker::setImageRaw(image, format);
	}


	void ARToolKit4Tracker::update()
	{		
		if(m_debugmode)
			osg::notify() << endl << "Start->" << getLabel() << "::update()" << endl;

		__AR_DO_PROFILE( static CL_FUNCT_TRC<CL_TimerVal>	*ThisFct			= this->LocalARTimeTracer->AddFunct		("arDetectMarker_TIME"));		

	    int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;

		AR_BENCH_TIME(ThisFct, 
			// Detect the markers in the video frame.
			if(arDetectMarker(gARHandle, m_imageptr) < 0) 
			{
				std::cerr << "Error detecting markers in image." << std::endl;
				return;
			}
		, m_markerlist.size() 
		, getLabel()
		, gARHandle->marker_num
		);
	
		if (m_debugmode)
			cout << "	arDetectMarker() => Markerdetected = " << gARHandle->marker_num <<endl;

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
				cout << ". ID " << singleMarker->getPatternID() <<endl;
				k = -1;

				for (j = 0; j < gARHandle->marker_num; j++)	
				{
			//		cout << "	try matching candidate pattern :" << gARHandle->markerInfo[j].id<<endl;
					if (singleMarker->getPatternID() == gARHandle->markerInfo[j].id) 
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(gARHandle->markerInfo[j].cf > gARHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
					}
				}
					
				if(k != -1) 
				{			
			//		cout << "		candidate " << k<< " match  "<< currentMarker->getName() <<endl;
					singleMarker->update(&gARHandle->markerInfo[k], gAR3DHandle);
					__AR_DO_PROFILE(RecordMarkerStats(singleMarker, true));
				} 
				else 
				{
					__AR_DO_PROFILE(RecordMarkerStats(singleMarker, false));
				}
			}
			else if (currentMarker->getType() == Marker::ART_MULTI)
			{
				static_cast<MultiMarker*>(currentMarker)->update(this->gAR3DHandle, gARHandle->markerInfo, gARHandle->marker_num);
			}
		}
		if(m_debugmode)
			osg::notify() << "<-Stop" << getLabel() << "::update()" << endl;
	}

	void ARToolKit4Tracker::setProjection(const double n, const double f) 
	{
		//arglCameraFrustum((ARParam*)&m_cparam, n, f, m_projectionMatrix);
		arglCameraFrustumRH((ARParam*)&m_cparam, n, f, m_projectionMatrix);
		/*cout << "arglCameraFrustum : " << endl;
		for (int j = 0; j < 4; j++)	
		{
			cout <<  m_projectionMatrix[j] << " " ;
			cout <<  m_projectionMatrix[j+4] << " " ;
			cout <<  m_projectionMatrix[j+8] << " " ;
			cout <<  m_projectionMatrix[j+12] << " " ;
			cout << endl;
		}*/
#if 0
		arglCameraFrustumRH((ARParam*)&m_cparam, n, f, m_projectionMatrix);
		cout << "arglCameraFrustumRH : " << endl;
		for (int j = 0; j < 16; j+=4)	
		{
			/*
			cout <<  m_projectionMatrix[j] << " " ;
			cout <<  m_projectionMatrix[j+1] << " " ;
			cout <<  m_projectionMatrix[j+2] << " " ;
			cout <<  m_projectionMatrix[j+3] << " " ;
			*/
			cout <<  m_projectionMatrix[j] << " " ;
			cout <<  m_projectionMatrix[j+4] << " " ;
			cout <<  m_projectionMatrix[j+8] << " " ;
			cout <<  m_projectionMatrix[j+12] << " " ;
			cout << endl;
		}
#endif
	}
}; // namespace osgART