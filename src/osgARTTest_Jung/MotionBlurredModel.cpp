#include "MotionBlurredModel.h"


MotionBlurredModel::MotionBlurredModel()
{
	//this->setUpdateCallback( new MotionBlurredModelCallback() );
	this->setEventCallback( new MotionBlurredModelCallback() );
}
	
MotionBlurredModel::~MotionBlurredModel()
{
}

void MotionBlurredModel::init(osg::ref_ptr<osg::Node> _targetModel, int attriIndex)
{
	targetModel = _targetModel;

	this->addChild(targetModel.get());
	
	geoVisitor.init(attriIndex);
	targetModel->accept(geoVisitor);
}

void MotionBlurredModel::update()
{
	//osg::MatrixList mList1 = targetModel->getWorldMatrices(); // not working with FBO? WHY???
	//// fix this later!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//osg::Matrix mm = osg::Matrixf::identity();
	//int size = mList1.size();
	//for ( int k=0; k < size; k++)
	//{
	//	mm = mm * mList1.at(k);
	//}

	//geoVisitor.update();	

	ARNode *arNode =  dynamic_cast<ARNode *> (targetModel.get());
	osg::ref_ptr<osgART::ARTTransform> trans = arNode->getMarkerTrans();
	geoVisitor.update( trans->getMatrix() );		
	
	
	
	
}

MotionBlurredModelCallback::MotionBlurredModelCallback()
{
	
}

MotionBlurredModelCallback::~MotionBlurredModelCallback()
{
}

void MotionBlurredModelCallback::operator()(osg::Node *nd , osg::NodeVisitor* nv)
{
		
	MotionBlurredModel *mbm = dynamic_cast<MotionBlurredModel*>( nd ); 
	mbm->update();

	//nd->comp
	
}
