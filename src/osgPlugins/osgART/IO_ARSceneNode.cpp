#include <osgART/ARSceneNode>
#include <osgART/VideoManager>
#include <osgART/GenericVideo>
#include <osgART/TrackerManager>

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

	osgART::ARSceneNode &node = 
		static_cast<osgART::ARSceneNode&>(obj);

	
	/*osg::notify() */ std::cerr << "Loading something" << std::endl;
	
	if (fr[0].matchWord("video_plugin")) {

        if (fr[1].getStr()) {

			std::string _plugin_name = "osgart_" + std::string(fr[1].getStr());

			osg::ref_ptr<osgART::GenericVideo> _video = osgART::VideoManager::createVideoFromPlugin(_plugin_name); 

			_video->open();
            
            fr += 2;

            iteratorAdvanced = true;
        }
    }

	if (fr[0].matchWord("tracker_plugin")) {

        if (fr[1].getStr()) {

			std::string _plugin_name = "osgart_" + std::string(fr[1].getStr());

			osgART::TrackerManager::createTrackerFromPlugin(_plugin_name);

			fr += 2;

            iteratorAdvanced = true;

		}
	}

	if (fr[0].matchWord("connect")) {

        if (fr[1].getStr()) {

			int _tracker_id, _video_id;

			if (fr[1].getInt(_tracker_id) && 
				fr[2].getInt(_video_id)) 
			{
				osg::ref_ptr<osgART::GenericVideo> _video = 
					osgART::VideoManager::getInstance()->getVideo(_video_id);
                
				osg::ref_ptr<osgART::GenericTracker> _tracker = 
					osgART::TrackerManager::getInstance()->getTracker(_tracker_id);

				node.connect(_tracker.get(),_video.get());
                
			}

			fr += 3;

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
