# Game of Life

This project implements **John Conway’s Game of Life** in the C programming language. The program simulates the evolution of a grid of cells according to simple rules that can generate complex behaviors.

## 📖 Description

The Game of Life is a cellular automaton devised by **John Conway** in 1970. Each cell in the grid can be in one of two states: **alive** or **dead**. At each iteration, the state of the cells is updated based on the following rules:

* A live cell with fewer than 2 live neighbors **dies** (underpopulation).
* A live cell with 2 or 3 live neighbors **survives**.
* A live cell with more than 3 live neighbors **dies** (overpopulation).
* A dead cell with exactly 3 live neighbors **becomes alive** (reproduction).

The file `main.c` contains the core logic to:

* Initialize the grid
* Print the current state
* Compute the next generation
* Run the simulation loop

## ⚙️ Requirements

* **Linux operating system**
* **ncurses library**

## 🚀 Compilation and Execution

To compile the program using the provided **Makefile**:

```bash
make
```

This will generate the executable named `main`.

To run it:

```bash
./main
```

To clean up compiled files:

```bash
make clean
```

## 🖥️ How It Works

* The grid is initialized with a random pattern.
* At each cycle, the terminal displays the new generation.
* You can modify the grid size or the number of generations directly in the code (`main.c`).

## 📂 Project Structure

```
game-of-life/
├── main.c        # Main source code
├── Makefile      # Build configuration
└── README.md     # Project documentation
```

## ✨ Possible Extensions

* Add input from a file for custom initial configurations.
* Enable saving generations to an output file.

## 📜 License

This project is distributed under the **MIT License**. You are free to use, modify, and share it, provided you credit the original author.
