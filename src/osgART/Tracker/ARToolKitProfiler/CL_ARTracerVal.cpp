#include "ARTracerVal"
#include "osgART/Utils"
#if AR_TRACKER_PROFILE
#include "math.h"


namespace osgART {

CL_ARTracerVal::CL_ARTracerVal(void) : CL_Virtual_Record()
{
	Clear();
}

CL_ARTracerVal::CL_ARTracerVal(const CL_ARTracerVal &_Rec2):CL_Virtual_Record()
{
	Clear();
	(*this) = _Rec2;
}

CL_ARTracerVal::CL_ARTracerVal(const long T):CL_Virtual_Record()
{
	Clear();
	Coef = T;
}

CL_ARTracerVal::~CL_ARTracerVal(void)
{}

void CL_ARTracerVal::Clear()
{
	PattID = -1;
	Distance = 0;
	Coef = 0;	
	RMS_Error = 0;
}		


void CL_ARTracerVal::SetTransform(const osg::Matrix &Transform)
{
	// Get the matrix from the tracker
	m_storedPosition = Transform.getTrans();
	m_storedRotation = Transform.getRotate().asVec3();
}

osg::Matrix CL_ARTracerVal::GetTransform()
{
	return GenerateMatrix(m_storedPosition, m_storedRotation);
}



/*void CL_ARTracerVal::SetValue(const long L)
{
	Coef = L;
}
*/

//arithmetic operators
CL_ARTracerVal&   CL_ARTracerVal::operator  =( const CL_ARTracerVal &_Rec2)
{
	PattID	= _Rec2.PattID;
	Distance= _Rec2.Distance;
	Coef	= _Rec2.Coef;
	RMS_Error = _Rec2.RMS_Error;
	return (*this);
}

bool CL_ARTracerVal::operator !=( const CL_ARTracerVal &_Rec2)
{
	if (PattID	!= _Rec2.PattID) return true;
	if (Distance!= _Rec2.Distance) return true;
	if (Coef	!= _Rec2.Coef) return true;
	if (RMS_Error	!= _Rec2.RMS_Error) return true;
	return false;
}
bool CL_ARTracerVal::operator ==( const CL_ARTracerVal &_Rec2)
{
	return !((*this)!=_Rec2);
}

bool CL_ARTracerVal::operator < ( const CL_ARTracerVal &_Rec2)
{
	return Coef < _Rec2.Coef;
}

bool   CL_ARTracerVal::operator > ( const CL_ARTracerVal &_Rec2)
{
	return Coef > _Rec2.Coef;
}

CL_ARTracerVal&   CL_ARTracerVal::operator +=( const CL_ARTracerVal &_Rec2)
{
	Coef += _Rec2.Coef;
	return *this;
}
CL_ARTracerVal&   CL_ARTracerVal::operator -=( const CL_ARTracerVal &_Rec2)
{
	Coef -= _Rec2.Coef;
	return *this;
}
CL_ARTracerVal&   CL_ARTracerVal::operator /=( const long T)
{
	Coef /= T;
	return *this;
}

CL_ARTracerVal&   CL_ARTracerVal::operator *=( const long T)
{
	Coef *= T;
	return *this;
}
/*
CL_ARTracerVal   operator  +( const CL_ARTracerVal &_Rec2)const = 0;//!< operator +.
CL_ARTracerVal   operator  -( const CL_ARTracerVal &_Rec2)const = 0;//!< operator -.
CL_ARTracerVal   operator  /( const long T)const = 0;//!< operator /.
CL_ARTracerVal   operator  *( const long T)const = 0;//!< operator *.
//========================	  
*/	  
/*void Run(void)
{//nothing to do
}
*/
long CL_ARTracerVal::GetValue(void)
{
	return Coef;
}

 std::string CL_ARTracerVal::GetStr(bool _Approxmimate/*=true*/)
{
	//return std::stringstream (Coef).str();
	return FloatToString(Coef);
/*	int     decimal,   sign;
	char  * Buff;
	double  Coef2 = Coef;
	Buff = _ecvt( Coef2, 10, &decimal, &sign);
	return Buff;
*/
}

void CL_ARTracerVal::Print()
{
	std::cout << "|PattID = " << PattID << 
		"||Distance = "<< Distance <<  
		"||Coef = "<< Coef << 
		"||RMS = "<< RMS_Error <<
		"|" <<endl;
}

//see http://en.mimi.hu/gis/rms_error.html
float CL_ARTracerVal::CalculatePosRMS ( osg::Vec3 & _RealPos)
{
	float RMS = SQUARRE(m_storedPosition[0] - _RealPos[0]) +
				SQUARRE(m_storedPosition[1] - _RealPos[1]) +
				SQUARRE(m_storedPosition[2] - _RealPos[2]);

	return sqrt(RMS);	
}

#if _SG_TLS_XML
/*! \fn TiXmlElement* CL_TimerVal::XMLLoad(TiXmlElement* _XML_ROOT, std::string _Name)
*\brief This fonction read the given XML stream looking for the _Name tag, then reads values.
*\param _XML_ROOT	=> XML stream where the given tag should be located
*\param _Name		=> tag name we are looking for.
*\return Success => Pointer to the _Name XML element. Failed => NULL.
*/
TiXmlElement* CL_ARTracerVal::XMLLoad(TiXmlElement* _XML_ROOT, std::string _Name/* =""*/)
{
	SG_Assert(_XML_ROOT, "CL_ARTracerVal::XMLLoad() => XML root element empty");
	SG_Assert(_Name!="", "XML tag name is empty.");

	Clear();//Init all positions values to default

	TiXmlElement* XMLTimer = XMLGetElement(_XML_ROOT, _Name);
	if (!XMLTimer)
		return NULL;

	XMLReadVal(XMLTimer, "dist", this->Distance);
	XMLReadVal(XMLTimer, "coef", this->Coef);
	XMLReadVal(XMLTimer, "patt_id", this->PattID);
	XMLReadVal(XMLTimer, "rms", this->RMS_Error);

	//load position
	XMLLoadVec3(XMLTimer, "pos", m_storedPosition);
	
	//load rotation
	XMLLoadVec3(XMLTimer, "rot", m_storedRotation);

	return XMLTimer;
}


/*! \fn TiXmlElement* CL_TimerVal::XMLSave(TiXmlElement* _XML_ROOT, std::string _Name)
*\brief This fonction save CL_TimerVal object into a XML tag called _Name and inserted ibnto the XML stream _XML_ROOT.
*\param _XML_ROOT	=> XML stream where the object tag will be saved
*\param _Name		=> name the tag will be saved to.
*\return Success => Pointer to the _Name XML element. Failed => NULL.
*/
TiXmlElement* CL_ARTracerVal::XMLSave(TiXmlElement* _XML_ROOT, std::string _Name/*=""*/)
{
	SG_Assert(_XML_ROOT, "CL_ARTracerVal::XMLSave() => XML root element empty");
	SG_Assert(_Name!="", "XML tag name is empty.");
	
	if (_Name!= "")
		SetMainTag(_Name);

	TiXmlElement * XMLTimer = CL_XML_OBJ::XMLLoad(_XML_ROOT);//new TiXmlElement( _Name.data() );
	
	bool newTag= false;
	if (!XMLTimer)
	{
		newTag= true;
		XMLTimer =new TiXmlElement( _Name.data() );
	}

	XMLWriteVal(XMLTimer, "dist",	this->Distance);
	XMLWriteVal(XMLTimer, "coef",	this->Coef);
	XMLWriteVal(XMLTimer, "patt_id",this->PattID);
	if (RMS_Error)
		XMLWriteVal(XMLTimer, "rms",	this->RMS_Error);

	//save position
	XMLSaveVec3(XMLTimer, "pos", m_storedPosition);
	
	//save rotation
	XMLSaveVec3(XMLTimer, "rot", m_storedRotation);

	if(newTag)
		_XML_ROOT->InsertEndChild(*XMLTimer);

	return XMLTimer;
}
#endif
}; // namespace osgART
#endif// AR_TRACKER_PROFILE




