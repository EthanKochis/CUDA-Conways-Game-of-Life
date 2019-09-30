# CUDA Conway's Game of Life

Parallel Implementation of Conway's Game of Life (see [Conway's Game of Life Wikipedia article](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)) using NVIDIA's CUDA.  Made by Ethan Kochis as the Final Project for HPC Systems Design & Implementation class.

## Installation

Linux:

```sh
make input
make all
```

Make sure that the *nvcc* compiler is in your $PATH environment variable before running the *make all* command.

## Usage

Before running, ensure that you have installed the *imagemagick* package, as the compiled program will use the *convert* command. Also ensure that you have python3 installed with the matplotlib library.

### Optional Input Creation Tool

```sh
./make_input <numRows> <numCols>
```

This program will make a random input grid numRows x numCols to be fed into the Conway's Game of Life algorithm.  This will be output as **input.txt**.

### Conway's Game of Life Programs

```sh
./non_cuda_conway <numRows> <numCols> <numGenerations> <optional:inputFile>
```
OR
```sh
./conway <numRows> <numCols> <numGenerations> <optional:inputFile>
```

This is the program that will run Conway's Game of Life.  It will either generate a random grid or use the one specified in the optional *inputFile* parameter.  The program will create a **results** folder that contains text and PNG representations of each generation. The file **conway.gif** shows each generation in an animation format.

Two versions are provided to compare the speed increase running on a GPU provides:
* ``non_cuda_conway`` runs purely on a CPU without using the CUDA framework.
* ``conway`` uses a GPU to calculate each cell's state in parallel using the CUDA framework.

## Meta

This project uses the MIT license, see ``LICENSE`` for more details.
