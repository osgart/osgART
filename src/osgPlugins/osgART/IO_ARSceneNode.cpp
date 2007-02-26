#include <osgART/ARSceneNode>
#include <osgART/VideoManager>
#include <osgART/GenericVideo>

#include <osg/Notify>
#include <osg/Object>
#include <osg/io_utils>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>

// forward declare functions to use later.
bool ARSceneNode_readLocalData(osg::Object& obj, osgDB::Input& fr);
bool ARSceneNode_writeLocalData(const osg::Object& obj, osgDB::Output& fw);

// register the read and write functions with the osgDB::Registry.
osgDB::RegisterDotOsgWrapperProxy ARSceneNode_Proxy
(
	new osgART::ARSceneNode,
	"ARSceneNode",
	"Object Node Group ARSceneNode",
    ARSceneNode_readLocalData,
    ARSceneNode_writeLocalData
);

bool ARSceneNode_readLocalData(osg::Object& obj, osgDB::Input& fr)
{
	bool iteratorAdvanced = false;
	
	/*osg::notify() */ std::cerr << "Loading something" << std::endl;
	
	if (fr[0].matchWord("video_plugin")) {

        if (fr[1].getStr()) {

			std::string _plugin_name = "osgart_" + std::string(fr[1].getStr());

			osg::ref_ptr<osgART::GenericVideo> _video = osgART::VideoManager::createVideoFromPlugin(_plugin_name); 

			_video->open();
            
            fr += 1;
            iteratorAdvanced = true;
        }
    }

	/* implement stuff here :) */
    return iteratorAdvanced;
}


bool ARSceneNode_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
	/*osg::notify()*/ std::cerr << "Saving something" << std::endl;
	const osgART::ARSceneNode& ar_sn = static_cast<const osgART::ARSceneNode &>(obj);
    

    
	/* implement stuff here :) */
    return true;
}
