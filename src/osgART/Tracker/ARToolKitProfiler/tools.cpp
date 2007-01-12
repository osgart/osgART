#include "ProfilerTools"
#include <iomanip>
#include <ar\config.h>



namespace osgART {

#if _SG_TLS_XML

TiXmlElement* XMLSaveVec3(TiXmlElement* _XML_ROOT, const std::string _Name, const osg::Vec3 &_Vec)
{
	SG_Assert(_XML_ROOT, "XMLSaveVec3() => XML root element empty");
	SG_Assert(_Name!="", "XMLSaveVec3() tag name is empty.");
	
	TiXmlElement * XMLVec3 = new TiXmlElement( _Name.data() );
	XMLVec3->SetDoubleAttribute("x", _Vec[0]);
	XMLVec3->SetDoubleAttribute("y", _Vec[1]);
	XMLVec3->SetDoubleAttribute("z", _Vec[2]);

	_XML_ROOT->InsertEndChild(*XMLVec3);
	return XMLVec3;
}

TiXmlElement* XMLLoadVec3(TiXmlElement* _XML_ROOT, const std::string _Name, osg::Vec3 &_Vec)
{
	SG_Assert(_XML_ROOT, "XMLReadVec3() => XML root element empty");
	SG_Assert(_Name!="", "XMLReadVec3() tag name is empty.");
	
	TiXmlElement * XMLVec3 = XMLGetElement(_XML_ROOT, _Name);
	if (!XMLVec3)
		return NULL;

	double Val[3];
	XMLReadVal(XMLVec3, "x", Val[0]);
	XMLReadVal(XMLVec3, "y", Val[1]);
	XMLReadVal(XMLVec3, "z", Val[2]);
	_Vec.set((float)Val[0], (float)Val[1], (float)Val[2]);

	//PrintVector3D("XMLRead", _Vec);
	return XMLVec3;
}


TiXmlElement* XMLSaveQuat(TiXmlElement* _XML_ROOT, const std::string _Name, const osg::Quat &_Quat)
{
	SG_Assert(_XML_ROOT, "XMLSaveQuat() => XML root element empty");
	SG_Assert(_Name!="", "XMLSaveQuat() tag name is empty.");
	
	TiXmlElement * XMLQuat = new TiXmlElement( _Name.data() );

	XMLQuat->SetDoubleAttribute("v0", _Quat[0]);
	XMLQuat->SetDoubleAttribute("v1", _Quat[1]);
	XMLQuat->SetDoubleAttribute("v2", _Quat[2]);
	XMLQuat->SetDoubleAttribute("v3", _Quat[3]);

	_XML_ROOT->InsertEndChild(*XMLQuat);

	return XMLQuat;
}


TiXmlElement* XMLLoadQuat(TiXmlElement* _XML_ROOT, const std::string _Name,  osg::Quat &_Quat)
{
	SG_Assert(_XML_ROOT, "XMLReadQuat() => XML root element empty");
	SG_Assert(_Name!="", "XMLReadQuat() tag name is empty.");
	
	TiXmlElement * XMLQuat = XMLGetElement(_XML_ROOT, _Name);
	if (!XMLQuat)
		return NULL;

	double Val[4];
	XMLQuat->QueryDoubleAttribute("v0", &Val[0]);
	XMLQuat->QueryDoubleAttribute("v1", &Val[1]);
	XMLQuat->QueryDoubleAttribute("v2", &Val[2]);
	XMLQuat->QueryDoubleAttribute("v3", &Val[3]);

	_Quat.set(Val[0], Val[1], Val[2], Val[3]);


	return XMLQuat;
}
#endif //_SG_TLS_XML

osgText::Text *
		CreateText(const char *_Text, 
						  osg::Vec3 &_Pos, 
						  osg::Vec4 &_Color, 
						  float _Size, 
						  osgText::Font *_Font, 
						  osgText::Text::Layout _Layout)
{
	osgText::Text * CurrText = new osgText::Text();
	if (!CurrText || !_Text)
		return NULL;
	
	CurrText->setFont(_Font);
	CurrText->setColor(_Color);
	CurrText->setCharacterSize(_Size);
	CurrText->setLayout(_Layout);
	//cout << "Create Text:'" << _Text <<"'" << std::endl;
	CurrText->setText(_Text);
	CurrText->setPosition(_Pos);

	return CurrText;
}

osg::Matrix FlipMatrix(const osg::Matrix &_srcMat, osg::Vec3 &_Flip)
{
	//const double PI = 3.1415926535897932;

	osg::Matrix FlipH;
	FlipH.makeRotate(osg::inDegrees(180.), _Flip);

	osg::Matrix Rslt = _srcMat * FlipH;
	
	//PrintMatrix("ConvertProjectionMatrix, SRC", _srcMat);
	//PrintMatrix("ConvertProjectionMatrix, RSLT", Rslt);
	
	return Rslt;
}



};