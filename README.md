# LTU-labs
Almost everything I have bashed on the keyboard the during game development studies at LTU

Each project builds to its own bin folder copying over the required resources so the working directory has to be set accordingly for each project to run debug.

## OpenGL
> ### Physics Engine
  - SAT 
  - Clipping - multiple contacts
  - BAUMGARTE + positional penetration correction
  - Impulse based collision response
  - Different integration methods.
  
> ### Graphics
  - Deferred Shading
  - Shadow Mapping
  - Per-pixel picking
  - Geometry Subdivision
  - Particles
  - HDR
  - Bloom
  
## Network
  - Socket programming for Windows & Linux
  - Cross-Platform communication between Windows & Linux
  - Multicast
  - UDP/TCP
  - JAVA, C, C++ 
  
## AI
  - State Machine project simulating life of "sims"
  - Pathfinding algorithms, performance and usecase analysis

### Key-bindings for OpenGL projects
- 1-5 - Changing scenes(number of scenes may vary for each project)
- WASD - Camera movement.
- Backspace - Enable physics for lights(deferred project only)
- LShift - Descend
- Spacebar - Ascend
- LAlt - Toggle lock/unlock mouse in window.
- Arrows - Move (last selected) object
- NUM+ - Increase time-step
- NUM- - Decrease time-step
- T - Reset time-step (60Hz)
- P - Pause
- O - Toggle drawing of bounding boxes.
- F5 - Toggle drawing of contact points, collision normals, and clip planes.
- TAB - Toggle wireframe/shaded drawing modes.
- E - Spawn a cube.
- LCtrl+S - Save the last object added to the scene, mostly for saving the subdivided mesh back to obj file.
