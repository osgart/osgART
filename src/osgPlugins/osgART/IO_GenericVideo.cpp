#include <osgART/GenericVideo>
#include <osgART/VideoManager>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>

// forward declare functions to use later.
bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr);
bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw);

// register the read and write functions with the osgDB::Registry.
osgDB::RegisterDotOsgWrapperProxy VideoContainer_Proxy
(
    new osgART::VideoContainer,
    "VideoContainer",
    "Object VideoContainer",
    &GenericVideo_readLocalData,
    &GenericVideo_writeLocalData,
	osgDB::DotOsgWrapper::READ_AND_WRITE

);

bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr)
{

    bool iteratorAdvanced = false;

	/* 
	osgART::VideoContainer& sw = static_cast<osgART::VideoContainer&>(obj);
	
	if (fr.matchSequence("Source %s"))
    {
        const char* _plugin = fr[1].getStr();
        fr+=2;

		sw.load(_plugin);	

		sw.start();

		iteratorAdvanced = true;
    }
	*/

    return iteratorAdvanced;
}


bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
	/* implement stuff here :) */
 
    return true;
}
