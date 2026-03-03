# Pac-Man on LandTiger LPC1768 board

Embedded implementation of Pac-Man for the NXP LPC1768 microcontroller,
developed in C with Keil µVision as part of the "Computer Architectures" course at Politecnico di Torino.

## Features
- GLCD rendering: maze, Pac-Man, ghost, score and lives
- Ghost AI using BFS (Breadth-First Search) for pathfinding
- Joystick control and interrupt-driven buttons
- Background music and sound effects via PWM
- Touch Panel support
- CAN bus communication
- RIT/Timer-based game loop with 60-second countdown

## Hardware
- Board: LandTiger (NXP LPC1768, ARM Cortex-M3)
- Toolchain: Keil MDK (µVision)
