 -------------------------------------------------------------------------------
-- Demonstrates the usage of OSGART
-- with osgIntrospection through Lua
--
-- (c) 2006 HIT Lab NZ
--
-------------------------------------------------------------------------------

require 'osgLua'


-- loading other wrappers
osgLua.loadWrapper("osg")
osgLua.loadWrapper("osgDB")
osgLua.loadWrapper("osgGA")
osgLua.loadWrapper("osgProducer")
osgLua.loadWrapper("osgART")


function createCube(boxSize)

	sd = osg.ShapeDrawable(osg.Box(osg.Vec3(0, 0, boxSize / 2.0), boxSize))
	sd:setColor(osg.Vec4(0, 0, 1, 1));

	geo = osg.Geode()
	geo:addDrawable(sd)

	return geo

end

-- load the viewer
viewer = osgProducer.Viewer()
-- set settings for the viewer
viewer:setUpViewer(osgProducer.Viewer.ViewerOptions.ESCAPE_SETS_DONE)
-- switch culling off
viewer:getCullSettings():setComputeNearFarMode(osg.CullSettings.ComputeNearFarMode.DO_NOT_COMPUTE_NEAR_FAR)

-- create the scene root
root = osg.Group()
--
world = osg.Group()

-- create a video capture object
video = osgART.VideoContainer(osgART.VideoManager.createVideoFromPlugin("osgart_artoolkit"));

-- open the video stream
video:open()

-- create a tracker
tracker = osgART.TrackerContainer(osgART.TrackerManager.createTrackerFromPlugin("osgart_artoolkit_tracker"))

-- initialise the tracker
tracker:init(video,"Data/markers_list.dat","Data/camera_para.dat")

projection = tracker:createProjection()

root:addChild(projection)

projection:addChild(world)
world:getOrCreateStateSet():setRenderBinDetails(2, "RenderBin")


-- create a video
videobackground = osgART.VideoBackground(video)

-- initialise the video background
videobackground:init()

-- add the video background to the scene node
projection:addChild(videobackground)


scene = osg.Group()
scene:getOrCreateStateSet():setRenderBinDetails(5, "RenderBin")

--
marker = osgART.MarkerContainer(tracker:getMarker(0))
marker:setActive(true)

--
markertransform = osgART.ARTTransform(marker)

scene:addChild(markertransform)

world:addChild(scene)

-- show a small cube
markertransform:addChild(createCube(40.0))


-- point the viewer to the scene root
viewer:setSceneData(root)

-- start the viewer
viewer:realize()

-- start video capture
video:start()

-- main loop
while ( not viewer:done() ) do

	viewer:update()
	video:update()

	tracker:update()

	viewer:frame()
	viewer:sync()
end

-- stop the video
video:stop()
-- shut down the video stream
video:close()
