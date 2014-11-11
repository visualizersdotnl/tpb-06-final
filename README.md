
TPB-06 - Excessination

This demo has been released and is based on Inque's core code.
Currently will receive cleanup, new features (for reuse) and maybe a final.


Some notes:

- The main idea is that a demo consists of a timeline that specifies the 
  scenes that should be rendered.    
  
  Each scene consists of a shader that is rendered on a full-screen quad. 

  Stuff like sprites, backgrounds and metaballs have been added.
  3D objects or scenes are still missing.

  Currently  subset of the core (and scenegraph, specifically) features are used
  by Player. This is because apart from Rocket sync. this is hardcoded, not
  Maya-generated content.

- There's typically a single 'world initialization' function that loads all assets
  and builds all timelines, creates all objects and constructs the scenegraph.
      
- Some important classes are:
  World: owns ALL objects and is responsible for rendering and updating everything
  Element: baseclass for anything that needs to get updated/ticked
  Node: baseclass for anything that needs to live in the scene graph (e.g anything with a transform)
  Xform: node responsible for building a transform matrix
  MaterialParameter: node responsible for pushing a value to scene shaders.
  