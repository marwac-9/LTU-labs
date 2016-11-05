# Subdivision - OpenGL

Visual presentation of optimized subdivision. 

The outer object is the base half edge mesh generated from obj file and then converted for rendering.  
The object inside is the subdivided half edge mesh also converted for rendering.

## Specific keys for this project
Keys 1-3 will toggle between different subdivision meshes.  
Meshes are subdivided in real-time as you switch between scenes.  
Toggle wireframe/shaded mode with TAB key.  
Press LCTRL+S to save the subdivided mesh to obj file.  

## Key-bindings
- 1-5 - Changing scenes.
- WASD - Camera movement.
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
