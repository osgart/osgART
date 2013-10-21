/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2013 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/


#include "ARToolKitTracker"
#include "ARToolKitTrainingSupport"


namespace osgART {

	class ARToolKitTrainingCandidate : public TrainingCandidate {

	public:

		ARToolKitTrainingCandidate(ARMarkerInfo targetInfo, osg::Vec2Array* outline, osg::Image* image) : TrainingCandidate(),
			mOutline(new osg::Vec2Array(outline->begin(), outline->end())),
			mImage(image) {

			mTargetInfo = targetInfo;

			//OutputDebugString("Candidate constructor\n");

		}

		virtual ~ARToolKitTrainingCandidate() {
			//OutputDebugString("Candidate destructor\n");
		}

		virtual osg::Vec2Array* getOutline() {
			return mOutline.get();
		}

		virtual void getSaveInfo(std::string& fileDescription, std::string& fileExtension) {
			fileDescription = "ARToolKit Pattern";
			fileExtension = "patt";
		}

		virtual bool save(std::string filename) {

			if (!mImage.valid()) return false;
			if (filename.empty()) return false;

			if (arSavePatt(mImage->data(), &mTargetInfo, (char*)filename.c_str()) < 0) {
				return true;
			} else {
				return false;
			}

		}


	protected:

		ARMarkerInfo mTargetInfo;
		osg::ref_ptr<osg::Vec2Array> mOutline;
		osg::observer_ptr<osg::Image> mImage;

	};


	class ARToolKitTrainingSet : public TrainingSet
	{
		public:

			ARToolKitTrainingSet(ARToolKitTracker* tracker, ARMarkerInfo* targets, int targetCount) : TrainingSet() {

				//OutputDebugString("TrainingSet constructor\n");

				float h = 0;

				if (osg::Image* img = tracker->getImage()) {

					// Create a copy of the tracker image as it was when the candidates were detected
					mImage = new osg::Image(*img);

					// Need the height of the image so we can invert the y value of each outline vertex
					h = img->t();
				}

				int maxArea = 0;
				ARMarkerInfo* target = NULL;


				for (int i = 0; i < targetCount; i++) {
					if (targets[i].area > maxArea) {
						target = &targets[i];
						maxArea = target->area;
					}
				}


				if (target) {

					osg::ref_ptr<osg::Vec2Array> outline = new osg::Vec2Array();
					for (int v = 0; v < 4; v++) {
						double x, y;
						tracker->getOrCreateCameraConfiguration()->undistort(target->vertex[v][0], target->vertex[v][1], &x, &y);
						outline->push_back(osg::Vec2(x, h - y));
					}

					mCandidates.push_back(new ARToolKitTrainingCandidate(*target, outline.get(), mImage.get()));
				}




			}

			virtual ~ARToolKitTrainingSet() {

				mCandidates.clear();

				//OutputDebugString("TrainingSet destructor\n");

			}

			virtual osg::Image* getImage() {
				return mImage.get();
			}

		protected:

			osg::ref_ptr<osg::Image> mImage;



	};






	ARToolKitTrainingSupport::ARToolKitTrainingSupport(ARToolKitTracker* tracker) : TrackerTrainingSupport(),
		mTracker(tracker) {



	}

	ARToolKitTrainingSupport::~ARToolKitTrainingSupport() {

	}

	void ARToolKitTrainingSupport::processTargets(ARMarkerInfo* targets, int targetCount) {

		mTrainingSet = new ARToolKitTrainingSet(mTracker, targets, targetCount);

	}

	TrainingSet* ARToolKitTrainingSupport::getLatestTrainingSet() {

		return mTrainingSet.get();

	}


};
