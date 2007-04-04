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
	osg::Program* shaderPrograme = new osg::Program;
   
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
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( vertObj );		

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
			
	nodeState->setAttributeAndModes(shaderPrograme, osg::StateAttribute::ON);
	shaderPrograme->addShader( fragObj );
	
#ifdef ShaderDebugMode
	if ( addReloadCallback )
	{
		osg::ref_ptr<ShaderReloadCallback> srcb = new ShaderReloadCallback;
		srcb->init(vertexFileName, fragmentFileName);
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
		srcb->init(fileName, "");
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
		srcb->init("", fileName);
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
//////////////////////////////////////////////////////////////////////////////////////

ShaderReloadCallback::ShaderReloadCallback()
{
	wrappedCallback = NULL;
}

ShaderReloadCallback::~ShaderReloadCallback()
{
	wrappedCallback = NULL;
}

void ShaderReloadCallback::init(string _vertFileName, string _fragFileName)
{
	if ( _vertFileName.empty() )
	{
		isVertexShaderOn = false;
	}
	else
	{
		isVertexShaderOn = true;
		vertexFileName = _vertFileName;

		struct stat attribForVert;
		stat( vertexFileName.c_str(), &attribForVert);

		vertFileClock = *gmtime( &(attribForVert.st_mtime) );

		
	}

	if ( _fragFileName.empty() )
	{
		isFragmentShaderOn = false;
	}
	else
	{
		isFragmentShaderOn = true;
		fragmentFileName = _fragFileName;

		struct stat attribForFrag;
		stat( fragmentFileName.c_str(), &attribForFrag);

		fragFileClock = *gmtime( &(attribForFrag.st_mtime) );
	}
}


void ShaderReloadCallback::setWrappedCallback(osg::NodeCallback *_callback)
{

	//osg::ref_ptr<osg::NodeCallback> wrappedCallback = new osg::NodeCallback( *(node->getUpdateCallback()), osg::CopyOp::SHALLOW_COPY );
	//wrappedCallback = new osg::NodeCallback( *(_callback), osg::CopyOp::SHALLOW_COPY );
	wrappedCallback =_callback;
	//wrappedCallback = _callback;
}

bool ShaderReloadCallback::isFileModified(std::string FileName, struct tm* OrigFileClock)
{
	struct tm CurFileClock;

	struct stat attrib; // create a file attribute structure
	stat(FileName.c_str(), &attrib); // get the attributes of afile.txt

	CurFileClock = *gmtime(&(attrib.st_mtime)); // Get the last modified time and put it into the time structure

	if (CurFileClock.tm_sec != OrigFileClock->tm_sec) /* seconds after the minute - [0,59] */
	{
		*OrigFileClock = CurFileClock;
		return true;
	}

	if (CurFileClock.tm_min != OrigFileClock->tm_min)/* minutes after the hour - [0,59] */
	{
		*OrigFileClock = CurFileClock;
		return true;
	}

	if (CurFileClock.tm_hour != OrigFileClock->tm_hour) /* hours since midnight - [0,23] */
	{
		*OrigFileClock = CurFileClock;
		return true;
	}

	//std::cout << "3" << std::endl;
	return false;
}


void ShaderReloadCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
	if ( wrappedCallback != NULL )
	{
		wrappedCallback->operator()(nd, nv);
	}

	if ( isVertexShaderOn && isFragmentShaderOn )
	{		
		if ( isFileModified( vertexFileName, &vertFileClock ) || isFileModified( fragmentFileName, &fragFileClock ) )
		{
			sf.addVertexAndFragmentShaderFromFile( vertexFileName, fragmentFileName, nd, false);
		}		
	}
	else if ( isVertexShaderOn )
	{
		if ( isFileModified( vertexFileName, &vertFileClock ) )
		{
			sf.addVertexShaderFromFile( vertexFileName, nd, false);
		}		
	}
	else if ( isFragmentShaderOn )
	{
		if ( isFileModified( fragmentFileName, &fragFileClock ) )
		{
			sf.addFragmentShaderFromFile( fragmentFileName, nd, false);
		}		
	}

	traverse(nd,nv);
	
}