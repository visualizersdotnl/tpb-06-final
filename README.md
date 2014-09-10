tpb-06
======

TPB-06 - Excessination

_

NOTES (revision 2, I believe)

- This framework is based on inque's 64k framework. Since that's a 64k system,
  expect some awkward stuff (things that are not really needed in a fullsize demo).

  @plek: I'm in the process of fixing what needs fixing for a demo, the rest is cool.
  I'm a little hazy now but I think this strategy works (though it's not very pretty,
  the exceptions we're an onion in the ointment but I was hesitant to remove them and
  replace it for a good old nested objects function with a SetLastError() :-)).

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

  @plek: I'm going to allow it to fail by throwing exceptions. Think of assets that
  won't load for whatever reason. This wasn't exactly the case (as far as I could
  see) before and why would it be for a 64K :)

- Note that in inque's 64k framework the code for this world initialization function 
  was actually automatically generated upon exporting the demo from maya. 

  @plek: Good to know. Thanks for the example, tells me more or less how to design
  some stuff that'll make it doable to manually operate.
      
- Some important classes are:
  World: owns ALL objects and is responsible for rendering and updating everything
  Element: baseclass for anything that needs to get updated/ticked
  Node: baseclass for anything that needs to live in the scene graph (e.g anything with a transform)
  Xform: node responsible for building a transform matrix
  MaterialParameter: node responsible for pushing a value to scene shaders.
  
- Take a look at Player\Settings.h, since it contains some important build settings.

  @plek: Did that. Added some, removed some :-)

- Inque's 64k framework was quite self-contained (only relied on DX and Win32 api) 
  so that's why 'pimp' is still mostly relying on its own container classes
  (instead of relying on STL for example).

  @plek: As far as I can see there's nothing really redundant nor missing!

_
  
TODO

- Add BASS and make it do bleep.
- Basic GNU rocket integration (further integration with scenes will develop as we go).
- Add some good post processing
	- HDR rendering.
	- Color grading?
	- Bokeh? (Shifter wants this, but what's there now is already quite suitable).
	- Lens flares?
- Add particle system (blend in using depth from scene shader?).
- Use precompiled shaders; dont ship text versions of shaders -> very important!
- Make it easier to construct the scene in the generate_scene() function.
  @plek: I'm going to try and take a crack at that, will cry for help at some point :-)
- Fly-through camera support.
  @plek: Not yet sure if we'll have scenes that need it but time will tell.

After the basics are rolling (which, surprise, is going to take longer than I thought it
would initially) we'll get Decipher on board and see if he has some requirements for his
FX (if he'll step up and actually make some). Hope that Glow also does some, but he knows
this code and will modify/help as sees fit.
