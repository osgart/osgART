/*
 *	osgART/Tracker/ARToolKit4NFT/ARToolKit4NFTTracker
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

typedef int AR_PIXEL_FORMAT;

#include "ARToolKit4NFTTracker"

#include "SingleMarker"
#include "MultiMarker"
#include "NFTMarker"

#include <iostream>
#include <fstream>

#include <osg/Notify>

//#include <AR/gsub.h>
#include <AR/gsub_lite.h>


#include <osgART/PluginManager>

// initializer for dynamic loading
osgART::PluginProxy<osgART::ARToolKit4NFTTracker> g_artoolkitnft("tracker_artoolkit4nft");


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

/* JENS MODIF
.dat configuration file format

	First integer in File does not represent the number of markers to be read, now represents the number of entries in the file.
	I.e. NFT image sets count as one entry, disregarding how many surfaces and markers they come with
	-> For single marker applications nothing has changed



ARToolKit4Tracker.cpp

	ARToolKit4Tracker::addNFTMarker

		Now reading all surfaces and related markers at once,
		also initializes all related ARTNFTMarkers at the same time
	
		Needs to be called only once per surface Set


	ARToolKit4Tracker::square_tracking

		Now returns index of currently tracked surface or -1 on errors


	ARToolKit4Tracker::update

		Introduced const int active_surface, holding index of currently tracked surface or -1 if no surface has been identified
		Using this info - the computed transformation matrix is copied to the right OSG node, i.e. model
						- the right OSG node is activated for rendering

*/

namespace osgART {

ARToolKit4NFTTracker::ARToolKit4NFTTracker() : GenericTracker(),
		threshold(100),
		arHandle(0L),
		arPattHandle(0L),
		ar3DHandle(0L),
		ar2Handle(0L),
		surfaceSet(0L),
		_useNFT(true)
{
	// attach a new field to the name "threshold"
	m_fields["threshold"] = new TypedField<int>(&threshold);
	m_fields["enableNFT"] = new TypedField<bool>(&_useNFT);

	//image = NULL;
}

ARToolKit4NFTTracker::~ARToolKit4NFTTracker()
{
    
}


bool
ARToolKit4NFTTracker::init(int xsize,int ysize,
						   const std::string& pattlist_name,const std::string& camera_name)
{
	ARParam  wparam;

    // Set the initial camera parameters.

    if(arParamLoad((char*)camera_name.c_str(), 1, &wparam) < 0) {
		std::cerr << "ERROR: Camera parameter load error." << std::endl;
		return false;
    }
    arParamChangeSize(&wparam, xsize, ysize,&cparam);
	std::cout << "*** Camera Parameter ***" << std::endl;
    arParamDisp( &cparam );

	if( (arHandle=arCreateHandle(&cparam)) == NULL ) 
	{
		std::cerr << "ERROR: arCreateHandle." << std::endl;
        return false;
    }

	int pixFormat;
	pixFormat=AR_PIXEL_FORMAT_BGRA;
    if( arSetPixelFormat(arHandle, pixFormat) < 0 ) 
	{
        std::cerr << "Error: arSetPixelFormat." << std::endl;
        return false;
    }

	if( arSetDebugMode(arHandle, AR_DEBUG_DISABLE) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." << std::endl;
        return false;
    }

	if( arSetLabelingThresh(arHandle,threshold) < 0 ) 
	{
        std::cerr << "Error: arSetLabelingThresh." << std::endl;
        return false;
    }

	arSetMarkerExtractionMode( arHandle, AR_NOUSE_TRACKING_HISTORY );

    if( (ar3DHandle=ar3DCreateHandle(&cparam)) == NULL ) 
	{
         std::cerr << "Error: ar3DCreateHandle." << std::endl;
         return false;
    }
    if( (arPattHandle=arPattCreateHandle()) == NULL ) 
	{
         std::cerr << "Error: arPattCreateHandle." << std::endl;
         return false;
    }
	setProjection(10.0f, 10000.0f);

	//INIT NFT
	int	matchingImageMode = AR2_MATCHING_FRAME_IMAGE;
	int	matchingMethod    = AR2_MATCHING_FINE;
	int	debugMode         = 0;
    
	ar2Handle = ar2CreateHandle( &cparam, pixFormat );

	ar2ChangeMacthingImageMode( ar2Handle, matchingImageMode );

	ar2ChangeMacthingMethod( ar2Handle, matchingMethod );

	ar2ChangeDebugMode( ar2Handle, debugMode );

	//arFittingMode   = AR_FITTING_TO_IDEAL;
	//arImageProcMode = AR_IMAGE_PROC_IN_FULL;
	if (!setupMarkers(pattlist_name)) {
		std::cerr << "ERROR: Marker setup failed." << std::endl;
		return false;
	}	

	return true;
}

std::string trim(std::string& s,const std::string& drop = " ")
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}

bool 
ARToolKit4NFTTracker::setupMarkers(const std::string& patternListFile)
{
	std::ifstream markerFile;

	markerFile.open(patternListFile.c_str());

	// Need to check for error when opening file
	if (!markerFile.is_open()) return false;

	bool ret = true;

	int patternNum = 0;
	markerFile>>patternNum;

	std::string patternName, patternType;

	std::cerr<<"ARToolKit4NFTTracker::setupMarkers:/ Number of markers: "<<patternNum<<std::endl;

	for (int i = 0; i < patternNum; i++) {

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
			std::cerr<<"ARToolKit4NFTTracker::setupMarkers:/ adding SingleMarker: ";
			std::cerr << patternName << " " << width << " " << center[0] << "x" <<center[1] << std::endl;
			if (addSingleMarker(patternName, width, center) == -1) {
				std::cerr << "ARToolKit4NFTTracker::setupMarkers:/ Error adding SingleMarker: " << patternName << std::endl;
				ret = false;
				break;
			}
		}
		else if (patternType == "MULTI")
		{
			std::cerr<<"ARToolKit4NFTTracker::setupMarkers:/ adding MultiMarker: "<<std::endl;
			std::cerr << patternName << std::endl;
			if (addMultiMarker(patternName) == -1) {
				std::cerr << "ARToolKit4NFTTracker::setupMarkers:/ Error adding MultiMarker: " << patternName << std::endl;
				ret = false;
				break;
			}
		}
		else if (patternType == "NFT")
		{
			std::cerr<<"ARToolKit4NFTTracker::setupMarkers:/ adding NFTMarker: "<<std::endl;
			std::cerr << patternName << std::endl;
			if (addNFTMarker(patternName) == -1) {
				std::cerr << "ARToolKit4NFTTracker::setupMarkers:/ Error adding nft-marker pattern: " << patternName << std::endl;
				ret = false;
				break;
			}
			std::cerr << std::endl;
		}
		else 
		{
			std::cerr <<"ARToolKit4NFTTracker::setupMarkers:/ Unrecognized marker type: " << patternType << std::endl;
			ret = false;
			break;
		}
	}

	arPattAttach( arHandle, arPattHandle );

	std::cerr<<"ARToolKit4NFTTracker::setupMarkers:/ Marker setup finished."<<std::endl;
	markerFile.close();

	return ret;
	
}

int 
ARToolKit4NFTTracker::addSingleMarker(const std::string& pattFile, 
									  double width, double center[2])
{

	osgART::SingleMarker* singleMarker = new osgART::SingleMarker();
	
	if (!singleMarker->initialise(arPattHandle,(char*)pattFile.c_str(), width, center))
	{
		singleMarker->unref();
		return -1;
	}

	m_markerlist.push_back(singleMarker);

	return m_markerlist.size() - 1;
}

int 
ARToolKit4NFTTracker::addMultiMarker(const std::string& multiFile) 
{
	osgART::MultiMarker* multiMarker = new osgART::MultiMarker();

	if (!multiMarker->initialise((char*)multiFile.c_str()))
	{
		multiMarker->unref();
		return -1;
	}
	
	m_markerlist.push_back(multiMarker);

	return m_markerlist.size() - 1;
}

int 
ARToolKit4NFTTracker::addNFTMarker(const std::string& nftFile) 
{
	// read surfaces and markers from file

	surfaceSet = ar2ReadSurfaceSet((char*)nftFile.c_str(),arPattHandle);
    if( surfaceSet == NULL ) 
	{
		std::cerr<<"ARToolKit4NFTTracker::addNFTMarker:/ error reading surface set from file: " << nftFile.c_str() <<std::endl;
		exit(0);
	}
	
	for (int i = 0; i < surfaceSet->num; i++) {

		NFTMarker* nftMarker = new NFTMarker();
		
		if (!nftMarker->initialise())
		{	
			std::cerr<<"ARToolKit4NFTTracker::addNFTMarker:/ error initializing NFTMarker, ->unref()"<<std::endl;			
			nftMarker->unref();

			break;
		}
		else 
		{
			m_markerlist.push_back(nftMarker);
		}

	}
	return m_markerlist.size() - 1;
}

void ARToolKit4NFTTracker::setThreshold(int thresh)	{
		// jcl64: Clamp to 0-255, hse25: use osg func
		threshold = osg::clampBetween(thresh,0,255);		
	}

int ARToolKit4NFTTracker::getThreshold() {
		return threshold;
}


int 
ARToolKit4NFTTracker::square_tracking( ARUint8 *dataPtr, double patt_trans1[3][4] )
{
	
    double          center[2] = { 0.0, 0.0 };
    double          wtrans[3][4];
    double          wtrans2[3][4];
	double			err;
    int             i, j, k, l, m, n; 

    // check if currently visible marker belongs to one of the surfaces
    k = -1; 
    for( j = 0; j < arHandle->marker_num; j++ ) {
		for( m = 0; m < surfaceSet->num; m++ ) {
            for( i = 0; i < surfaceSet->surface[m].markerSet->num; i++ ) {
                if( arHandle->markerInfo[j].id == surfaceSet->surface[m].markerSet->marker[i].pattId ) {
                    if( k == -1 ) { k = j; l = i; n = m; }
                    else if( arHandle->markerInfo[k].cf < arHandle->markerInfo[j].cf ) { k = j; l = i; n = m; }
				}
			}
		}
	}
    if( k == -1 ) {		// if no marker is detected, return -1
        return -1;
	}
	// k is index of marker in arHandle that equals marker with index l in surface set
	// n is number of surface in which marker has been found
	
    // get the transformation between the marker and the real camera
    err = arGetTransMatSquare(ar3DHandle, &(arHandle->markerInfo[k]), surfaceSet->surface[n].markerSet->marker[l].width, wtrans);
	if( err > 10.0 ) return -1;
    arUtilMatMul( wtrans, surfaceSet->surface[n].markerSet->marker[l].transI2M, wtrans2 );
    arUtilMatMul( wtrans2, surfaceSet->surface[n].itrans, patt_trans1 );

    return n; // return index of currently tracked surface	
}

void ARToolKit4NFTTracker::updateStandardTracker()
{
	int j, k;
	// browse through list of markers
	for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter != m_markerlist.end(); 
			iter++)		
		{
			Marker* currentMarker = (*iter).get();

			if (currentMarker->getType() == Marker::ART_SINGLE)
			{
				SingleMarker* singleMarker = static_cast<SingleMarker*>(currentMarker);
				
				// get markerInfo for each marker
				k = -1;
				for (j = 0; j < arHandle->marker_num; j++)	
				{
					if (singleMarker->getPatternID() == arHandle->markerInfo[j].id) 
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(arHandle->markerInfo[j].cf > arHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
					}
				}
				// markerInfo for current marker found -> update transf. matrix
				if(k != -1) 
				{
					//std::cerr<<"Found single marker with ID: " << singleMarker->getPatternID() <<std::endl;
					singleMarker->update(ar3DHandle,&arHandle->markerInfo[k]);
				} 
				else 
				{
					singleMarker->update(ar3DHandle,NULL);
				}
			}
		}
}


void
ARToolKit4NFTTracker::updateNFTTracker(ARUint8* imageptr)
{	
	// reset active surface to none active
	static int active_surface = -1;
		
	static double		patt_trans1[3][4]; 
	static double		patt_trans2[3][4]; 
	static double		patt_trans3[3][4];
	static int			contF = 0; 
	double				new_trans[3][4];
	double				err;

	MarkerList::iterator iter = m_markerlist.begin();
	// declare all nft-markers as not valid
	while (iter != m_markerlist.end())
	{
		if (NFTMarker* nm = dynamic_cast<NFTMarker*>((*iter).get()))
			nm->update(NULL);

		iter++;		
	}

	// this allows to enable/disable nft tracking using field mechanism
	if ( _useNFT) {
		// try NFT tracking
		if( ((contF == 1 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, imageptr, patt_trans1, NULL, NULL, new_trans, &err) == 0)
			|| (contF == 2 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, imageptr, patt_trans1, patt_trans2, NULL, new_trans, &err) == 0)
			|| (contF == 3 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, imageptr, patt_trans1, patt_trans2, patt_trans3, new_trans, &err) == 0))
			&& err < 10.0)
		{
			//std::cout << "ARToolKit4NFTTracker::updateNFTTracker:/ here2" << std::endl;
			//std::cerr<<"features found.."<<std::endl;
			for(int  j = 0; j < 3; j++ ) {
				for(int  i = 0; i < 4; i++ ) patt_trans3[j][i] = patt_trans2[j][i];
				for(int  i = 0; i < 4; i++ ) patt_trans2[j][i] = patt_trans1[j][i];
				for(int  i = 0; i < 4; i++ ) patt_trans1[j][i] = new_trans[j][i];
			}
			contF++;
			if( contF > 3) contF = 3;
		}
		// NFT failed, try normal marker tracking
		else {
			//std::cout << "ARToolKit4NFTTracker::updateNFTTracker:/ here3" << std::endl;

			if( (active_surface = square_tracking(imageptr, patt_trans1 )) < 0 ){
				contF = 0;
			}
			else {
				//std::cerr<<"ARToolKit4NFTTracker::updateNFTTracker:/ marker found on surface "<< active_surface <<std::endl;
				contF = 1;	
			}
		}
	}
	// nft was disabled via _useNFT using field mechanism
	else{
		if( (active_surface = square_tracking(imageptr, patt_trans1 )) < 0 ){
			contF = 0;
		}
		else {
			//std::cerr<<"ARToolKit4NFTTracker::updateNFTTracker:/ marker found on surface "<< active_surface <<std::endl;
			contF = 1;	
		}
	}
	
// apply transformation matrix and activate the right pattern
	if (active_surface >= 0)
		(static_cast<NFTMarker*>(m_markerlist[active_surface].get()))->update(patt_trans1);
}


void
ARToolKit4NFTTracker::update()
{	

	if (!m_imagesource.valid())
	{
		osg::notify(osg::WARN) << "No connected image source for the tracker" << std::endl;
		return;
	}

	// Do not update with a null image.
	if (!m_imagesource->valid())
	{
		osg::notify(osg::WARN) << "osgart_artoolkit_tracker: received NULL pointer as image" << std::endl;
		return;
	}

	// hse25: performance measurement: only update if the image was modified
	if (m_imagesource->getModifiedCount() == m_lastModifiedCount)
	{
		return; 
	}

	// update internal modified count
	m_lastModifiedCount = m_imagesource->getModifiedCount();

	//arSetPixelFormat(arHandle, getARPixelFormatForImage(*m_imagesource.get()));

	// detect visible markers
	arDetectMarker(arHandle,m_imagesource->data());
	
	// update single and multiple artoolkit markers
	updateStandardTracker();
	
	// update nft
	updateNFTTracker(m_imagesource->data());
}


void ARToolKit4NFTTracker::setProjection(const double n, const double f) 
{
	arglCameraFrustumRH((ARParam*)&cparam, n, f, m_projectionMatrix);
}



int ARToolKit4NFTTracker::getARPixelFormatForImage(const osg::Image& _image) const
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

bool ARToolKit4NFTTracker::getNFTOn(){
	return _useNFT;
}

};