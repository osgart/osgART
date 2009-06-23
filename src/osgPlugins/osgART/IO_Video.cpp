#include <osgART/Video>
#include <osgART/PluginManager>

#include <osg/Notify>
#include <osg/Object>
#include <osg/io_utils>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>

// forward declare functions to use later.
bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr);
bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw);

// register the read and write functions with the osgDB::Registry.
osgDB::RegisterDotOsgWrapperProxy GenericVideo_Proxy
(
	new osgART::Video,
	"osgART::Video",
	"Object Image ImageStream VideoImageStream Video",
    GenericVideo_readLocalData,
    GenericVideo_writeLocalData
);

bool GenericVideo_readLocalData(osg::Object& obj, osgDB::Input& fr)
{
	
	osgART::Video &myobj = static_cast<osgART::Video&>(obj); 

	osg::notify() << "Loading something" << std::endl;

    bool iteratorAdvanced = false;
	//
	//if (fr.matchSequence("Source %s"))
 //   {
 //       const char* _plugin = fr[1].getStr();
 //       fr+=2;

	//	/*
	//	sw.load(_plugin);	

	//	sw.start();
	//	*/

	//	std::cout << "Loading '" << _plugin << "'" << std::endl;

	//	iteratorAdvanced = true;
 //   }

    return iteratorAdvanced;
}


bool GenericVideo_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
	osg::notify() << "Saving something" << std::endl;

	/* implement stuff here :) */
    return true;
}
