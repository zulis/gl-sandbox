glSandbox
=======

My attempt to write C++ OpenGL graphics library suitable for small game development.

Library motto:

 * Write less, get quick results.
 * Flexibility.
 * Cross platform.
 * Clean header files.
 
I'm pretty sure that nobody will use it, but for me it's a pleasure to work on it and get some experience.

## Examples

See examples folder for more details.

### 04-imgui

Render user interface using Dear ImGui.
![04-imgui](screenshots/04-imgui.png)

### 03-mesh

Mesh loading using Assimp and use of FPS camera.
![03-mesh](screenshots/03-mesh.png)

### 02-primitives

Predefined primitives.
![02-primitives](screenshots/02-primitives.png)

### 01-custom-geometry

Rotating custom geometry.
![01-custom-geometry](screenshots/01-custom-geometry.png)

### 00-simple

Simple window and texture.
![00-simple](screenshots/00-simple.png)

## To get all submodules
```
git submodule update --init --recursive
```

## To update all submodules
```
git submodule update --remote --recursive --merge
```