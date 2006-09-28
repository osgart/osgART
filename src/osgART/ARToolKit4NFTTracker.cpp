///////////////////////////////////////////////////////////////////////////////
// File name : ARToolKit4NFTTracker.C
//
// Creation : YYY
//
// Version : YYY
//
// Author : Raphael Grasset
//
// email : Raphael.Grasset@imag.fr
//
// Purpose : ??
//
// Distribution :
//
// Use :
//	??
//
// Todo :
//	O ??
//	/
//	X
//
// History :
//	YYY : Mr Grasset : Creation of the file
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// include file
///////////////////////////////////////////////////////////////////////////////
//JENS changed the following line
//#include <ARModif/gsub_lite.h>

#include <AR/gsub_lite.h>
#include <osgART/ARToolKit4NFTTracker>
#include <osgART/SingleART4Marker>
#include <osgART/MultiART4Marker>
#include <osgART/NFTART4Marker>

using namespace std;
using namespace osgART;

///////////////////////////////////////////////////////////////////////////////
// Macro 
///////////////////////////////////////////////////////////////////////////////

#if defined(NO_DEBUG)
#define ASSERT(x)
#else //defined(NO_DEBUG)
#define ASSERT(x) if(!(x)) \
    { cerr << "Assertion failed : (" << #x << ')' << endl \
    << "In file : " << __FILE__ << "at line #" << __LINE__ << endl \
    << "Compiled the " << __DATE__ << " at " << __TIME__ << endl; abort();}
#endif // else defined(NO_DEBUG)

const char* const ARToolKit4NFTTracker_RCS_ID = "@(#)class ARToolKit4NFTTracker definition.";

///////////////////////////////////////////////////////////////////////////////
// class ARToolKit4NFTTracker
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

ARToolKit4NFTTracker::ARToolKit4NFTTracker(void)
{
    threshhold= 100;
	image = NULL;
}

ARToolKit4NFTTracker::~ARToolKit4NFTTracker(void)
{
    
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void 
ARToolKit4NFTTracker::SelfDisplay(ostream& stream) const
{
   stream << "< class ARToolKit4NFTTracker >" << endl;
}

bool 
ARToolKit4NFTTracker::OK(void) const
{
    return true;
}


void
ARToolKit4NFTTracker::init(int xsize,int ysize,char* pattlist_name,char* camera_name)
{
	ARParam  wparam;
    // Set the initial camera parameters.
	cparamName=camera_name;
    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
		std::cerr << "ERROR: Camera parameter load error." << std::endl;
		//exit
    }
    arParamChangeSize(&wparam, xsize, ysize,&cparam);
	std::cout << "*** Camera Parameter ***" << std::endl;
    arParamDisp( &cparam );

	if( (arHandle=arCreateHandle(&cparam)) == NULL ) 
	{
		std::cerr << "Error: arCreateHandle." < std::endl;
        exit(0);
    }
	int pixFormat;
	pixFormat=AR_PIXEL_FORMAT_BGRA;
    if( arSetPixelFormat(arHandle, pixFormat) < 0 ) 
	{
        std::cerr << "Error: arSetPixelFormat." < std::endl;
        exit(0);
    }
	if( arSetDebugMode(arHandle, AR_DEBUG_ENABLE) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." < std::endl;
        exit(0);
    }
	if( arSetLabelingThresh(arHandle,threshhold) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." < std::endl;
        exit(0);
    }

	arSetMarkerExtractionMode( arHandle, AR_NOUSE_TRACKING_HISTORY );

    if( (ar3DHandle=ar3DCreateHandle(&cparam)) == NULL ) 
	{
         std::cerr << "Error: ar3DCreateHandle." < std::endl;
        exit(0);
    }
    if( (arPattHandle=arPattCreateHandle()) == NULL ) 
	{
         std::cerr << "Error: arPattCreateHandle." < std::endl;
        exit(0);
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
	pattListName=pattlist_name;
	SetupMarkers();
	
}

ARParam
ARToolKit4NFTTracker::getIntrinsicParameters()
{
	return cparam;
}

void
ARToolKit4NFTTracker::SetupMarkers()
{
	std::ifstream markerFile;

	markerFile.open(pattListName.c_str());

	markerFile>>patternNum;


	for (int i=0;i<patternNum;i++)
	{
		std::string namepatt;
		std::string patttype;

		markerFile>>namepatt;
		markerFile>>patttype;

		if (patttype=="SINGLE")
		{
			double width;
			double centre[2];
			markerFile>>width;
			markerFile>>centre[0];
			markerFile>>centre[1];
			addSingleMarker(namepatt,width, centre[0],centre[1]);
		}
		else
		{
			if (patttype=="MULTI")
				addMultiMarker(namepatt);
			else
				if (patttype=="NFT")
				{
					cout << "initialized "<< addNFTMarker(namepatt) + 1 << " NFT markers" << endl;
				}
		}
	}
	arPattAttach( arHandle, arPattHandle );
}

int 
ARToolKit4NFTTracker::addSingleMarker(string pattFile, double width, double centerX, double centerY) {

	Marker* singleMarker = new SingleART4Marker();
	
	if (!static_cast<SingleART4Marker*>(singleMarker)->initialise(arPattHandle,(char*)pattFile.c_str(), width, centerX, centerY))
	{
		delete singleMarker;
		return -1;
	}

	pattList.push_back(singleMarker);

	return pattList.size() - 1;
}

int 
ARToolKit4NFTTracker::addMultiMarker(string multiFile) 
{
	Marker* multiMarker = new MultiART4Marker();

	if (!static_cast<MultiART4Marker*>(multiMarker)->initialise((char*)multiFile.c_str()))
	{
		delete multiMarker;
		return -1;
	}
	
	pattList.push_back(multiMarker);

	return pattList.size() - 1;
}

int 
ARToolKit4NFTTracker::addNFTMarker(string nftFile) 
{
	//JENS
	// changes: creating ART4Markers for all surfaces of surface set in here
	// instead of adding only one to pattList
	// original code below

	// read surfaces and markers from file
	surfaceSet = ar2ReadSurfaceSet((char*)nftFile.c_str(),arPattHandle);
    if( surfaceSet == NULL ) exit(0);
	
	
	for (int i = 0; i < surfaceSet->num; i++){

		Marker* nftMarker = new NFTART4Marker();
		if (!static_cast<NFTART4Marker*>(nftMarker)->initialise()){
			delete nftMarker;
			break;
		}
		else {
			pattList.push_back(nftMarker);
		}

	}
	
	// check if all markers got initialized
	if (pattList.size() != surfaceSet->num ){
		// delete all nftmarkers
		for (int j = 0; j < pattList.size(); j++){
			delete (static_cast<NFTART4Marker*>(pattList[j]));
		}
		// empty pattern list
		pattList.clear();
		return -1;
	}
	
	return pattList.size() - 1;
	
	
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

void
ARToolKit4NFTTracker::setThreshhold(int thresh)
{
	threshhold=thresh;
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
    int             j, k;
	static int confF;

	//JENS
	static int active_surface = 0;

	// declare all markers as not beeing updated nor valid
	for (vector<Marker*>::const_iterator iter=pattList.begin();iter!=pattList.end();iter++)		
	{
				Marker* currentMarker=(*iter);
//				currentMarker->m_updated=false;
//				currentMarker->m_valid=false;				
	}
	
	if (image!=NULL)
	{
			//NFT TRACKING
			static double		patt_trans1[3][4]; 
			static double		patt_trans2[3][4]; 
			static double		patt_trans3[3][4];
			static int			contF = 0; 
		    double				new_trans[3][4];
			double				err;
			int i,j;
			int mode;			//JENS this variable seems to be a leftover to me, doesn't do no harm, may be removed, check later

			// try NFT
			if( ((contF == 1 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, image, patt_trans1, NULL, NULL, new_trans, &err) == 0)
			|| (contF == 2 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, image, patt_trans1, patt_trans2, NULL, new_trans, &err) == 0)
			|| (contF == 3 && ar2Tracking_Jens(active_surface, ar2Handle, surfaceSet, image, patt_trans1, patt_trans2, patt_trans3, new_trans, &err) == 0))
			&& err < 10.0 ) {
				std::cerr<<"features found.."<<std::endl;
				for( j = 0; j < 3; j++ ) {
					for( i = 0; i < 4; i++ ) patt_trans3[j][i] = patt_trans2[j][i];
				}
				for( j = 0; j < 3; j++ ) {
					for( i = 0; i < 4; i++ ) patt_trans2[j][i] = patt_trans1[j][i];
				}
				for( j = 0; j < 3; j++ ) {
					for( i = 0; i < 4; i++ ) patt_trans1[j][i] = new_trans[j][i];
				}
				contF++;
				if( contF > 3) contF = 3;
				mode = 1;
			}
			// NFT failed, try normal marker tracking
			else {
				if( (active_surface = square_tracking(image, patt_trans1 )) < 0 ){
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
				(static_cast<NFTART4Marker*>(pattList[active_surface]))->update(patt_trans1);
				//(static_cast<NFTART4Marker*>(pattList[active_surface]))->m_valid=true;
			}
			else
			{
				(static_cast<NFTART4Marker*>(pattList[active_surface]))->update(NULL);
			}
			// tell nodes about update
			for (vector<Marker*>::const_iterator iter=pattList.begin();iter!=pattList.end();iter++)		
			{
				Marker* currentMarker=(*iter);
			//	currentMarker->m_updated=true;			
			}
	}
}

void ARToolKit4NFTTracker::ARTransToGL(double para[3][4], double gl[16]) {
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 4; i++) {
            gl[i*4+j] = para[j][i];
        }
    }
    gl[0*4+3] = gl[1*4+3] = gl[2*4+3] = 0.0;
    gl[3*4+3] = 1.0;
}

void ARToolKit4NFTTracker::setProjection(const double n, const double f) {

	/*double   icpara[3][4];
    double   trans[3][4];
    double   p[3][3], q[4][4];

	ARParam	param  = cparam;
	
	for (int i = 0; i < 4; i++) {
        param.mat[1][i] = (param.ysize - 1)*(param.mat[2][i]) - param.mat[1][i];
    }

    if (arParamDecompMat(param.mat, icpara, trans) < 0) {
        printf("Parameter error while constructing projection matrix\n");
        return;
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            p[i][j] = icpara[i][j] / icpara[2][2];
        }
    }
    q[0][0] = (2.0 * p[0][0] / param.xsize);
    q[0][1] = (2.0 * p[0][1] / param.xsize);
    q[0][2] = (2.0 * p[0][2] / param.xsize)  - 1.0;
    q[0][3] = 0.0;

    q[1][0] = 0.0;
    q[1][1] = (2.0 * p[1][1] / param.ysize);
    q[1][2] = (2.0 * p[1][2] / param.ysize) - 1.0;
    q[1][3] = 0.0;

    q[2][0] = 0.0;
    q[2][1] = 0.0;
    q[2][2] = (f + n)/(f - n);
    q[2][3] = -2.0 * f * n / (f - n);

    q[3][0] = 0.0;
    q[3][1] = 0.0;
    q[3][2] = 1.0;
    q[3][3] = 0.0;*/

	arglCameraFrustum(&cparam,n,f,m_projectionMatrix);


/* 
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            projectionMatrix[i+j*4] = q[i][0] * trans[0][j]
			+ q[i][1] * trans[1][j]
			+ q[i][2] * trans[2][j];
        }
        projectionMatrix[i+3*4] = q[i][0] * trans[0][3]
		+ q[i][1] * trans[1][3]
		+ q[i][2] * trans[2][3]
		+ q[i][3];
    }*/
}


///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////