#include "ARToolKitTracker"

#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif

#include "SingleMarker"
#include "MultiMarker"

#include <osgART/GenericVideo>

#include <iostream>
#include <fstream>


namespace osgART {

	ARToolKitTracker::ARToolKitTracker() : GenericTracker(),
		threshold(100),
		m_debugMode(false)
	{
		// attach a new field to the name "threshold"
		m_fields["threshold"] = new TypedField<int>(&threshold);
		// attach a new field to the name "debug"
		m_fields["debug"] = new TypedField<bool>(&m_debugMode);
	}

	ARToolKitTracker::~ARToolKitTracker()
	{
	}


	bool ARToolKitTracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{
		ARParam  wparam;
		
	    // Set the initial camera parameters.
		cparamName = camera_name;
	    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
			std::cerr << "ERROR: Camera parameter load error." << std::endl;
			return false;
	    }

	    arParamChangeSize(&wparam, xsize, ysize,(ARParam*)&cparam);
	    arInitCparam((ARParam*)&cparam);
	    arParamDisp((ARParam*)&cparam);

		arFittingMode = AR_FITTING_TO_IDEAL;
	    arImageProcMode = AR_IMAGE_PROC_IN_FULL;

		setProjection(10.0f, 8000.0f);
		// setDebugMode(false);
		// setThreshold(100);


		if (!setupMarkers(pattlist_name)) {
			std::cerr << "ERROR: Marker setup failed." << std::endl;
			return false;
		}

		// Success
		return true;
	}


std::string trim(std::string& s,const std::string& drop = " ")
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}


	bool ARToolKitTracker::setupMarkers(const std::string& patternListFile)
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

	int ARToolKitTracker::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		Marker* singleMarker = new SingleMarker();
		if (!static_cast<SingleMarker*>(singleMarker)->initialise(pattFile, width, center))
		{
			// delete singleMarker;
			return -1;
		}

		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;

	}

	int ARToolKitTracker::addMultiMarker(const std::string& multiFile) 
	{
		Marker* multiMarker = new MultiMarker();
		
		if (!static_cast<MultiMarker*>(multiMarker)->initialise(multiFile))
		{
			// delete multiMarker;
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	void ARToolKitTracker::setThreshold(int thresh)	{
		// jcl64: Clamp to 0-255, hse25: use osg func
		threshold = osg::clampBetween(thresh,0,255);		
	}

	int ARToolKitTracker::getThreshold() {
		return threshold;
	}


	unsigned char* ARToolKitTracker::getDebugImage() {
		return arImage;
	}
		
	void ARToolKitTracker::setDebugMode(bool d) 
	{
		m_debugMode = d;
		if (m_debugMode) arDebug = 1;
		else arDebug = 0;
	}

	bool ARToolKitTracker::getDebugMode() 
	{
		return m_debugMode;
	}


	void ARToolKitTracker::update()
	{	

		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		int             marker_num;						// Count of number of markers detected.
	    int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;


		unsigned int _artoolkit_pixsize = 0;

		// hse25: this is really a dumb sanity check
		switch (this->m_imageptr_format) {
			case VIDEOFORMAT_RGB24:
			case VIDEOFORMAT_BGR24:
				_artoolkit_pixsize = 3;
				break;
			case VIDEOFORMAT_BGRA32:
			case VIDEOFORMAT_RGBA32:
			case VIDEOFORMAT_ARGB32:
			case VIDEOFORMAT_ABGR32:
				_artoolkit_pixsize = 4;
				break;
			case VIDEOFORMAT_YUV422:
			case VIDEOFORMAT_YUV422P:
				_artoolkit_pixsize = 2;
				break;
			case VIDEOFORMAT_GREY8:
				_artoolkit_pixsize = 1;
				break;
				// please fill out the rest!
			default:
                break;
		}

		if (AR_PIX_SIZE_DEFAULT != _artoolkit_pixsize) {
			std::cerr << "osgart_artoolkit_tracker::update() Incompatible pixelformat!" << std::endl;
			return;
		}

		// Detect the markers in the video frame.
		if(arDetectMarker(m_imageptr, threshold, &marker_info, &marker_num) < 0) 
		{
			std::cerr << "Error detecting markers in image." << std::endl;
			return;
		}
			
			
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter != m_markerlist.end(); 
			iter++)		
		{
			
			Marker* currentMarker = (*iter).get();

			if (currentMarker->getType() == Marker::ART_SINGLE)
			{

				SingleMarker* singleMarker = static_cast<SingleMarker*>(currentMarker);

				k = -1;
				for (j = 0; j < marker_num; j++)	
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
				} 
				else 
				{
					singleMarker->update(NULL);
				}
			}
			else if (currentMarker->getType() == Marker::ART_MULTI)
			{
				static_cast<MultiMarker*>(currentMarker)->update(marker_info, marker_num);
				
			}
		}
	}

	void ARToolKitTracker::setProjection(const double n, const double f) {
		arglCameraFrustumRH((ARParam*)&cparam, n, f, m_projectionMatrix);
	}

}; // namespace osgART
