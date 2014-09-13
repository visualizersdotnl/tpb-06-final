tpb-06
======

TPB-06 - Excessination

_

NOTES

- This framework is based on inque's 64k framework. Since that's a 64k system,
  expect some awkward stuff (things that are not really needed in a fullsize demo).

  @plek: I've fully replaced the stub (Main.cpp), got things back to a working state
  and I'm preparing to remove all (intentional) leaks and add old fashioned boolean
  error checking. For the specifics see Stub.cpp.

  @plek: Also, the whole concept of 64KB-esque code or compatibility is out the door,
  as you advised Glow!

- The main idea is that a demo consists of a timeline that specifies the 
  scenes that should be rendered.    
  
  Each scene consists of a shader that is rendered on a full-screen quad. 
  
  There's also a timeline that specifies the camera that should be used
  to render the active scene with.
  
  A Camera is an object that lives in the scene graph. It's parented to a
  Xform node that moves it through space. Xform nodes also allow you to attach
  an AnimCurve to each of their members.

  @plek: This will change a bit in the timing department as Rocket is a little
  different from old school timing. Will have to dive into it further to see how
  it's going to fit exactly.

- There's typically a single 'world initialization' function that loads all assets
  and builds all timelines, creates all objects and constructs the scenegraph.

  @plek: See Stub.cpp, eventually it must be allowed to fail and exit gracefully
  (for which the new stub allows beautifully).

- Note that in inque's 64k framework the code for this world initialization function 
  was actually automatically generated upon exporting the demo from maya. 

  @plek: Good to know. Thanks for the example, tells me more or less how to design
  some stuff that'll make it doable to manually operate. But help is needed! :-)
      
- Some important classes are:
  World: owns ALL objects and is responsible for rendering and updating everything
  Element: baseclass for anything that needs to get updated/ticked
  Node: baseclass for anything that needs to live in the scene graph (e.g anything with a transform)
  Xform: node responsible for building a transform matrix
  MaterialParameter: node responsible for pushing a value to scene shaders.
  
- Take a look at Player\Settings.h, since it contains some important build settings.

  @plek: Did that. Added some, removed some. Check it out!

- Inque's 64k framework was quite self-contained (only relied on DX and Win32 api) 
  so that's why 'pimp' is still mostly relying on its own container classes
  (instead of relying on STL for example).

  @plek: As far as I can see there's nothing really redundant nor missing!

_
  
TODO (@plek: Beyond what's in Src/Player/Stub.cpp.)

- Add BASS and make it do bleep.
- Basic GNU rocket integration (further integration with scenes will develop as we go).
- Add some good post processing
	- Bokeh (Shifter wants this, but what's there now is already quite suitable). (@glow: I'll take a look.)
	- Anamorphic lens flares (@glow: I'll take a look.)
	- Color grading? (@glow: I'll take a look. probably lower prio.)  
- Add particle system (blend in using depth from scene shader?).
- Use precompiled shaders; dont ship text versions of shaders -> very important!
- Make it easier to construct the scene in the GenerateScene() function -> very important!

@plek: As I go on and learn how all this sticks together and am refactoring it I realize
it's going to take a little longer than expected to get into full production. Having said
that, it won't hurt if any partaking in the programming try some effects in either Shadertoy
or whatever boilerplate code!
