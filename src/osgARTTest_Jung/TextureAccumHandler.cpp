#include "TextureAccumHandler.h"
	
TextureAccumHandler::TextureAccumHandler()
{	
}
	
TextureAccumHandler::~TextureAccumHandler()
{
}

void TextureAccumHandler::init(int w,int h, osg::Group *_root, int _numRepeate, int colNum, int rowNum, int _startingBinNum)
{
	width = w;
	height = h;
	startingBinNum = _startingBinNum;

	root = _root;
	
	fboManager = new FBOManager();
	fboManager->init(width, height, root);

		
	ShaderFactory sf;

	for ( int k=0; k < _numRepeate ; k++)
	{
		osg::ref_ptr<DummyImageLayer> dummy = new DummyImageLayer;
		dummy->init(width,height, colNum, rowNum);
		std::cout << colNum << " " << rowNum << std::endl;
		
		osg::ref_ptr<osg::Group> g = new osg::Group;
		g->getOrCreateStateSet()->setRenderBinDetails(startingBinNum+k , "RenderBin");
		g->addChild(dummy.get());

		fboManager->attachTarget( g.get(), startingBinNum);

		if ( k != 0 )
		{
			dummy->setTexture( fboManager->getTexture(k-1) );
		}
		layers.push_back(dummy);
		groupForLayers.push_back(g);
	}



}

osg::ref_ptr<osg::Texture> TextureAccumHandler::getResultTexture()
{
	int size = fboManager->size();
	return fboManager->getTexture( size - 1);
}

void TextureAccumHandler::addNodeAt(int id, osg::ref_ptr<osg::Node> nd,  bool b4DummyLayer)
{

	osg::ref_ptr<osg::Group> g = groupForLayers.at(id);
	if ( b4DummyLayer )
	{
		int numChild = g->getNumChildren();
		
		if ( numChild > 0 )
		{
			int binNum = g->getChild( numChild - 1)->getOrCreateStateSet()->getBinNumber();

			nd->getOrCreateStateSet()->setRenderBinDetails(binNum - 1 , "RenderBin");

		}
		
	}
	
	g->addChild(nd.get());
	//
}

vector< osg::ref_ptr<DummyImageLayer> > TextureAccumHandler::getLayers()
{
	return layers;
}

	
void TextureAccumHandler::setSeedTexture(osg::ref_ptr<osg::Texture> _tex)
{
	layers.at(0)->setTexture(_tex);
}

void TextureAccumHandler::setTextureInput(osg::ref_ptr<osg::Texture> _tex, int indexForLayer, int indexForTexture)
{
	layers.at(indexForLayer)->setTexture(_tex, indexForTexture);

	//if ( index == 0 )
	//{
	//	std::cout << "Cannot modify the 0 texture." << std::endl;
	//	return;
	//}

	//int size = layers.size();
	//for ( int k=0; k < size ; k++)
	//{
	//	layers.at(k)->setTexture(_tex, index);
	//}
}

TextureAccumHandlerCallback::TextureAccumHandlerCallback()
{
	
}

TextureAccumHandlerCallback::~TextureAccumHandlerCallback()
{
}

void TextureAccumHandlerCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
		

}
