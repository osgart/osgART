#include "ShaderFactory.h"

ShaderFactory::ShaderFactory()
{
}

ShaderFactory::~ShaderFactory()
{
}

bool ShaderFactory::addVertexAndFragmentShaderFromFile(string vertexFileName, string fragmentFileName, osg::Node *node)
{	
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* vertObj = 
		new osg::Shader( osg::Shader::VERTEX );
    if ( !loadShaderSource( vertObj, vertexFileName ) )
	{
		std::cerr << "Vertex shader error!!" << std::endl;
		return false;
	}		
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( vertObj );		

    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( !loadShaderSource( fragObj, fragmentFileName ) )
	{
		std::cerr << "Fragment shader error!!" << std::endl;
	}
			
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( fragObj );
	
	return true;
}

bool ShaderFactory::addVertexShaderFromFile(string fileName, osg::Node *node)
{
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* vertObj = 
		new osg::Shader( osg::Shader::VERTEX );
    if ( loadShaderSource( vertObj, fileName ) )
	{
		nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
		shaderPrograme->addShader( vertObj );
		nodeState->addUniform(new osg::Uniform("tex", 0)); 

		return true;
	}
	
	std::cerr << "File Not Found" << std::endl;
	return false;
}


bool ShaderFactory::addFragmentShaderFromFile(string fileName, osg::Node *node)
{	
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( loadShaderSource( fragObj, fileName ) )
	{
		nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
		shaderPrograme->addShader( fragObj );

		std::cerr << fileName << " loaded" << std::endl;
		return true;
	}
	
	std::cerr << "File Not Found" << std::endl;
	return false;
}


bool ShaderFactory::loadShaderSource(osg::Shader* obj, const string fileName )
{
   if( fileName.length() == 0 )
   {
      std::cout << "File \"" << fileName << "\" not found." << std::endl;
      return false;
   }
   bool success = obj->loadShaderSourceFromFile( fileName.c_str());
   if ( !success  )
   {
      std::cout << "Couldn't load file: " << fileName << std::endl;
      return false;
   }
   else
   {
      return true;
   }
}