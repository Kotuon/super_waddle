# Super Waddle

## Table of Contents
* [Introduction](#introduction)
* [Technology Used](#technology-used)
* [Requirements](#requirements)
* [Features](#features)
* [References](#references)

## Introduction

Super Waddle uses verlet integration to simulate up to 36,000 particles inside a custom engine.

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

## References
Primary reference was this [implementation of verlet integration in C.](https://github.com/marichardson137/VerletIntegration/tree/main)
