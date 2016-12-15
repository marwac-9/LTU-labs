

# Physics 

Impulse based physics engine presentation.

## Features
- SAT for collision detection
- Clipping for generation of multiple contacts
- Rigid bodies behaviour
- Impulse based collision response
- Multiple implementations of integrators, Euler, Mid-Point, Runge-Kutta 4
- BAUMGARTE and positional penetration correction
- Unlimited stacking
- Advanced debug rendering(F5) of: contact points, collision normals, clip planes, active/sleeping objects, collision detection
- Per-pixel picking
- Scene-Graph

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
