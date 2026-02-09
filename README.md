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
Super simplified version that throws boids on screen, and let's patterns emerge from pre-defined parameters. Boids wrap around the world border
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
Move towards the center of mass of neighbours, $$ F_{\pu{coh}} = \pu{avg}(pos_{\pu{neighbours}}) - pos_i$$
