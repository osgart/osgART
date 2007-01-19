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
osgLua.loadWrapper("osgGA")
osgLua.loadWrapper("osgProducer")
osgLua.loadWrapper("osgART")

-- create the scene root
root = osg.Group()

-- create a video capture object
video = osgART.VideoContainer(osgART.VideoManager.createVideoFromPlugin("osgart_artoolkit"));

-- open the video stream
video:open()

-- create a video
videobackground = osgART.VideoBackground(video)

-- initialise the video background
videobackground:init()

-- add the video background to the scene node
root:addChild(videobackground)

-- load the viewer
viewer = osgProducer.Viewer()

-- point the viewer to the scene root
viewer:setSceneData(root)

-- set settings for the viewer
viewer:setUpViewer(osgProducer.Viewer.ViewerOptions.STANDARD_SETTINGS)

-- start the viewer
viewer:realize()

-- start video capture
video:start()

-- main loop
while ( not viewer:done() ) do

	viewer:update()
	video:update()

	viewer:frame()
	viewer:sync()
end

-- stop the video
video:stop()
-- shut down the video stream
video:close()
