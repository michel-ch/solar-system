# Solar System Graphics Project

This project, developed as part of the Computer Graphics course, demonstrates the implementation of fundamental concepts in real-time 3D rendering using OpenGL. By directly manipulating the graphics pipeline, we create an interactive visualization of a simplified solar system that showcases various modern rendering techniques.

## Prerequisites

- GCC/G++ compiler (version 11.0 or higher)
- CMake (version 3.10 or higher)
- OpenGL (version 3.0 or higher)
- GLFW3
- Graphics card with OpenGL support

## Features

- Interactive 3D Solar System visualization
- Custom shader implementation
- Real-time lighting and shadows
- Camera controls and movement
- ImGui integration for interactive controls
- Post-processing effects
- Skybox implementation

## Dependencies Installation

### Ubuntu/Debian
```bash
# Install essential build tools
sudo apt-get update
sudo apt-get install build-essential cmake

# Install OpenGL and GLFW dependencies
sudo apt-get install libgl1-mesa-dev mesa-common-dev
sudo apt-get install libglfw3 libglfw3-dev

# Install additional development libraries
sudo apt-get install pkg-config
sudo apt-get install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Building the Project

Clone the repository:
```bash
git clone https://github.com/yourusername/solar-system.git
```

Navigate to Project Directory
```bash
cd solar-system
```

Create and Enter Build Directory
```bash
mkdir build
cd build
```

Configure CMake
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZER=ON ..
```

Build the project
```bash
make
```

Run the executable
```bash
./systemSolaire
```

### Controls

- Q Z S D keys: Camera movement

- E A keys: Move up / Move down

- Mouse scroll: Zoom in / Zoom out

- Arrow Keys on Keyboard: Look around

- ESC: Exit application

