# Graph Visualizer

Graph Visualizer is a desktop application for creating, exploring, and analyzing graph topologies. It is particularly suited to interconnection networks and networks-on-chip (NoC), while also supporting interactive editing and export of general graph data.

## Overview

The application lets users construct supported topologies from parameters, import existing graphs, adjust their presentation interactively, and calculate graph metrics. The interface is built with Qt Quick and the graph-processing logic is implemented in C++.

## Features

- Create common graph and interconnection-network topologies
- Import graphs from JSON configuration files and adjacency matrices
- Automatically detect supported topologies from adjacency matrices
- Pan, zoom, select, reposition, and recolor vertices and edges
- Group edges and configure line, arc, and Bézier-curve styles
- Find and highlight shortest paths
- Calculate graph metrics, including diameter, average distance, and maximum degree
- Export graphs as PNG/JPEG, SVG, JSON, adjacency matrices, and BookSim network files
- Switch between light and dark themes

## Technology Stack

- C++17
- Qt 6: Core, Gui, Widgets, Quick, Concurrent, and Svg
- QML / Qt Quick Controls
- CMake

## Prerequisites

- CMake 3.28 or later
- A compiler with C++17 support
- Qt 6 development packages with the components listed above

## Build and Run on Linux

Install CMake, a C++17-capable compiler, and the required Qt 6 development packages through your distribution's package manager. Then build and run the application:

```sh
cmake -S . -B build
cmake --build build
./build/GraphVisualizer
```

The application has been tested on Arch Linux and Ubuntu.

## Usage

1. Select a topology, import a JSON configuration, or import an adjacency matrix.
2. Enter the required topology parameters and create the graph.
3. Use the canvas and side controls to edit vertices, edges, colors, groups, and edge styles.
4. Use the menu to export the result or calculate graph parameters.

## Academic Context

This project was developed as a 2024–2025 bachelor's project at HSE University (National Research University Higher School of Economics).

## Contributors

- **Maksim Pavlovich Selin — Programmer**
  - Implemented graph topology construction and automatic vertex-layout methods.
  - Implemented edge grouping, edge-parameter editing, graph metrics, JSON and BookSim import/export, curve-rendering improvements, and part of the topology forms.
  - Contributed to documentation and image export.

- **Aleksei Yakovlevich Bozhedomov — Programmer**
  - Implemented automatic topology detection and graph algorithms.
  - Implemented adjacency-matrix import with topology detection and adjacency-matrix export.
  - Wrote most of the project documentation.

- **[Mikhail Mikhailovich Kuzikov](https://catoffeine.dev) — Programmer**
  - Implemented the application frontend and the frontend-to-backend data-transfer mechanism.
  - Implemented graph-canvas rendering, canvas navigation, display settings, and vertex and edge selection/movement.
  - Implemented final raster and vector image export.
