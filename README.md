
TPB-06 - Excessination

This is a demo(system) in the works based on Inque's 64kb system.
We're both in production (TPB-06) and fixing some basic features for full-size demo use.

Some notes (for more information check the list of issues):

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
      
- Some important classes are:
  World: owns ALL objects and is responsible for rendering and updating everything
  Element: baseclass for anything that needs to get updated/ticked
  Node: baseclass for anything that needs to live in the scene graph (e.g anything with a transform)
  Xform: node responsible for building a transform matrix
  MaterialParameter: node responsible for pushing a value to scene shaders.
  
