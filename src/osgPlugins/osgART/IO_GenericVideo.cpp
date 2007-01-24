#include <osg/Object>
#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>

#include <osgART/GenericVideo>
#include <osgART/VideoManager>

// forward declare functions to use later.
bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr);
bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw);

// register the read and write functions with the osgDB::Registry.
osgDB::RegisterDotOsgWrapperProxy GenericVideo_Proxy
(
	0L,
    "GenericVideo",
    "Object ImageStream VideoImageStream GenericVideo",
    &GenericVideo_readLocalData,
    &GenericVideo_writeLocalData,
	osgDB::DotOsgWrapper::READ_AND_WRITE

);

bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr)
{

	std::cout << "Loading something" << std::endl;

    bool iteratorAdvanced = false;
	
	if (fr.matchSequence("Source %s"))
    {
        const char* _plugin = fr[1].getStr();
        fr+=2;

		/*
		sw.load(_plugin);	

		sw.start();
		*/

		std::cout << "Loading '" << _plugin << "'" << std::endl;

		iteratorAdvanced = true;
    }

    return iteratorAdvanced;
}


bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
	/* implement stuff here :) */
    return true;
}
