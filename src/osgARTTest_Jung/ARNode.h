#ifndef ARNODE_H
#define ARNODE_H

#include <osg/Node>
#include <osg/Group>
#include <osg/Projection>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>

#include <osg/Matrixf>


class ARScene;

class ARNode : public osg::Group
{
public:
	ARNode();
	virtual ~ARNode();

	void init(int markerID, osgART::GenericTracker* tracker);
	void addModel(osg::ref_ptr<osg::Node> model);

	osg::ref_ptr<osgART::ARTTransform> getMarkerTrans();

	void setImportanceVal(float v)
	{	
		importanceVal = v;
	};
	
	float getImportanceVal()
	{
		return importanceVal;
	};

	void setImportanceValUnit(float v)
	{	
		importanceValUnit = v;
	};
	
	float getImportanceValUnit()
	{
		return importanceValUnit;
	};
	
	void increaseImportanceVal()
	{
		importanceVal = importanceVal + importanceValUnit;

		if (importanceVal > 1 )
			importanceVal = 1.0;
	};
	
	void decreaseImportanceVal()
	{
		float temp = importanceVal - importanceValUnit;
		if ( temp > 0 )
			importanceVal = temp;
	};


	void setParentScene(osg::ref_ptr<ARScene> _arScene);
	osg::ref_ptr<ARScene> getParentScene();

private:

	osg::ref_ptr<osgART::ARTTransform> markerTrans;

	float importanceVal;
	float importanceValUnit;
	osg::ref_ptr<ARScene> arScene;
};

#endif