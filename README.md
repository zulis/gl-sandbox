glSandbox
=======

My attempt to write C++ OpenGL graphics library suitable for small game development.

Library motto:

 * Write less, get quick results.
 * Flexibility.
 * Cross platform.
 * Clean header files.
 
 I'm pretty sure that nobody will use it, but for me it's a pleasure to work on it and get some experience.
 Please be kind and [tweet me](https://twitter.com/Zulis79) if you found it useful :)

## Examples

See examples folder for more details.

### 02-mesh

Mesh loading using Assimp and use of FPS camera.
![01-cube](screenshots/02-mesh.png)

### 01-cube

Rotating textured cube.
![01-cube](screenshots/01-cube.png)

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