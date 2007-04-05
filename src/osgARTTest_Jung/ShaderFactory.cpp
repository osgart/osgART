#include "ShaderFactory.h"
#include <time.h>

ShaderFactory::ShaderFactory()
{
}

ShaderFactory::~ShaderFactory()
{
}

bool ShaderFactory::addVertexAndFragmentShaderFromFile(string vertexFileName, string fragmentFileName, osg::Node *node, bool addReloadCallback)
{	
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	//osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* vertObj = 
		new osg::Shader( osg::Shader::VERTEX );
    if ( !loadShaderSource( vertObj, vertexFileName ) )
	{
		std::cerr << "Vertex shader error!!" << std::endl;
		return false;
	}		
	else
	{
		std::cerr << vertexFileName << " loaded" << std::endl;
	}
	//nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	

    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( !loadShaderSource( fragObj, fragmentFileName ) )
	{
		std::cerr << "Fragment shader error!!" << std::endl;
	}
	else
	{
		std::cerr << fragmentFileName << " loaded" << std::endl;
	}
	
	osg::StateAttribute *program = nodeState->getAttribute( osg::StateAttribute::PROGRAM );


	osg::Program* shaderPrograme = new osg::Program;
	shaderPrograme->addShader( vertObj );
	shaderPrograme->addShader( fragObj );
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);


	
	//shaderPrograme->addShader( vertObj );		
	//shaderPrograme->addShader( fragObj );
	//nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	
#ifdef ShaderDebugMode
	if ( addReloadCallback )
	{
		osg::ref_ptr<ShaderReloadCallback> srcb = new ShaderReloadCallback;
		srcb->addShader( vertexFileName, vertObj);
		srcb->addShader( fragmentFileName, fragObj);

		if ( node->getUpdateCallback() != NULL)
		{
			//osg::ref_ptr<osg::NodeCallback> wrappedCallback = new osg::NodeCallback( *(node->getUpdateCallback()) );
			srcb->setWrappedCallback( node->getUpdateCallback() );
		}
		node->setUpdateCallback( srcb.get() );
	}
#endif

	return true;
}

bool ShaderFactory::addVertexShaderFromFile(string fileName, osg::Node *node, bool addReloadCallback)
{
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* vertObj = 
		new osg::Shader( osg::Shader::VERTEX );
    if ( !loadShaderSource( vertObj, fileName ) )
	{
		std::cerr << "File Not Found" << std::endl;
		return false;
	}
	std::cerr << fileName << " loaded" << std::endl;
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( vertObj );
	nodeState->addUniform(new osg::Uniform("tex", 0)); 

#ifdef ShaderDebugMode
	if ( addReloadCallback )
	{
		osg::ref_ptr<ShaderReloadCallback> srcb = new ShaderReloadCallback;

		srcb->addShader( fileName, vertObj);
		
		if ( node->getUpdateCallback() != NULL)
		{
			srcb->setWrappedCallback( node->getUpdateCallback() );
		}
		node->setUpdateCallback( srcb.get() );
	}
#endif

	return true;
}


bool ShaderFactory::addFragmentShaderFromFile(string fileName, osg::Node *node, bool addReloadCallback)
{	
	osg::StateSet* nodeState = node->getOrCreateStateSet();	
	osg::Program* shaderPrograme = new osg::Program;
   
    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( !loadShaderSource( fragObj, fileName ) )
	{
		std::cerr << "File Not Found" << std::endl;
		return false;
	}
	
	
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( fragObj );
	std::cerr << fileName << " loaded" << std::endl;

#ifdef ShaderDebugMode
	if ( addReloadCallback )
	{
		osg::ref_ptr<ShaderReloadCallback> srcb = new ShaderReloadCallback;
		srcb->addShader( fileName, fragObj);

		if ( node->getUpdateCallback() != NULL)
		{
			srcb->setWrappedCallback( node->getUpdateCallback() );
		}
		node->setUpdateCallback( srcb.get() );
	}
#endif
	return true;
}


bool ShaderFactory::loadShaderSource(osg::Shader* obj, const string fileName )
{
   if( fileName.length() == 0 )
   {
      std::cerr << "File \"" << fileName << "\" not found." << std::endl;
      return false;
   }
   bool success = obj->loadShaderSourceFromFile( fileName.c_str());
   if ( !success  )
   {
      std::cerr << "Couldn't load file: " << fileName << std::endl;
      return false;
   }
   else
   {
      return true;
   }
}

//////////////////////////////////////////////////////////////////////////////////////

ShaderPackage::ShaderPackage()
{
}

ShaderPackage::~ShaderPackage()
{
}

void ShaderPackage::init( string _fileName, osg::Shader *_shader)
{
	fileName = _fileName;
	shader = _shader;

	struct stat attrib;
	stat( fileName.c_str(), &attrib);

	fileClock = *gmtime( &(attrib.st_mtime) );
}

bool ShaderPackage::isModified()
{
	struct tm CurFileClock;

	struct stat attrib; // create a file attribute structure
	stat(fileName.c_str(), &attrib); // get the attributes of afile.txt

	CurFileClock = *gmtime(&(attrib.st_mtime)); // Get the last modified time and put it into the time structure

	if (CurFileClock.tm_sec != fileClock.tm_sec) /* seconds after the minute - [0,59] */
	{
		fileClock = CurFileClock;
		return true;
	}

	if (CurFileClock.tm_min != fileClock.tm_min)/* minutes after the hour - [0,59] */
	{
		fileClock = CurFileClock;
		return true;
	}

	if (CurFileClock.tm_hour != fileClock.tm_hour) /* hours since midnight - [0,23] */
	{
		fileClock = CurFileClock;
		return true;
	}

	return false;
}

void ShaderPackage::update()
{
	if ( isModified() )
	{
		bool success = shader->loadShaderSourceFromFile( fileName.c_str() );
		if ( success )
		{
			std::cerr << fileName << " is reloaded" << std::endl;
		}
		else
		{
			std::cerr << "Fail to reload "<< fileName << std::endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

ShaderReloadCallback::ShaderReloadCallback()
{
	wrappedCallback = NULL;
}

ShaderReloadCallback::~ShaderReloadCallback()
{
	wrappedCallback = NULL;
}

void ShaderReloadCallback::addShader(string shaderFileName, osg::Shader *shader)
{
	//vector<string> fileNames;
	//vector<osg::Shader*> shaders;
	//vector<struct tm> fileClocks;
	//fileNames.push_back( shaderFileName );

	ShaderPackage sp;
	sp.init( shaderFileName, shader);
	
	shaders.push_back( sp );

}


void ShaderReloadCallback::setWrappedCallback(osg::NodeCallback *_callback)
{

	//osg::ref_ptr<osg::NodeCallback> wrappedCallback = new osg::NodeCallback( *(node->getUpdateCallback()), osg::CopyOp::SHALLOW_COPY );
	//wrappedCallback = new osg::NodeCallback( *(_callback), osg::CopyOp::SHALLOW_COPY );
	wrappedCallback =_callback;
	//wrappedCallback = _callback;
}

void ShaderReloadCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
	if ( wrappedCallback != NULL )
	{
		wrappedCallback->operator()(nd, nv);
	}

	int size = (int)(shaders.size());
	
	for ( int k=0; k < size ; k++)
	{
		shaders.at(k).update();
	}

	traverse(nd,nv);
	
}