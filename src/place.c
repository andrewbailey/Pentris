#include "gbalib.h"
#include "place.h"
#include "gameboard.h"
#include "pentomino.h"
#include "dashboard.h"
#include "main.h"

#define D_SCORE_SOFT_DROP 1
#define D_SCORE_HARD_DROP 2

static PENTOMINO current;
static PENTOMINO prev;

// Used to force the tile to be commited after we draw it if the player
// preformed a hard drop
bool hardDrop = FALSE;

static void commit();

// Used to avoid repeated calls when popping the random stack
bool isGameStarted() {
    return current.tile != NONE;
}

// Called when the game ends to reset place.c's state
void resetPlacer() {
    current = getPentomino(NONE);
    prev = getPentomino(NONE);
}

// Pops a piece from the dashboard and begins to place it on the board
void popPiece() {
    if (!isGameOver()) {
        current = getPentominoWithPos(
                getNext(), BOARD_WIDTH / 2 - 1, BOARD_HEIGHT - 4);
        prev = current;
        prev.pos.y++;
        generateNext();
        if (!isPentominoValid(current)) {
            forceGameOver();
        }
    }
}

// Called in the onUpdate phase in main to move the tile around based on user
// button presses (only for the D-pad). This function doesn't handle gravity
void handleTileMove() {
    prev = current;
    if (keyPressStart(BUTTON_LEFT)) {
        current = moveLeft(current);
    } else if (keyPressStart(BUTTON_RIGHT)) {
        current = moveRight(current);
    } else if (keyPressStart(BUTTON_DOWN)) {
        current = moveDown(current);
        if (current.pos.y == prev.pos.y) {
            commit();
        } else {
            incrementScore(D_SCORE_SOFT_DROP);
        }
    }
}

// Called in onUpdate when the 'A' button is pressed to preform a hard
// drop of the tile
void handleTileDrop() {
    int count = 0;
    PENTOMINO moving = current;
    while (moving.pos.y == current.pos.y - count) {
        moving = moveDown(moving);
        count++;
    }
    prev = current;
    current = moving;
    incrementScore(2 * count);
    hardDrop = TRUE;
}

// Called in onUpdate when one of the bumpers is pressed to handle tile
// rotation
void handleTileRotate() {
    prev = current;
    if (keyPressStart(BUTTON_R)) {
        current = rotateCW(current);
    } else if (keyPressStart(BUTTON_L)) {
        current = rotateCCW(current);
    }
}

// Called in onUpdate when the user isn't pressing a button so that tile
// gravity may be applied. Main.c is responsible for timing this call and
// handles game progression.
void applyGravity() {
    prev = current;
    current = moveDown(current);
    if (current.pos.y == prev.pos.y) {
        commit();
    }
}

// When a tile is attempted to be moved down 1 tile (whether by the player or
// by graviity), this function is called to commit its tiles to the gameboard,
// and to prepare the next piece from the random stack so that it can be placed
static void commit() {
    // The order in which these tiles are committed matters, because when
    // gameboard checks cleared rows, it expects them to be sorted. So here, 
    // we need to set the tiles from bottom to top.
    
    // Set index 6 & 7
    if (hasTile(current, 7)) {
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(current, 6)) {
            setTileAtPosition(current.tile, current.pos.x, current.pos.y - 2);
        }
        setTileAtPosition(current.tile, current.pos.x, current.pos.y - 1);
    }
    
    // Set index 8
    if (hasTile(current, 8)) {
        setTileAtPosition(current.tile, current.pos.x + 1, current.pos.y - 1);
    }
    
    // Set index 11
    if (hasTile(current, 11)) {
        setTileAtPosition(current.tile, current.pos.x - 1, current.pos.y - 1);
    }
    
    // Set the center tile since it's always there
    setTileAtPosition(current.tile, current.pos.x, current.pos.y);
    
    // Set index 3 & 4
    if (hasTile(current, 4)) {
        setTileAtPosition(current.tile, current.pos.x + 1, current.pos.y);
        // If there's not a tile in index 4, then there can't be one in index 3
        if (hasTile(current, 3)) {
            setTileAtPosition(current.tile, current.pos.x + 2, current.pos.y);
        }
    }
    
    // Set index 9 & 10
    if (hasTile(current, 10)) {
        setTileAtPosition(current.tile, current.pos.x - 1, current.pos.y);
        // If there's not a tile in index 10, then there can't be one in index 9
        if (hasTile(current, 9)) {
            setTileAtPosition(current.tile, current.pos.x - 2, current.pos.y);
        }
    }
    
    // Set index 2
    if (hasTile(current, 2)) {
        setTileAtPosition(current.tile, current.pos.x - 1, current.pos.y + 1);
    }
    
    // Set index 5
    if (hasTile(current, 5)) {
        setTileAtPosition(current.tile, current.pos.x + 1, current.pos.y + 1);
    }
    
    // Set index 0 & 1
    if (hasTile(current, 1)) {
        setTileAtPosition(current.tile, current.pos.x, current.pos.y + 1);
        // If there's not a tile in index 1, then there can't be one in index 0
        if (hasTile(current, 0)) {
            setTileAtPosition(current.tile, current.pos.x, current.pos.y + 2);
        }
    }
    
    // Once the tile has been placed, get the next piece
    popPiece();
    prev = current;
}

// Draws the pentomino that the user is currently placing
void drawPlacingTile() {
    erasePentomino(prev);
    drawPentominoOnBoard(current);
    if (hardDrop) {
        commit();
        hardDrop = FALSE;
    }
}
