# Verlet Integration with C++ and OpenGL
![](https://github.com/Kotuon/super_waddle/blob/main/verletintegration_gif_short.gif)

## Table of Contents
* [Introduction](#introduction)
* [Technology Used](#technology-used)
* [Requirements](#requirements)
* [Features](#features)
* [References](#references)

## Introduction

My project uses verlet integration to simulate up to 40,000 particles inside a custom engine.

## Technology Used
* C++ 20
* GLFW 3.3.4
* GLAD
* GLM
* Dear ImGUI

## Requirements
Built for Windows 10 using CMake.

## Features
* Particle simulation using verlet integration.
* Custom SIMD implementation for math.
* Fixed update loop for physics.
* Sampling profiler.
* Benchmark class.
* Logging system.
* Octree for collision optimization.

## References
Primary reference was this [implementation of verlet integration in C.](https://github.com/marichardson137/VerletIntegration/tree/main)
