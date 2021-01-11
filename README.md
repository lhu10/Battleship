# Battleship
Lucy Hu

## Problem
Creating a simple game of battleship with two players (two terminals running simultaneously)

---

## Description
### User Interface 
- Two terminals (one for each player)

### Class Concepts
- Allocating memory
  - memory is allocated for each player's boards and ship placements
- Working with files
  - the boards are generated based on information found in a file
  - the user can edit the file before starting the program 
- Pipes
  - named pipes will be used to establish a connection between the two players
- Signals
  - when user sends the signal SIGNIT, it means the user surrenders

### Breakdown
- Each player's boards are generated based on a files, containing the coordinates, length, and vertical or horizontal information about the ships
- Player 1 and Player 2 see their opponent's board that initially looks blank
- Player 1 is prompted to input a coordinate to guess the location of one of the ships
- Program shows whether it was hit or miss
  - ship is fully revealed when it has all been hit
- Player 2, who was waiting for Player 1, is now prompted to input a coordinate 
- Program shows whether it was hit or miss
- the back and forth continues until one player reveals all the ships
- Player to first reveal all the ships wins and the game ends

### Data Structures


### Algorithms

---

## Timeline
