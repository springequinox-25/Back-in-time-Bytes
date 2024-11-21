————————Project Overview————————
Back-in-time Bytes is a modern compilation of beloved classic games, including 2048, Tic-Tac-Toe, Fifteen Puzzle and Snake. Designed with enhanced graphics and sound effects, this collection aims to provide an immersive gaming experience. Each game offers unique challenges and has been developed with features like highest score recording, varying difficulties and themes etc.




————————Installation Steps————————
1. Ensure you have Qt 6.6.3 installed on your system.
2. Download the zip file and unzip the file.
3. Open the Qt Creator and navigate to the project directory.
4. Select the .pro file for the game you wish to play and configure the project with your system's Qt version.
5. Build the project.
6. Run the game.




————————————————USAGE MANUAL————————————————

————————Snake User Manual————————

Snake controls:
- Navigate using arrow keys or the keys 'W', 'A', 'S', and 'D'.
- The snake can move only in directions other than its current opposite direction; pressing the key for the opposite direction will have no effect.


Foods:
- Red apple: Increases score and length by 1.
- Silver apple: Increases score and length by 1, triggers protected stage for the snake. The snake gains temporary protection, allowing it to pass through obstacles within 10 seconds. If protection is used within 10 seconds, it returns to normal immediately; otherwise, it reverts after 10 seconds.
- Golden apple: Doubles body length, increasing score based on size.


Poison:
- Decreases length by 2, with no change in score.


Obstacle:
- Collision results in game over unless protected. With protection, obstacle is removed and snake returns to normal phase.


Snake stages:
- Green: Normal phase.
- Silver: Protected phase, activated by consuming a silver apple. Provides temporary protection against obstacles for one collision within 10 seconds. Reverts to normal phase after collision or 10 seconds.


Speed:
- Speed increases with each red apple consumed until reaching maximum.


Game over:
- Occurs upon collision with walls or itself or obstacles (with no protection), or if snake length is less than or equal to zero.


Winning:
- When the snake size reached 300 for easy mode, 400 for medium mode, 500 for hard mode.


Additional effects:
- Silver apples appear randomly and may not always be present.
- Golden apples revert to red apples after 5 seconds and may not always appear.




————————2048 user manual————————
Starting the Game:
- Launch the 2048 game from the main menu of the Classic Games Collection.
- The game board is a 4x4 grid, initially with two numbered tiles.
- Press start/restart button to start the game.


Controls:
- Use the arrow keys or WASD keys on your keyboard to slide the tiles in the desired direction. Each press of a key slides all tiles in the chosen direction, combining tiles of the same number.


Gameplay Mechanics:
- When two tiles with the same number collide, they merge into one tile with their total value.
- Every time you make a valid move, a new tile (either 2 or 4) randomly appears on an empty spot on the grid.
- Your score increases with every successful merge, the game tracks and keeps your best score.


Winning the Game:
- You win the game by creating a tile with the number 2048.
- After winning, you can continue playing to achieve higher scores. Even if the grids become filled and the tiles are unable to be merged, you will not face defeat. Simply press the restart button to commence a new round.


Losing the Game:
- The game ends when the grid is full, and there are no possible merges left.




————————Tic-Tac-Toe User Manual————————
Starting the Game
-  Launch the Tic-Tac-Toe game from the main menu of the Classic Games Collection. You can choose the game's difficulty and grid size through the settings menu.


Game Modes:
 - Player vs Player (PvP): Two players take turns marking spaces in the 3x3 (or 4x4 or 5x5) grid.
 - Player vs Machine (PvM): A single player competes against the game's machine. The difficulty level for the machine can be adjusted in the settings.


Making a Move:
 - Click on an empty square to place your symbol (X or O) in that space.
 - Players alternate turns until one wins or the game ends in a tie.


Winning the Game:
 - You win by being the first to get three of your symbols, either X or O, in a row vertically, horizontally, or diagonally.
 - If all squares are filled without any player achieving this, the game ends in a tie.




————————Fifteen Puzzle User Manual————————

Starting the Game
- Upon starting the Fifteen Puzzle, you can choose the difficulty level: Easy or Hard. The Easy level offers a puzzle that can be solved in fewer moves, while the Hard level provides a more challenging experience with a completely randomized setup.


Interacting with Tiles:
 - Click on a tile adjacent to the empty space to slide it into that space.
 - Continue sliding tiles until you achieve the correct numerical sequence.


Tracking Moves:
 - The game tracks the number of moves you make. The fewer moves you use to solve the puzzle, the better your skill level.


Additional Information
- If you encounter any issues or have questions not covered in this manual, the "Get Help" button provides additional guidance and troubleshooting advice.
- If the number is in the correct position, it will be green, otherwise it will be purple.



————————Configuration————————

The games come with default settings suitable for immediate play. However, you can customize game settings such as difficulty level and dark or light mode through the in-game menus.

