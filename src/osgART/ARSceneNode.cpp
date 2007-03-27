#include "osgART/ARSceneNode"

#include <osg/FrameStamp>
#include <osg/Notify>

namespace osgART 
{
	class ARSceneNodeCallback : public osg::NodeCallback
	{
	public:
		ARSceneNodeCallback(ARSceneNode* parent)
			: osg::NodeCallback(),
			_parent(parent)
		{
		}

		void ARSceneNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) 
		{
			ARSceneNode* n = dynamic_cast<ARSceneNode*>(node);

			if (n) 
			{
				ARSceneNode::ConnectionMap::iterator i = n->_connectionmap.begin();

				while (i != n->_connectionmap.end()) 
				{
					if ((*i).second) 
					{
						// If a new frame is available from the video,
						// then call setImage() with the video object
						// and then update.
						if ((*i).second->getFrame())
						{
							(*i).first->setImage((*i).second);
							(*i).first->update();
						}
					}
					
					i++;
				}

				ARSceneNode::VideoMap::iterator j = n->_videomap.begin();

				while (j != n->_videomap.end()) 
				{

					(*j)->update();

					j++;
				}

			}
			// must traverse the Node's subgraph            
			traverse(node,nv);
		}

	protected:

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

	void 
	ARSceneNode::add(GenericVideo* video)
	{
		if (_videomap.size() == 0 && _connectionmap.size() == 0) 
		{			
			this->setUpdateCallback(new ARSceneNodeCallback(this));
		}

		_videomap.push_back(video);
	}

	bool
	ARSceneNode::connect(GenericTracker* tracker,
		GenericVideo* video) 
	{
		if (video == 0L || tracker == 0L) 
		{
			return false;
		}

		if (!tracker->init(video->getWidth(),
			video->getHeight())) 
		{
			osg::notify(osg::WARN) << "osgART::ARSceneNode::connect(tracker,video): Can not connect video and tracker, initialisation failed!" << std::endl;
			return false;
		}
			
        if (_videomap.size() == 0 && _connectionmap.size() == 0) 
		{			
			this->setUpdateCallback(new ARSceneNodeCallback(this));
		}

		_connectionmap[tracker] = video;

		return true;
	}

}
