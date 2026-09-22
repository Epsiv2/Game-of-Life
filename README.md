# Conway's Game of Life

A console-based implementation of **Conway's Game of Life** written in C++.

The project simulates generations of cells on a dynamic board and allows the user to create an initial state manually, generate one randomly, or load it from a file.

## Features

- Dynamic board size
- Manual cell placement
- Random board generation
- Load board configuration from a file
- Conway's Game of Life rules
- Manual generation stepping
- Automatic simulation mode
- Generation counter
- Cell age tracking
- Color-coded cell ages
- Input validation
- 8-neighbour cell detection, including diagonals

## How It Works

Each cell can be either:

- `X` — alive
- `.` — dead

For every generation, the program checks the eight neighbouring cells and applies the standard Conway's Game of Life rules:

- A live cell survives with 2 or 3 neighbours.
- A live cell dies with fewer than 2 or more than 3 neighbours.
- A dead cell becomes alive with exactly 3 neighbours.

The simulation continues generation by generation until the user exits.

## Setup Modes

When the program starts, you can choose one of three setup modes:

### 1. Manual Setup

Enter the coordinates of the cells that should start alive.

### 2. Random Setup

Choose how many cells should start alive and let the program place them randomly.

### 3. Load From File

Load the initial board configuration from `board.txt`.

Example:

```text
5 8
........
..XX....
...X....
........
....XX..
