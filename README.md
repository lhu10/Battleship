# Battleship
Lucy Hu

## Broad Description
A game of battleship with two players (two terminals running simultanteously)
- Each player has their own board with 5 ships (ship placement info is accessed through files)
- Players take turns to input a coordinate to guess the location of their opponent's ships
  - hit or miss is shown on their board
  - ship is revealed when all components of it has been hit
- Game ends when:
  - a player surrenders with SIGINT
  - first player to clear all of their opponent's ships win
  
---

## Instructions
### General
- Compile with make
- Run ```./server``` in one terminal

### Class Concepts
- Allocating memory
  - memory is allocated for each player's boards and ship placements
- Working with files
  - the boards are generated based on information found in a file
  - the user can edit the file before starting the program 
- Pipes
  - named pipes used to establish a connection between the two players
- Signals
  - when user sends the signal SIGNIT, it means the user surrenders and pipes are unlinked to disconnect

### Breakdown
- Board Generation 
  - Each player's boards are generated based on a file
  - file contains the coordinates, length, and vertical or horizontal 
- Check Board
  - Program checks if the information in file doesn't cause any error with formation of board
- Display Board 
- Launch Missile 
  - user inputs coordinate to attack opponent's board
- Check Win
  - checks if player has won after each turn 
- Check Downed Ship
  - checks if all components of a ship has been destroyed
  - if it has, it is revealed with another symbol

### Data Structures
- 2D Array
  - used to represent the board
  - rows are letters
  - columns are numbers
- Linked list
  - individual parts that form a ship will be linked
  - head of the linked list is the head of the ship

### Algorithms
- Handshake
  - ensures a connection has been established between the two players
  
---

## Timeline
- Reading a file and creating board (1/18)
- Creating two programs to send and receive between the two players (1/22)
