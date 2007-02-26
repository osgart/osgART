#include <osgART/VideoBackground>

#include <osgART/VideoManager>

#include <osg/Notify>
#include <osg/Object>
#include <osg/io_utils>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>

// forward declare functions to use later.
bool VideoBackground_readLocalData(osg::Object& obj, osgDB::Input& fr);
bool VideoBackground_writeLocalData(const osg::Object& obj, osgDB::Output& fw);

// register the read and write functions with the osgDB::Registry.
osgDB::RegisterDotOsgWrapperProxy VideoBackground_Proxy
(
	new osgART::VideoBackground /* 0L */,
    "VideoBackground",
    "Object Node Group GenericVideoObject VideoLayer VideoBackground",
    VideoBackground_readLocalData,
    VideoBackground_writeLocalData
);

bool VideoBackground_readLocalData(osg::Object& obj, osgDB::Input& fr) {

	bool iteratorAdvanced = false;

	osgART::VideoBackground &node = 
		static_cast<osgART::VideoBackground&>(obj);
	
	/*osg::notify() */ std::cerr << "Loading VideoBackground" << std::endl;

	int _video_id;

	
	if (fr[0].matchWord("connected_video")) {

        if (fr[1].getInt(_video_id))
		{
	
			osg::ref_ptr<osgART::GenericVideo> _video = 
				osgART::VideoManager::getInstance()->getVideo(_video_id);

			if (_video.valid()) 
			{

				node.setVideo(_video.get());

				node.init();

				_video->start();
			} else 
			{
				std::cout << "Error getting the video!" << std::endl;
			}
		
			fr += 2;

            iteratorAdvanced = true;
        }
    }

	/*

	
    if (fr[0].matchWord("NewChildDefaultValue"))
    {
        if (fr[1].matchWord("TRUE")) 
        {
            sw.setNewChildDefaultValue(true);
            iteratorAdvanced = true;
            fr += 2;
        }
        else if (fr[1].matchWord("FALSE"))
        {
            sw.setNewChildDefaultValue(false);
            iteratorAdvanced = true;
            fr += 2;
        }
        else if (fr[1].isInt())
        {
            int value;
            fr[1].getInt(value);
            sw.setNewChildDefaultValue(value!=0);
            iteratorAdvanced = true;
            fr += 2;
        }
    }

    if (fr.matchSequence("ActiveSwitchSet %i"))
    {
        unsigned int switchSet;
        fr[1].getUInt(switchSet);
        fr+=2;
        
        sw.setActiveSwitchSet(switchSet);
    }


    if (fr.matchSequence("ValueList %i {"))
    {
        int entry = fr[0].getNoNestedBrackets();

        unsigned int switchSet;
        fr[1].getUInt(switchSet);

        // move inside the brakets.
        fr += 3;

        unsigned int pos=0;
        while (!fr.eof() && fr[0].getNoNestedBrackets()>entry)
        {
            int value;
            if (fr[0].getInt(value))
            {
                sw.setValue(switchSet, pos,value!=0);
                ++pos;
            }
            ++fr;
        }

        ++fr;
        
        iteratorAdvanced = true;
        
    }

	*/

    return iteratorAdvanced;
}


bool VideoBackground_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
    /*
	const MultiSwitch& sw = static_cast<const MultiSwitch&>(obj);


    fw.indent()<<"NewChildDefaultValue "<<sw.getNewChildDefaultValue()<<std::endl;

    fw.indent()<<"ActiveSwitchSet "<<sw.getActiveSwitchSet()<<std::endl;

    unsigned int pos = 0;
    const osgSim::MultiSwitch::SwitchSetList& switchset = sw.getSwitchSetList();
    for(osgSim::MultiSwitch::SwitchSetList::const_iterator sitr=switchset.begin();
        sitr!=switchset.end();
        ++sitr,++pos)
    {
        fw.indent()<<"ValueList "<<pos<<" {"<< std::endl;
        fw.moveIn();
        const MultiSwitch::ValueList& values = *sitr;
        for(MultiSwitch::ValueList::const_iterator itr=values.begin();
            itr!=values.end();
            ++itr)
        {
            fw.indent()<<*itr<<std::endl;
        }
        fw.moveOut();
        fw.indent()<<"}"<< std::endl;
    }
	*/

    return true;
}
