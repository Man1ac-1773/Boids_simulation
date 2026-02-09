# Boids - Interactive Flocking Simulation (raylib+raygui) 

An interactive, gamified implementation of Craig Reynolds’ **Boids algorithm**, built using Raylib and Raygui.

This project is not just a simulation; it is a real-time experimentation environment that allows live tuning of flocking parameters to observe emergent collective behavior.

The goal: Understand emergent systems by manipulating them.

## Overview
Divided into three files, all versions include
- 600 autonomous agents (“boids”)
- Camera pan + zoom (WASD + scroll-wheel)
- Mouse-based repulsion 

All flocking forces are applied dynamically and scaled by deltaTime.
### `simple_boids.cpp` 
Super simplified version that throws boids on screen, and lets patterns emerge from pre-defined parameters. Boids wrap around the world border.
### `simple_wall_hater_boids.cpp`
Same as previous, but walls are obstacles that are steered away from. 
### `boids_game.cpp`
A fully gamified version of the boid simulation with sliders for scaling Seperation, Cohesion, and Alignment forces, and buttons to choose between world wrapping and wall hating boids. 

## Core Concepts
Each boid follows 3 fundamental steering behaviours
#### 1. Seperation
Avoid crowding neighbours, $$F_{\pu{sep}} \propto \frac{(pos_i - pos_j)}{\pu{distance}}$$
#### 2. Alignment
Match velocity with neighbours, $$F_{\pu{ali}} = \pu{avg}(\mathbf{v}_{\pu{neighbours}})$$
#### 3. Cohesion
Move towards the center of mass of neighbours, $$F_{\pu{coh}} = \pu{avg}(pos_{\pu{neighbours}}) - pos_i$$
### Additional forces
#### Mouse repulsion
Mouse acts as a temporary predator, scaring boids away if within a certain radius
#### Wall avoidance
When world wrapping is disable, boids experience repulsive force near walls

--- 
The resultant force on each boid, is the _weighted_ sum of all of these forces, scaled with an appropriate weight.
## GUI Sliders
The controls provided to the user are : 
- Seperation weight
- Alignment weight
- Cohesion weight
- Mouse fear weight
- Wall fear weight
- Max speed
- Wrap Around World (toggle)

## Implementation notes
- `Triangle` struct, for ease in drawing Boid triangles (note : vertices in clock-wise order)
- `Boids` is encapsulated in a class, which stores it's position, velocity, and Triangle data. Each boid is responsible for : 
    - Updating it's current triangle in each frame
    - Wrapping around world (if WrapAround is enabled)
    - Clamping to world (if WrapAround is disabled)
- Each force, when applied, is scaled by deltaTime to accomodate variable FPS simulation. 
- Current neighbour detection follows O(N^2) brute force approach. 

## Design Philosophy
This project emphasizes: 
- Visual feedback for parameter intuition
- Real-time system manipulation
- Emergent complexity from simple rules
- Minimal abstraction overhead

Rather than hiding behavior inside black-box systems, every force is explicitly computed. 

## Build Instructions
**Dependencies**
- Raylib
- c++17 (or compatible)
- raygui is shipped with the repository, under `lib/`
```bash
g++ boids_game.cpp -o boids -lraylib -lm
```

## TODO : Improvements
### Performance
-  Implement spatial hashing grid (uniform grid)
- Implement quadtree neighbor lookup
- SIMD optimizations for force accumulation
- Parallelize update step (OpenMP or std::execution)
### Physics 
- Separate acceleration vector from velocity for ease of understanding from physics standpoint
- Limit steering force instead of raw velocity
- Add smooth wall force based on distance curve
- Implement obstacle avoidance
### Visual 
- Toggle perception radius visualization
- Highlight selected boid
- Add predator boid (red, hunts flock)
- Add color based on speed
- Motion trails
- Density heatmap
### Interaction
- Click to spawn boids
- Drag to attract flock
- Adjustable world size
- Save/load parameter presets
- “Chaos mode” randomizer button
### System Extension
- Multi-species flocks
- Energy system (boids tire over time)
- Goal-directed flock (target waypoint)
- Obstacles and terrain
- Dynamic environment forces (wind fields)

## Possible Future Directions
- A systems design showcase
- A visual AI demo
- Performance Optimisation study
- Or a full interactive artificial life sandbox 

## Final Note
This project intentionally balances clarity and control over performance and abstraction.

It is a barebones platform for exploration; not a finished engine. 

