#include "NFTMarker"

namespace osgART {

NFTMarker::NFTMarker():Marker()
{

}

NFTMarker::~NFTMarker(void)
{
    
}

Marker::MarkerType NFTMarker::getType() const {
	return Marker::ART_MULTI;
}

void NFTMarker::setActive(bool a) {
	m_active = a;
	
	//TO FIX HERE
}

bool 
NFTMarker::initialise() {
	return true;
}

void 
NFTMarker::update(double trans[3][4]) {
	if (trans!=NULL)
		m_valid = true;
	else
		m_valid = false;
	
	if (m_valid)
		updateTransform(trans);
		
}
};