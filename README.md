# Battleship
Lucy Hu

## Description
A game of battleship with two players (two terminals running simultanteously)
- Each player has their own board with 5 ships (ship placement info is accessed through files)
- Players take turns to input a coordinate to guess the location of their opponent's ships
  - hit or miss is shown on their board
  - ship is revealed when all components of it has been hit
- Game ends when:
  - a player surrenders 
  - first player to clear all of their opponent's ships wins
  
---

## Instructions
### General
- Compile with make
- Run ```./server``` in one terminal
- Run ```./client``` in another terminal
- After handshake is completed, player1 (client.c) goes first to input a coordinate
- Input Coord
  - general format: RowColumn (as depicted on board display)
    - ex: ```B2```
  - program checks if coordinate is valid
    - if coordinate is invalid, user is prompt to input another coordinate
- Symbols
  - hit ```*```
  - miss ```O```
  - your ship ```$```
  - all components of a ship destroyed ```#```
- player2 (server.c) goes next to input a coordinate
- game continues as player1 and player2 take turns until
  - one of them clears all of the other's ships
  - one of them surrenders

### Surrendering
- To surrender, press ```ctrl-c```

### Ships Files
- Board generation is dependent on two files: ```player1``` and ```player2```
- Files contain information about
  - size of ships 
  - direction (horizontal or vertical) represented by 'h' and 'v' respectively
  - row and column of the head of the ship
- Based on original battleship game, there are 5 ships:
  - Carrier (size of 5), Battleship (size of 4), Cruiser (size of 3), Submarine (size of 3), Destroyer (size of 2)
- Program assumes the syntax (specific order) is entered correctly
  - but row and column are checked during board generation to see if ships are out of bound or overlap
    - ex:
    ```
    5hD1
    4vF6
    3vB10
    3vH8
    2hG2
    ```
- Files can be edited before running ```./server``` and ```./client```

---

## Breakdown
### Server and Client Communication
- Server and Client do a handshake

#### Board Generation
- Server (player2) generates its board
  - board generation check
    - check if board generation failed because ships overlap or out of bound
    - Server sends check to Client
    - Client receives check and exits if player2 board generation fails
- Client (player1) generates its board
  - board generation check 
    - Client sends check to Server
    - Server receives check and exits if player1 board generation fails
  
#### Start of Player1's Turn
- Player1 is prompted to input coord
  - surrender check
    - if player1 inputs ctrl-c, sighandler lets program know player1 surrenders
    - Client sends surrender check to Server 
    - Server recieves check and exits if player1 surrenders
- Client sends coord to Server
- Server recieves coord and launches missile
  - coord check
    - if coord is invalid, Server sends check to Client
    - Client recieves check and has to input another coord and sends that to Server
    - Server recieves new coord
    - this repeats until coord is valid
- Launching missile gives a *copy of player2's board* without player2's ships being visible
- Server sends 1D array of *copy of player2's board* 2D array to Client
  - because can't send 2D array through the pipes
- Client receives 1D array and converts it to 2D array to display
- Server checks if all of player2's ships have been destroyed
  - win check 
    - if they were all destroyed, player2 loses and Server exits
    - Server sends the check to Client
    - Client receives check and exits if player2 ships were all destroyed and player1 wins
#### End of Player1's Turn

#### Start of Player2's Turn
- Player2 is prompted to input coord
  - surrender check
    - if player2 inputs ctrl-c, sighandler lets program know player2 surrenders
    - Server sends surrender check to Client
    - Client recieves check and exits if player2 surrenders
- Server sends coord to Client
- Client recieves coord and launches missile
  - coord check
    - if coord is invalid, Client sends check to Server
    - Server recieves check and has to input another coord and sends that to Client
    - Client recieves new coord
    - this repeats until coord is valid
- Launching missile gives a *copy of player1's board* without player1's ships being visible
- Client sends 1D array of *copy of player1's board* 2D array to Server
  - because can't send 2D array through the pipes
- Server receives 1D array and converts it to 2D array to display
- Client checks if all of player1's ships have been destroyed
  - win check 
    - if they were all destroyed, player1 loses and Client exits
    - Client sends the check to Server
    - Server receives check and exits if player1 ships were all destroyed and player2 wins
#### End of Player2's Turn

- Repeats until surrender check or win check is fulfilled
  
### battleship.c

