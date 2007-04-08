#include <osg/FrameStamp>
#include <osg/Notify>


#include "osgART/ARSceneNode"


namespace osgART 
{
	class ARSceneNodeCallback : public osg::NodeCallback
	{
	public:
		ARSceneNodeCallback(ARSceneNode* parent)
			: osg::NodeCallback(),
			_lastModifiedCount(0xFFFFFF),
			_parent(parent)
		{
		}

		void operator()(osg::Node* node, osg::NodeVisitor* nv) 
		{
			ARSceneNode* n = dynamic_cast<ARSceneNode*>(node);

			if (n) 
			{
				ARSceneNode::ConnectionMap::iterator i = n->_connectionmap.begin();

				while (i != n->_connectionmap.end()) 
				{
					if ((*i).second) 
					{
						(*i).second->update();
						(*i).first->update();
					}
					
					i++;
				}

			}
			// must traverse the Node's subgraph            
			traverse(node,nv);
		}

	protected:

		unsigned int _lastModifiedCount;
        
        ARSceneNode* _parent;
	};

	// --------------------------------------------------------------------------

	ARSceneNode::ARSceneNode() :
		osg::Group()		
	{		
	}

	ARSceneNode::ARSceneNode(const ARSceneNode& node, 
		const osg::CopyOp& copyop /* = osg::CopyOp::SHALLOW_COPY*/)
		: Group(node,copyop)
	{
	}

	ARSceneNode::~ARSceneNode()
	{
	}

	//void 
	//ARSceneNode::add(osg::Image* video)
	//{
	//	if (_videomap.size() == 0 && _connectionmap.size() == 0) 
	//	{			
	//		this->setUpdateCallback(new ARSceneNodeCallback(this));
	//	}

	//	_videomap.push_back(video);
	//}

	bool
	ARSceneNode::connect(GenericTracker* tracker,
		osg::ImageStream* image,
		const std::string& patternlist,
		const std::string& cameraparam) 
	{
		if (image == 0L || tracker == 0L) 
		{
			return false;
		}

		if (!tracker->init(image->s(),
			image->t(),
			patternlist,
			cameraparam)) 
		{
			osg::notify(osg::WARN) << "osgART::ARSceneNode::connect(tracker,video): Can not connect video and tracker, initialisation failed!" << std::endl;
			return false;
		}
			
        if (/*_videomap.size() == 0 && */_connectionmap.size() == 0) 
		{			
			this->setUpdateCallback(new ARSceneNodeCallback(this));
			
		}

        // add the connection to the connection map
		_connectionmap[tracker] = image;

		// set the image source for the tracker
		tracker->setImageSource(image);

		return true;
	}

}
