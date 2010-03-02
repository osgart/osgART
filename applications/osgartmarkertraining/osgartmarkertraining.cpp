/* -*-c++-*-
 *
 * osgART - ARToolKit for OpenSceneGraph
 * Copyright (C) 2005-2008 Human Interface Technology Laboratory New Zealand
 *
 * This file is part of osgART 2.0
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/FileUtils>

#include <osgART/Scene>
#include <osgART/GeometryUtils>

#include <sstream>

#ifdef WIN32
#include <windows.h>
#endif

std::string askForFileName(std::string defaultValue = "marker.patt") {

	std::string filename = defaultValue;

#ifdef WIN32

	OPENFILENAME ofn; // common dialog box structure
	char szFile[260]; // buffer for file name
	sprintf(szFile, defaultValue.c_str());

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All Files\0*.*\0Pattern Files\0*.patt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName (&ofn) != 0) {
		filename = std::string(ofn.lpstrFile);
	}

#else

	std::cout << "Enter the marker file name (leave blank to cancel): ";
	getline(std::cin, filename);

#endif

	return filename;

}





class TrainingCandidateNode : public osg::Group {

protected:
	osg::observer_ptr<osgART::TrainingCandidate> mCandidate;
	osg::ref_ptr<osg::Switch> mVisibleSwitch;
	osg::ref_ptr<osg::Geode> mGeode;
	osg::ref_ptr<osg::Geometry> mBorderGeom;
	osg::ref_ptr<osg::Vec3Array> mVertexArray;
	osg::ref_ptr<osgText::Text> mCornerText;

public:

	TrainingCandidateNode() : osg::Group(),
		mCandidate(NULL),
		mVisibleSwitch(new osg::Switch()),
		mGeode(new osg::Geode()),
		mBorderGeom(new osg::Geometry()),
		mVertexArray(new osg::Vec3Array(4)),
		mCornerText(new osgText::Text()) {

		this->addChild(mVisibleSwitch.get());
		mVisibleSwitch->addChild(mGeode.get());

		mGeode->addDrawable(mBorderGeom.get());
		mGeode->addDrawable(mCornerText.get());

		mGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		osg::Vec4Array* colors = new osg::Vec4Array();
		colors->push_back(osg::Vec4(0, 1, 0, 1));
		colors->push_back(osg::Vec4(1, 0, 0, 1));
		colors->push_back(osg::Vec4(1, 0, 0, 1));
		colors->push_back(osg::Vec4(0, 1, 0, 1));
		mBorderGeom->setColorArray(colors);
		mBorderGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		mBorderGeom->setVertexArray(mVertexArray.get());
		mBorderGeom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, 4));
		mBorderGeom->setUseDisplayList(false);

		mCornerText->setText("Top Left");
		mCornerText->setFont("tahoma.ttf");
		mCornerText->setCharacterSize(10.0f);
		mCornerText->setAlignment(osgText::Text::RIGHT_BOTTOM);
		mCornerText->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
		mCornerText->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

	}

	void setCandidate(osgART::TrainingCandidate* candidate) {
		mCandidate = candidate;
		osg::Vec2Array* outline = candidate->getOutline();
		if (outline->getNumElements() >= 4) {
			for (int i = 0; i < 4; i++) {
				(*mVertexArray)[i] = osg::Vec3((*outline)[i].x(), (*outline)[i].y(), 0);
			}
			mCornerText->setPosition((*mVertexArray)[2]);
		}
	}

	osgART::TrainingCandidate* getCandidate() {
		return mCandidate.get();
	}

	void setVisible(bool v) {
		if (v) mVisibleSwitch->setAllChildrenOn();
		else mVisibleSwitch->setAllChildrenOff();
	}

	void save() {
		if (!mCandidate.valid()) return;

		std::string filename = askForFileName();

		if (!filename.empty()) {
			if (mCandidate->save(filename)) {
				std::cout << "Marker saved" << std::endl;
			} else {
				std::cout << "Error saving marker" << std::endl;
			}
		}

	}


};


osg::ref_ptr<TrainingCandidateNode> trainingNode;
osg::ref_ptr< osgART::TypedField<int> > thresholdField;
osg::ref_ptr< osgART::TypedField<bool> > debugField;
osg::ref_ptr< osgART::TypedField< osg::ref_ptr<osg::Image> > > debugImageField;
osg::ref_ptr<osgText::Text> thresholdText;
osg::ref_ptr<osg::Switch> debugSwitch;

class KeyboardMouseEventHandler : public osgGA::GUIEventHandler {

public:
	KeyboardMouseEventHandler() : osgGA::GUIEventHandler() { }

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) {

		switch (ea.getEventType()) {
			case osgGA::GUIEventAdapter::PUSH:
				trainingNode->save();
				break;

			case osgGA::GUIEventAdapter::KEYDOWN:
				switch (ea.getKey()) {
					case 't':
						thresholdField->set(thresholdField->get() + 1);
						break;
					case 'y':
						thresholdField->set(thresholdField->get() - 1);
						break;
					case 'd':
						debugField->set(!debugField->get());
						break;
				}

				break;

		}
		return false;
	}
};



osg::Node* createDebugImageVisual() {

	osg::Group* debugVisual = new osg::Group();

	if (osg::Image* debugImage = debugImageField->get()) {
		osgART::VideoGeode* vidGeode = new osgART::VideoGeode(debugImage, NULL, debugImage->s(), debugImage->t(), 20, 20, osgART::VideoGeode::USE_TEXTURE_2D);
		vidGeode->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
		debugVisual->addChild(vidGeode);
	}

	return debugVisual;
}


int main(int argc, char* argv[])  {

	osgViewer::Viewer viewer;

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// add relevant handlers to the viewer
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	viewer.addEventHandler(new KeyboardMouseEventHandler());

	osgART::Scene* scene = new osgART::Scene();

	osg::ref_ptr<osgART::Video> video = scene->addVideoBackground("osgart_video_artoolkit2");
	if (!video.valid()) {
		std::cout << "No video" << std::endl;
		return -1;
	}

	osg::ref_ptr<osgART::Tracker> tracker = scene->addTracker("osgart_tracker_artoolkit2");
	if (!tracker.valid()) {
		std::cout << "No tracker" << std::endl;
		return -1;
	}


	// Threshold field (used to adjust threshold for image binarisation process)
	thresholdField = reinterpret_cast< osgART::TypedField<int>* >(tracker->get("threshold"));
	if (!thresholdField.valid()) {
		std::cout << "Threshold field not available from tracker" << std::endl;
		return -1;
	}

	// Debug field (used to enable and disable generation of the black/white debug image)
	debugField = reinterpret_cast< osgART::TypedField<bool>* >(tracker->get("debug"));
	if (!debugField.valid()) {
		std::cout << "Debug field not available from tracker" << std::endl;
		return -1;
	}

	// Debug image field (gives access to the black/white debug image itself)
	debugImageField = reinterpret_cast< osgART::TypedField< osg::ref_ptr<osg::Image> >* >(tracker->get("debug_image"));
	if (!debugImageField.valid()) {
		std::cout << "Debug image field not available from tracker" << std::endl;
		return -1;
	}

	// Training support module for the selected tracker
	osg::ref_ptr<osgART::TrainingSupport> trainer = tracker->getTrainingSupport();
	if (!trainer.valid()) {
		std::cout << "This tracker does not support training." << std::endl;
		return -1;
	}

	trainer->setEnabled(true);

	osg::Camera* hudCamera = new osg::Camera();
	hudCamera->setProjectionMatrixAsOrtho2D(0, video->s(), 0, video->t());
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	hudCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);
	hudCamera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, GL_FALSE);
	scene->addChild(hudCamera);

	trainingNode = new TrainingCandidateNode();
	hudCamera->addChild(trainingNode.get());

	// -------------------------------------------------------
	// Label for the threshold
	// -------------------------------------------------------

	osg::Geode* labelGeode = new osg::Geode();
	thresholdText = new osgText::Text();
	thresholdText->setFont("tahoma.ttf");
	thresholdText->setCharacterSize(12.0f);
	thresholdText->setPosition(osg::Vec3(20, 20, 0));
	thresholdText->setAlignment(osgText::Text::LEFT_BOTTOM);
	thresholdText->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);
	thresholdText->setColor(osg::Vec4(0.85f, 0.0f, 0.0f, 1.0f));
	labelGeode ->addDrawable(thresholdText.get());
	labelGeode->getOrCreateStateSet()->setRenderBinDetails(3, "RenderBin");
	hudCamera->addChild(labelGeode);

	debugField->set(false);

	debugSwitch = new osg::Switch();
	debugSwitch->addChild(createDebugImageVisual());
	hudCamera->addChild(debugSwitch.get());


	viewer.setSceneData(scene);
	viewer.realize();

	// Set the window title text
	osgViewer::Viewer::Windows ws;
	viewer.getWindows(ws);
	if (!ws.empty()) ws[0]->setWindowName("osgART Marker Training");

	while (!viewer.done()) {

		osg::ref_ptr<osgART::TrainingSet> trainingSet = trainer->getLatestTrainingSet();

		if (trainingSet.valid()) {
			osgART::TrainingSet::TrainingCandidateList& candidates = trainingSet->getTrainingCandidates();

			if (!candidates.empty()) {
				trainingNode->setCandidate(candidates[0].get());
				trainingNode->setVisible(true);
			} else {
				trainingNode->setVisible(false);
			}
		}

		// Show debug image if enabled
		if (debugField->get()) debugSwitch->setAllChildrenOn();
		else debugSwitch->setAllChildrenOff();

		// Update onscreen text
		std::stringstream ss;
		ss << "Debug Image [d]: " << (debugField->get() ? "On" : "Off") << std::endl << std::endl;
		ss << "Threshold [t/y]: " << thresholdField->get();
		thresholdText->setText(ss.str());

		viewer.frame();

	}

	return 0;

}
