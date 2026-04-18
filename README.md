# Simple Graphing Calculator (SGC)

Visual GPU-driven graphing calculator made in c++ with OpenGL.

## Description

The rendering of the graph dependant only on gpu, with this approach
it doen't matter how complex the graph is or how far zoomed out are you.
Workload depends on pixel count and number of graphs.

Graphs with a lot of expensive operations still can lower performance,
because operations will be recalculated for every pixel every frame.

Example of simple and complex functions. (y = sin(x) and cos(x) = cos(y))

<div align="center">
    <img src="docs/images/sine_wave.png" alt="sine_wave" width="45%"></img>
    <img src="docs/images/stars.png" alt="starts" width="45%"></img>
</div>

## Usage

Executable should be in the same directory as data folder.

When SGC is open would won't see any windows, you will need to open them
through headers first.

## Headers

- windows (here you can find all the windows you can open)
- tools (tools and utils you might need)

## Windows

- info window (get general info about SGC version, possition, zoom, etc.)
- graphs window (add, remove and edit graphs)
- variables window (add, remove, change and edit variables)

## Graphs

Graphs can be functional (default) or equational.

Functional work like this:

`if (approx(y,graph_body,ps)`

Graph body should return a float.

Equational work like this:

`if (graph_body)`

Graph body should return a bool.

## Variables

You can set max value, min value and step.

You can then use variable in your graphs.

The name of the variable can contain letter, numbers or '_' and starts with a letter or a '_'.

## Constants

- x (world pos x)
- y (world pos y, for equations)
- ps (pixel size, for equations)
- pi (~3.14)
- t (elapsed time in seconds)

## Operations

- \+a
- -a
- a\+b
- a*b
- a/b
- a&&b
- a||b
- !a
- a?b:c

## Functions

- abs(a)
- min(a, b)
- max(a, b)
- pow(a,b)
- sin(a)
- cos(a)
- tan(a)
- cot(a)
- approx(a,b,c) - is distance between a & b = c or lower

## Saving graphs

Press save graphs and enter filename you want and all
graphs will be saved to the file. (variables will be saved too)

Savefiles are at "./data/saves/"