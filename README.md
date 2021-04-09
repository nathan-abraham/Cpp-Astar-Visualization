# About
C++ pathfinding visualization that implements the A* Pathfinding Algorithm. This project was inspired by Tech With Tim's pathfinding visualization in python. The SFML header files and .dll files are not mine; they are used for the graphics.

## Usage
Start by placing the start and end nodes by clicking at the desired location (the start node is yellow and the end node is cyan). Then, place the barrier nodes (white) by clicking or clicking and dragging. Press space to start the algorithm.

## About the A* Algorithm
The A* algorithm is an informed pathfinding algorithm. It is very similar to Dijkstra's algorithm, but unlike Dijkstra, A* uses a heuristic to approximate the distance from the current node to the end node. In this case, the heuristic I used was Manhattan distance, which is just the absolute value of the differences between the x-coordinates and y-coordinates of the two nodes in question. 

## Demo
![GIF Demo](demo.gif)