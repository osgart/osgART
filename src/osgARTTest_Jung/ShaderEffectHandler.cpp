#include "ShaderEffectHandler.h"

ShaderEffectHandler::ShaderEffectHandler()
{
	index = -1;
	targetIndex = -1;
}

ShaderEffectHandler::~ShaderEffectHandler()
{
}



void ShaderEffectHandler::init(osg::Node *nd)
{
	node = nd;
}

void ShaderEffectHandler::addEffect( osg::ref_ptr<ShaderEffect> effect )
{
	effects.push_back( effect );

}

void ShaderEffectHandler::switchToEffect(int id)
{
	int size = effects.size()- 1;
	if ( id > size )
		return;
	
	if ( id == index )
		return;


	if ( index == -1 )
	{
		targetIndex = id;
	}
	else
	{
		// turn the prev effect off
		effects.at(index)->turnOn(false);
		targetIndex = id;
	}
	std::cout << "Index " << index << " TargetIndex " << targetIndex << " " << effects.size() << std::endl;
}


void ShaderEffectHandler::update()
{
	if ( targetIndex != -1 && index == -1)
	{
		// 1st time
		index = targetIndex;
		targetIndex = -1;
		effects.at(index)->turnOn(true);

		effects.at(index)->linkTo(node);
		//node->setUpdateCallback( effects.at(index).get() );

		return;
	}

	if ( index < 0 || index > (int) ( effects.size() - 1))
		return;
	
	if ( targetIndex != -1 && effects.at(index)->isAnimationOn() == false )
	{

		// now switch to a new effect
		index = targetIndex;
		targetIndex = -1;
				
		effects.at(index)->turnOn(true);		
		effects.at(index)->linkTo(node);
		//node->setUpdateCallback( effects.at(index).get() );
	}
}

bool ShaderEffectHandler::turnOnOff()
{
	if ( index != -1 && index < (int)(effects.size()))
	{			
		effects.at(index)->turnOnOff();
	}
}