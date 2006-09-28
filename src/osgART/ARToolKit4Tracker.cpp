///////////////////////////////////////////////////////////////////////////////
// File name : ARToolKit4Tracker.C
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
#include <osgART/ARToolKit4Tracker>
#include <osgART/SingleART4Marker>
#include <osgART/MultiART4Marker>

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

const char* const ARToolKit4Tracker_RCS_ID = "@(#)class ARToolKit4Tracker definition.";

///////////////////////////////////////////////////////////////////////////////
// class ARToolKit4Tracker
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

ARToolKit4Tracker::ARToolKit4Tracker(void)
{
    threshhold= 100;
	image = NULL;
}

ARToolKit4Tracker::~ARToolKit4Tracker(void)
{
    
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void 
ARToolKit4Tracker::SelfDisplay(ostream& stream) const
{
   stream << "< class ARToolKit4Tracker >" << endl;
}

bool 
ARToolKit4Tracker::OK(void) const
{
    return true;
}


void
ARToolKit4Tracker::init(int xsize,int ysize,char* pattlist_name,char* camera_name)
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
		std::cerr << "Error: arCreateHandle." << std::endl;
        exit(0);
    }
	int pixFormat;
	pixFormat=AR_PIXEL_FORMAT_BGRA;
    if( arSetPixelFormat(arHandle, pixFormat) < 0 ) 
	{
        std::cerr << "Error: arSetPixelFormat." << std::endl;
        exit(0);
    }
	if( arSetDebugMode(arHandle, AR_DEBUG_ENABLE) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." << std::endl;
        exit(0);
    }
	if( arSetLabelingThresh(arHandle,threshhold) < 0 ) 
	{
        std::cerr << "Error: arSetDebugMode." << std::endl;
        exit(0);
    }

	arSetMarkerExtractionMode( arHandle, AR_NOUSE_TRACKING_HISTORY );

    if( (ar3DHandle=ar3DCreateHandle(&cparam)) == NULL ) 
	{
        std::cerr << "Error: ar3DCreateHandle." << std::endl;;
        exit(0);
    }
    if( (arPattHandle=arPattCreateHandle()) == NULL ) 
	{
        std::cerr << "Error: arPattCreateHandle." << std::endl;
        exit(0);
    }
	setProjection(10.0f, 10000.0f);

	pattListName=pattlist_name;
	SetupMarkers();
	
}

ARParam
ARToolKit4Tracker::getIntrinsicParameters()
{
	return cparam;
}

void
ARToolKit4Tracker::SetupMarkers()
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
			//else ERROR ?
		}
	}
	arPattAttach( arHandle, arPattHandle );
}

int 
ARToolKit4Tracker::addSingleMarker(string pattFile, double width, double centerX, double centerY) {

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
ARToolKit4Tracker::addMultiMarker(string multiFile) 
{
	Marker* multiMarker = new MultiART4Marker();

	if (!static_cast<MultiART4Marker*>(multiMarker)->initialise((char*)multiFile.c_str()))
	{
		std::cerr<<"ERROR:can't initialize multi pattern.."<<std::endl;
		delete multiMarker;
		return -1;
	}
	
	pattList.push_back(multiMarker);

	return pattList.size() - 1;
}

void
ARToolKit4Tracker::setThreshhold(int thresh)
{
	threshhold=thresh;
}


void
ARToolKit4Tracker::update()
{	
    int             j, k;
	static int confF;

	// declare all markers as not beeing updated nor valid
	for (vector<Marker*>::const_iterator iter=pattList.begin();iter!=pattList.end();iter++)		
	{
				Marker* currentMarker=(*iter);
//				currentMarker->m_updated=false;
//				currentMarker->m_valid=false;				
	}
	
	if (image!=NULL)
	{
			// Detect the markers in the video frame.
			if(arDetectMarker(arHandle, image) < 0) 
			//arDetectMarker(arHandle, image)
			{
				exit(-1);
			}
			// Check through the marker_info array for highest confidence
			// visible marker matching our preferred pattern.

				for (vector<Marker*>::const_iterator iter=pattList.begin();
					iter!=pattList.end();iter++)		
				{
					Marker* currentMarker=(*iter);

				//	currentMarker->m_valid=false;
				//	currentMarker->m_updated=true;
					if (currentMarker->getType() == Marker::ART_SINGLE)
					{
						k = -1;
						for (j = 0; j < arHandle->marker_num; j++)	
						{
							if (static_cast<SingleART4Marker*>(currentMarker)->getPatternID() == arHandle->markerInfo[j].id) 
							{
								if (k == -1) k = j; // First marker detected.
								else 
									if(arHandle->markerInfo[k].cf < arHandle->markerInfo[j].cf) k = j; // Higher confidence marker detected.
							}
						}
						if(k != -1) 
						{
							static_cast<SingleART4Marker*>(currentMarker)->update(ar3DHandle, &(arHandle->markerInfo[k]));
						} 
						else 
						{
							static_cast<SingleART4Marker*>(currentMarker)->update(ar3DHandle,NULL);
						}
					}
					else
						if (currentMarker->getType()== Marker::ART_MULTI)
						{
							static_cast<MultiART4Marker*>(currentMarker)->update(ar3DHandle,&(arHandle->markerInfo[k]), arHandle->marker_num);
						}
				}
	}
}

void ARToolKit4Tracker::ARTransToGL(double para[3][4], double gl[16]) {
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 4; i++) {
            gl[i*4+j] = para[j][i];
        }
    }
    gl[0*4+3] = gl[1*4+3] = gl[2*4+3] = 0.0;
    gl[3*4+3] = 1.0;
}

void ARToolKit4Tracker::setProjection(const double n, const double f) {

	/*double   icpara[3][4];
    double   trans[3][4];
    double   p[3][3], q[4][4];

	ARParam	param  = cparam;
	
	for (int i = 0; i < 4; i++) {
        param.mat[1][i] = (param.ysize - 1)*(param.mat[2][i]) - param.mat[1][i];
    }

    if (arParamDecompMat(param.mat, icpara, trans) < 0) {
		std::cerr << "Parameter error while constructing projection matrix." << std::endl;
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