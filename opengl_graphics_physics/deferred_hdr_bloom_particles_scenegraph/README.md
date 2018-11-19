
# Deferred Shading, HDR, Bloom, Soft Particles, Scene-Graph, Shadows, Per-Pixel Picking
A project to show off most stuff.

## Features
- directional lights
- point lights
- spot lights
- vsm shadows
- physics(see physics project for more details)
- lights as object components
- per-pixel picking
- soft particles
- HDR
- Bloom
- scene-graph

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
- LAlt - Toggle lock/unlock mouse in window.
- 0-9 - Changing scenes.
- LMB in scene 3 will shoot point light, remember you can pause with P
- K - Pause physics
- WASD - Camera movement.
- LShift - Descend
- Spacebar - Ascend

- Arrows - Move (last selected) object
- NUM+ - Increase time-step
- NUM- - Decrease time-step
- T - Reset time-step (60Hz)
- P - Pause
- O - Toggle drawing of bounding boxes(for objects with rigidbody component).
- L - Draw Lines
- N - Draw Points
- B - Draw Particles
- TAB - Toggle wireframe/shaded drawing modes.
- E - Spawn a cube with physics.
- LCtrl+S - Save the last object added to the scene, mostly for saving the subdivided mesh back to obj file.
