#include "ShaderEffect.h"

ShaderEffect::ShaderEffect()
{
	animationOn = false;
	isOn = false;
	total = 0;
}

ShaderEffect::~ShaderEffect()
{
}

void ShaderEffect::init(float _timeInMillisec)
{
	shaderProgram = new osg::Program;
	effectTimer = new osg::Timer(); 

	timeInMillisec = _timeInMillisec;
}


osg::Program* ShaderEffect::createVertexAndFragmentShaderFromFile(string vertexFileName, 
											string fragmentFileName)
{
    osg::Shader* vertObj = 
		new osg::Shader( osg::Shader::VERTEX );
    if ( !loadShaderSource( vertObj, vertexFileName ) )
	{
		std::cerr << "Vertex shader error!!" << std::endl;
		return NULL;
	}		

	
	shaderProgram->addShader( vertObj );		

    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( !loadShaderSource( fragObj, fragmentFileName ) )
	{
		std::cerr << "Fragment shader error!!" << std::endl;
		return NULL;
	}
	
	shaderProgram->addShader( fragObj );

	return shaderProgram.get();
}


osg::Program* ShaderEffect::createFragmentShaderFromFile(string fragmentFileName)
{
	   
    osg::Shader* fragObj = 
      new osg::Shader( osg::Shader::FRAGMENT );
    if ( !loadShaderSource( fragObj, fragmentFileName ) )
	{
		std::cerr << "File Not Found" << std::endl;
		return NULL;
	}

	shaderProgram->addShader( fragObj );
	
	return shaderProgram.get();
}

bool ShaderEffect::loadShaderSource(osg::Shader* obj, const string fileName )
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

void ShaderEffect::linkTo( osg::Node *nd)
{
	osg::StateSet* nodeState = nd->getOrCreateStateSet();	
	nodeState->setAttributeAndModes(shaderProgram.get(), osg::StateAttribute::ON);

	nd->setEventCallback( this );
}

void ShaderEffect::operator()(osg::Node *nd, osg::NodeVisitor* nv)
{
	update();
	traverse(nd,nv);
}


bool ShaderEffect::turnOn(bool b)
{
	if ( !animationOn )
	{
		if ( isOn != b)
		{
			isOn = b;
			animationOn = true;
			lastTime = effectTimer->tick();
			return true;
		}
	}

	return false;
}
void ShaderEffect::update()
{
	if ( animationOn )
	{
		osg::Timer_t curTime = effectTimer->tick();

		float timePassed = effectTimer->delta_m(lastTime, curTime);
		float t;
		//delta_m 0.004
		//delta_n 300~400

		total = total + timePassed ;
		
		t = total / timeInMillisec;
		if ( t < 0.0f )
		{
			t = 0.0f;
		}
		
		if ( t > 1.0f )
		{
			t = 1.0f;
		}
		
		if ( isOn )
		{
			whenOn(t);
		}
		else 
		{
			whenOff(t);
		}
				
		lastTime = effectTimer->tick();

		if ( t >= 1.0 )
		{
			animationOn = false;
			total = 0;
		}
	}

}