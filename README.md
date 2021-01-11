# Battleship
Lucy Hu

## Problem
Creating a simple game of battleship with two players (two terminals running simultaneously)

---

## Description
### User Interface 
- Two terminals (one for each player)
- Player 1 and Player 2 see their opponent's board that initially looks blank
- Player 1 is prompted to input a coordinate to guess the location of one of the ships
- Program shows whether it was hit or miss
  - ship is fully revealed when it has all been hit
- Player 2, who was waiting for Player 1, is now prompted to input a coordinate 
- Program shows whether it was hit or miss
- the back and forth continues until one player reveals all the ships
- Player to first reveal all the ships wins and the game ends

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
