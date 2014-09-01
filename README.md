tpb-06
======

TPB-06 - Excessination

_

NOTES

- This framework is based on inque's 64k framework. Since that's a 64k system,
  expect some awkward stuff (things that are not really needed in a fullsize demo)

- The main idea is that a demo consists of a timeline that specifies the 
  scenes that should be rendered.    
  
  Each scene consists of a shader that is rendered on a full-screen quad. 
  
  There's also a timeline that specifies the camera that should be used
  to render the active scene with.
  
  A Camera is an object that lives in the scene graph. It's parented to a
  Xform node that moves it through space. Xform nodes also allow you to attach
  an AnimCurve to each of their members.
- There's typically a single 'world initialization' function that loads all assets
  and builds all timelines, creates all objects and constructs the scenegraph.
- Note that in inque's 64k framework the code for this world initialization function 
  was actually automatically generated upon exporting the demo from maya. 
      
- Some important classes are:
  World: owns ALL objects and is responsible for rendering and updating everything
  Element: baseclass for anything that needs to get updated/ticked
  Node: baseclass for anything that needs to live in the scene graph (e.g anything with a transform)
  Xform: node responsible for building a transform matrix
  MaterialParameter: node responsible for pushing a value to scene shaders.
  
- Take a look at Player\Settings.h, since it contains some important build settings.

- Inque's 64k framework was quite self-contained (only relied on DX and Win32 api) 
  so that's why 'pimp' is still mostly relying on its own container classes
  (instead of relying on STL for example).

_
  
TODO

- Add sound playback system
- Integrate GNU rocket
- Add some good post processing
	- HDR rendering
	- Color grading?
	- Bokeh?
	- Lens flares?
- Add particle system (blend in using depth from scene shader?)
- Use precompiled shaders; dont ship text versions of shaders
- Make it easier to construct the scene in the generate_scene() function
- Fly-through camera support

