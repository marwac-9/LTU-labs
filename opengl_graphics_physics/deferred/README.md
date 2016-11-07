
# Deferred Shading

## Features
- directional lights
- point lights
- physics(see physics project for more details)
- lights treated as objects meaning that physics can be easily applied(to activate press Backspace and switch to scene 4)
- per-pixel picking

## Implementation
Uses 4 textures:
- worldpos
- diffuse+material color
- normals
- material properties 
  - ambient intensity
  - diffuse intensity
  - specular intensity
  - glossiness/shininess

## Key-bindings
- 1-3 - Changing scenes.
- LMB in scene 3 will shoot point lights, remember you can pause with P
- **Backspace** - Toggle physics for lights in scene 4
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
