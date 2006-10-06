#include "ARToolKit4NFTTracker"

#include "SingleMarker"
#include "MultiMarker"
#include "NFTMarker"

#include <iostream>
#include <fstream>

typedef int AR_PIXEL_FORMAT;
#include <AR/gsub_lite.h>

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
		surfaceSet(0L)
{
	// attach a new field to the name "threshold"
	m_fields["threshold"] = new TypedField<int>(&threshold);

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

	if( arSetDebugMode(arHandle, AR_DEBUG_ENABLE) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." << std::endl;
        return false;
    }

	if( arSetLabelingThresh(arHandle,threshold) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." << std::endl;
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
	int	matchingImageMode = AR2_MATCHING_FIELD_IMAGE;
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

	std::cerr<<"pattern list.."<<patternNum<<std::endl;

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
			std::cerr<<"new single.."<<std::endl;
			if (addSingleMarker(patternName, width, center) == -1) {
				std::cerr << "Error adding single pattern: " << patternName << std::endl;
				ret = false;
				break;
			}
		}
		else if (patternType == "MULTI")
		{
			std::cerr<<"new multi.."<<std::endl;
			if (addMultiMarker(patternName) == -1) {
				std::cerr << "Error adding multi-marker pattern: " << patternName << std::endl;
				ret = false;
				break;
			}
		}
		else if (patternType == "NFT")
		{
			std::cerr<<"new nft.."<<std::endl;
			if (addNFTMarker(patternName) == -1) {
				std::cerr << "Error adding nft-marker pattern: " << patternName << std::endl;
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

	arPattAttach( arHandle, arPattHandle );

	std::cerr<<"Finish setup patterns.."<<std::endl;
	markerFile.close();

	return ret;
	
}

int 
ARToolKit4NFTTracker::addSingleMarker(const std::string& pattFile, 
									  double width, double center[2])
{

	Marker* singleMarker = new osgART::SingleMarker();
	
	if (!static_cast<osgART::SingleMarker*>(singleMarker)->initialise(arPattHandle,(char*)pattFile.c_str(), width, center))
	{
		//delete singleMarker;
		return -1;
	}

	m_markerlist.push_back(singleMarker);

	return m_markerlist.size() - 1;
}

int 
ARToolKit4NFTTracker::addMultiMarker(const std::string& multiFile) 
{
	Marker* multiMarker = new MultiMarker();

	if (!static_cast<MultiMarker*>(multiMarker)->initialise((char*)multiFile.c_str()))
	{
		//delete multiMarker;
		return -1;
	}
	
	m_markerlist.push_back(multiMarker);

	return m_markerlist.size() - 1;
}

int 
ARToolKit4NFTTracker::addNFTMarker(const std::string& nftFile) 
{
	//JENS
	// changes: creating ART4Markers for all surfaces of surface set in here
	// instead of adding only one to pattList
	// original code below

	// read surfaces and markers from file

	surfaceSet = ar2ReadSurfaceSet((char*)nftFile.c_str(),arPattHandle);
    if( surfaceSet == NULL ) 
	{
		std::cerr<<"ca chie la colle.."<<std::endl;
		exit(0);
	}
	
	for (int i = 0; i < surfaceSet->num; i++) {

		Marker* nftMarker = new NFTMarker();
		if (!static_cast<NFTMarker*>(nftMarker)->initialise()){
				std::cerr<<"ca initialize mal la colle.."<<std::endl;
			//delete nftMarker;
			break;
		}
		else {
			m_markerlist.push_back(nftMarker);
		}

	}
	
	// check if all markers got initialized
	/*
	if (m_markerlist.size() != surfaceSet->num ){
			std::cerr<<"ca a pas marche mal la colle.."<<std::endl;
		// delete all nftmarkers
		for (int j = 0; j < m_markerlist.size(); j++){
			//delete (static_cast<NFTMarker*>(m_markerlist[j].get()));
		}
		// empty pattern list
		m_markerlist.clear();
		return -1;
	}
	*/
	return m_markerlist.size() - 1;
	
	
	//JENS left the original code in here

	//ART4Marker* nftMarker = new NFTART4Marker();
	//
	//// read surfaces and markers from file
	//surfaceSet = ar2ReadSurfaceSet((char*)nftFile.c_str(),arPattHandle);
 //   if( surfaceSet == NULL ) exit(0);
	//	
	//if (!static_cast<NFTART4Marker*>(nftMarker)->initialise())
	//{
	//	delete nftMarker;
	//	return -1;
	//}
	//pattList.push_back(nftMarker);
	//return pattList.size() - 1;
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
 
	// detect visible marker
	arDetectMarker(arHandle, dataPtr);

    // check for object visibility, i.e. which marker is visible
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

void
ARToolKit4NFTTracker::update()
{	

	//JENS
	static int active_surface = 0;

	// declare all markers as not beeing updated nor valid
	//for (MarkerList::iterator iter = m_markerlist.begin(); 
	//		iter != m_markerlist.end(); 
	//		iter++)	
	//{
	//			Marker* currentMarker=(*iter);
//				currentMarker->m_updated=false;
//				currentMarker->m_valid=false;				
	//}
	
	// Do not update with a null image
	if (m_imageptr == NULL) return;

		//NFT TRACKING
	static double		patt_trans1[3][4]; 
	static double		patt_trans2[3][4]; 
	static double		patt_trans3[3][4];
	static int			contF = 0; 
	double				new_trans[3][4];
	double				err;
	int mode;			//JENS this variable seems to be a leftover to me, doesn't do no harm, may be removed, check later

		// try NFT
		if( ((contF == 1 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, m_imageptr, patt_trans1, NULL, NULL, new_trans, &err) == 0)
		|| (contF == 2 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, m_imageptr, patt_trans1, patt_trans2, NULL, new_trans, &err) == 0)
		|| (contF == 3 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, m_imageptr, patt_trans1, patt_trans2, patt_trans3, new_trans, &err) == 0))
		&& err < 10.0 ) {
			std::cerr<<"features found.."<<std::endl;
			for(int  j = 0; j < 3; j++ ) {
				for(int  i = 0; i < 4; i++ ) patt_trans3[j][i] = patt_trans2[j][i];
			}
			for(int  j = 0; j < 3; j++ ) {
				for(int  i = 0; i < 4; i++ ) patt_trans2[j][i] = patt_trans1[j][i];
			}
			for(int  j = 0; j < 3; j++ ) {
				for(int  i = 0; i < 4; i++ ) patt_trans1[j][i] = new_trans[j][i];
			}
			contF++;
			if( contF > 3) contF = 3;
			mode = 1;
		}
		// NFT failed, try normal marker tracking
		else {
			if( (active_surface = square_tracking(m_imageptr, patt_trans1 )) < 0 ){
				contF = 0;
			}
			else {
				std::cerr<<"marker found on surface "<< active_surface <<std::endl;
				contF = 1;
				mode = 2;
			}
		}
					
		// apply transformation matrix and activate the right pattern
		if (active_surface >= 0){

			(static_cast<NFTMarker*>(m_markerlist[active_surface].get()))->update(patt_trans1);
			//(static_cast<NFTART4Marker*>(pattList[active_surface]))->m_valid=true;
		}
		else
		{
			(static_cast<NFTMarker*>(m_markerlist[active_surface].get()))->update(NULL);
		}
		// tell nodes about update
		//for (MarkerList::iterator iter = m_markerlist.begin(); 
		//	iter != m_markerlist.end(); 
		//	iter++)		
		//{
		//	Marker* currentMarker=(*iter);
		//	currentMarker->m_updated=true;			
		//}

}


void ARToolKit4NFTTracker::setProjection(const double n, const double f) {
	arglCameraFrustumRH((ARParam*)&cparam, n, f, m_projectionMatrix);
}
};