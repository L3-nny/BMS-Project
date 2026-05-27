# Project Overview & Contributor Guide

## Objective

Build a modular, safety-critical Lithium-ion Battery Management System (BMS) in C++.

Each contributor owns exactly one module. Every module integrates into a shared `main.cpp` orchestration layer.

Similar architecture is used in Software Defined Vehicles which run embedded BMS firmware that monitors cells at the millisecond level.

---

## Skills Enhanced

1. Heap Memory Management  
2. Exception Handling  
3. Function Templates  
4. Collaborative Git Workflow


## Repository Layout
BMS-Project/               ← Repository Root
├── include/               ← INTERFACE LAYER
│   └── BMS_Foundation.h   ← Shared header — READ ONLY after architecture review
├── src/                   ← IMPLEMENTATION LAYER (Your private logic)
│   ├── task1_physics.cpp
│   ├── task2_safety.cpp
│   ├── task3_auditor.cpp
│   ├── task4_health.cpp
│   ├── task5_thermal.cpp
