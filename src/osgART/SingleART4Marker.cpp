///////////////////////////////////////////////////////////////////////////////
// File name : SingleART4Marker.C
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

#include <osgART/SingleART4Marker>

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

const char* const SingleART4Marker_RCS_ID = "@(#)class SingleART4Marker definition.";

///////////////////////////////////////////////////////////////////////////////
// class SingleART4Marker
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Static variable
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// PUBLIC: Standard services 
///////////////////////////////////////////////////////////////////////////////

SingleART4Marker::SingleART4Marker():Marker()
{

}

SingleART4Marker::~SingleART4Marker(void)
{
    
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC : Interface 
///////////////////////////////////////////////////////////////////////////////

void 
SingleART4Marker::SelfDisplay(ostream& stream) const
{
   stream << "< class SingleART4Marker >" << endl;
}

bool 
SingleART4Marker::OK(void) const
{
    return true;
}


Marker::MarkerType SingleART4Marker::getType() const
{
	return Marker::ART_SINGLE;
}

void SingleART4Marker::setActive(bool a) {
	m_active = a;
	
	//HACK RAPH
	//TO FIX HERE
}

bool SingleART4Marker::initialise(ARPattHandle *arPattHandle, char* pattFile, double width, double centerX, double centerY) {
	patt_id = arPattLoad(arPattHandle,pattFile);
	if (patt_id < 0) return false;
	patt_width = width;
	patt_center[0] = centerX;
	patt_center[1] = centerY;
	return true;
}

void SingleART4Marker::update(AR3DHandle *ar3DHandle, ARMarkerInfo* markerInfo) 
{
	
	if (markerInfo == NULL) {
		m_valid = false;
	} else {
		arGetTransMatSquare(ar3DHandle,&(markerInfo[patt_id]), patt_width, patt_trans);
//		arGetTransMatCont(markerInfo, patt_trans, patt_center, patt_width, patt_trans);
		m_valid = true;
			for (int i=0;i<3;i++)
	{
		for (int j=0;j<4;j++)
		{
			std::cerr<<patt_trans[i][j]<<" ";
		}
		std::cerr<<endl;
	}
		updateTransform(patt_trans);
	}

}

int SingleART4Marker::getPatternID() {
	return patt_id;
}

double SingleART4Marker::getPatternWidth() {
	return patt_width;
}
	
double* SingleART4Marker::getPatternCenter() {
	return patt_center;
}

///////////////////////////////////////////////////////////////////////////////
// PROTECTED : Services
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// PRIVATE : Services
///////////////////////////////////////////////////////////////////////////////