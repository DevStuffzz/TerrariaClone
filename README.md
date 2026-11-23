# TerrariaClone

Terraria clone made using C++ and OpenGL.

## Setup (Windows)

Install Visual Studio 2022 with the **Desktop development with C++** workload
and Git, then:

1. Run `git submodule update --init --recursive` (or let `build.ps1` do it).
2. Double-click **`GenerateProjects.bat`**. It downloads Premake 5.0.0 into
   `build\tools` if `premake5` is not on PATH and generates
   `TerrariaClone.sln` in the repo root.
3. Open `TerrariaClone.sln` and press F5. `TerrariaClone` is the startup
   project, and Debug and Release (x64) configurations are available.

Re-run `GenerateProjects.bat` after adding, removing, or moving source files
or after editing `premake5.lua`. When debugging from Visual Studio the working
directory is the repo root, so `assets\...` paths resolve without copying.

To build from the command line instead:

```powershell
.\build.ps1                         # Debug build
.\build.ps1 -Run                    # build, then run the game
.\build.ps1 -Configuration Release -Test   # Release build + renderer tests
```

The script also mirrors the `assets` folder into
`build\bin\<config>-windows-x86_64\assets`, so the executable can be run from
there. If script execution is disabled, use
`powershell -ExecutionPolicy Bypass -File .\build.ps1 -Run`.

GLFW, GLM, stb, and the Khronos OpenGL registry are pinned as submodules in
`vendor`.
OpenGL itself is provided by Windows and the graphics driver. The project uses
the OpenGL 3.3 GLAD loader bundled with GLFW; replace or regenerate it if a
different OpenGL version or additional extensions are needed.

## Project layout

The solution has three projects (plus GLFW under *Dependencies*):

| Project | Output | Contents |
| --- | --- | --- |
| `Engine` | `Engine.dll` | `Engine\src`: `Core` (`Application`, `Window`, `Input`), `Renderer` (`Renderer`, `Mesh`, `Texture`, `Shader`, `Camera2D`), and `Vendor` (GLAD, stb_image). GLFW is linked into the DLL. |
| `TerrariaClone` | `TerrariaClone.exe` | `TerrariaClone\src`: the game. Links `Engine` and is the startup project. |
| `RendererTests` | `RendererTests.exe` | `tests`: an OpenGL integration test that compiles the engine sources directly so it can call OpenGL itself. |

All binaries go to `build\bin\<config>-windows-x86_64`, so the game finds
`Engine.dll` next to it. Other folders: `assets` (game assets) and `vendor`
(git submodules: GLFW, GLM, stb, OpenGL registry).

Engine includes are relative to `Engine\src`, e.g. `#include "Renderer/Mesh.h"`;
games can include everything with `#include <Engine.h>`. Public engine classes
are marked `ENGINE_API` (see `Core\Base.h`) so they're exported from the DLL;
add it to any new class the game needs to use. The game only sees GLM and the
engine headers, not GLFW or OpenGL.

## Application, Window, and Input

`Application` is an engine base class that owns the `Window` and `Renderer`
and runs the game loop: poll events, `onUpdate(deltaTime)`, `onRender()`, swap
buffers. Games derive from it and override those hooks (and `onResize`), using
`window()` and `renderer()` to reach the engine. See
`TerrariaClone\src\TerrariaGame.cpp`. `main.cpp` only creates the game, calls
`run()`, and deletes it.

`Window` hides GLFW and GLAD: constructing it creates the OS window and OpenGL
context, so it must outlive every GPU object (`Application` declares it first
for this reason). Use `setResizeCallback` to react to framebuffer resizes.

`window.input()` gives polled input. `isKeyDown(Key::A)` is true while a key is
held. `isKeyPressed`/`isKeyReleased` are true only on the frame the key changed.
Mouse buttons work the same way, and `mousePosition()` and `scrollDelta()` are
also available. Combine `mousePosition()` with `Camera2D::screenToWorld` to
find the world point under the cursor.

## Renderer

The renderer targets OpenGL 3.3. Construct GPU objects (`Renderer`, `Mesh`,
`Texture`, `Shader`) after making a context current and loading GLAD, and
destroy them before destroying that context. They are move-only.

- `Mesh` owns a vertex/index buffer of `Vertex { position, texCoord, color }`.
  Indices are optional. Use `MeshUsage::Dynamic` and `setData` for geometry
  that is rebuilt often, such as tile chunks. `Mesh::createQuad()` makes a
  unit quad from (0, 0) to (1, 1).
- `Texture` loads image files with stb_image (`Texture("assets/tile.png")`) or
  wraps raw RGBA8 pixels. Nearest filtering is the default for pixel art, and
  images are flipped so UV (0, 0) is the bottom-left.
- `Shader` compiles a GLSL program and caches uniform locations.
- `Camera2D` is an orthographic camera: `position` is the world point at the
  screen center and `viewHeight` is how many world units are visible
  vertically. Call `setViewportSize` on resize; `screenToWorld` converts GLFW
  cursor coordinates to world coordinates.

```cpp
Renderer renderer;
Camera2D camera(width, height, 9.0f);
Texture dirt("assets/dirt.png");
Mesh chunk(vertices, indices);

renderer.clear({0.1f, 0.15f, 0.2f, 1.0f});
renderer.beginScene(camera);
renderer.draw(chunk, Renderer::transform({0.0f, 0.0f}), &dirt);
renderer.drawQuad({-1.0f, -1.0f}, {2.0f, 2.0f}, {0.3f, 0.7f, 0.3f, 1.0f});
renderer.drawQuad({2.0f, 0.0f}, {1.0f, 1.0f}, dirt);
```

The built-in shader outputs `texture * vertex color * tint` with alpha
blending enabled. `draw(mesh, shader, transform)` renders with a custom shader;
the renderer sets `uViewProjection` and `uTransform`, and vertex attributes are
at locations 0 (position), 1 (texCoord), and 2 (color).
`TerrariaClone\src\TerrariaGame.cpp`
shows a WASD-controlled camera.

The `RendererTests` project runs a hidden-window OpenGL integration test. Build
it in Visual Studio and run the resulting `RendererTests.exe` on a machine with
an OpenGL 3.3-capable driver.
