#include "gbalib.h"
#include "main.h"
#include "gameboard.h"
#include "dashboard.h"
#include "sprite/TILE_F.h"
#include "sprite/TILE_F_MIRRORED.h"
#include "sprite/TILE_I.h"
#include "sprite/TILE_L.h"
#include "sprite/TILE_L_MIRRORED.h"
#include "sprite/TILE_N.h"
#include "sprite/TILE_N_MIRRORED.h"
#include "sprite/TILE_P.h"
#include "sprite/TILE_P_MIRRORED.h"
#include "sprite/TILE_T.h"
#include "sprite/TILE_U.h"
#include "sprite/TILE_V.h"
#include "sprite/TILE_W.h"
#include "sprite/TILE_X.h"
#include "sprite/TILE_Y.h"
#include "sprite/TILE_Y_MIRRORED.h"
#include "sprite/TILE_Z.h"
#include "sprite/TILE_Z_MIRRORED.h"
#include "sprite/TILE_EMPTY.h"

#define ROW_CLEAR_ANIMATION_DURATION 10 // in vblank cycles

// States used when animating row clearing -- also used to pause the game during
// row clears
enum ROW_CLEAR_PHASE {
    INACTIVE,
    READY,
    ANIMATE_0,
    ANIMATE_1,
    REDRAW
};

// The number of rows that have been cleared. If this is 1, then we need to
// pause the game and shift some things down
int clearedRows = 0;
// Holds the row indices that are full and need to be cleared
int clearedRowNumbers[5];
// Holds the number of tiles in each row
int rowCount[BOARD_HEIGHT];
// The current phase of the row clearing
enum ROW_CLEAR_PHASE rowClearPhase;
// Used to time the row clear animation
int clearAnimationTimer;

// The gameboard is represented as a 2D array. Note that this array should be
// indexed first by row, then by column (y, x)
TILE board[BOARD_HEIGHT][BOARD_WIDTH];

// Clears the contents of the board to prepare a new game
void resetBoard() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = NONE;
        }
        rowCount[i] = 0;
    }
    
    clearedRows = 0;
    rowClearPhase = INACTIVE;
    clearAnimationTimer = 0;
}

// Sets a tile on the gameboard and checks for row clearing
void setTileAtPosition(TILE tile, int x, int y) {
    board[y][x] = tile;
    
    // Check if this fills a row
    rowCount[y]++;
    
    if (rowCount[y] == BOARD_WIDTH) {
        clearedRowNumbers[clearedRows] = y;
        clearedRows++;
        rowClearPhase = READY;
        speedUp();
        indicateLineClear();
    }
}

// Called after tiles have been moved in a game tick to handle row clearing
void checkRowClear() {
    if (rowClearPhase == READY) {
        if (clearedRows) {
            // give 100 points for clearing a row,
            // doubling for each additional row
            incrementScore(100 * (1 << (clearedRows - 1)));
            rowClearPhase = ANIMATE_0;
        }
    } else if (rowClearPhase == ANIMATE_1 && clearAnimationTimer == 3) {
        // Arraycopy tile data down when we're animating
        // We do this in the middle of the animation to ensure it runs, and to
        // ensure we don't waste time by doing it only once
        for (int i = 0; i < clearedRows; i++) {
            int r = clearedRowNumbers[i];
            clearedRowNumbers[i] = 0;
            for (int j = r - i; j < BOARD_HEIGHT - 3; j++) {
                rowCount[j] = rowCount[j + 1];
                for (int k = 0; k < BOARD_WIDTH; k++) {
                    board[j][k] = board[j + 1][k];
                }
            }
        }
        clearedRows = 0;
    }
}

// Used after a row is cleared to make sure that all tiles have been drawn in
// their updated state
void redrawEntireBoard() {
    for (int i = 0; i < BOARD_HEIGHT_VISIBLE; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            drawTile(j, i, tileSpriteForType(tileAtPosition(j, i)));
        }
    }
}

// Called in the onDraw phase of main to animate any row clearings. If there
// aren't any filled rows, then this function does nothing.
void drawRowClear() {
    if (rowClearPhase == ANIMATE_0) {
        for (int i = 0; i < clearedRows; i++) {
            int r = clearedRowNumbers[i];
            drawRect(
                BOARD_OFFSET_X_PX,
                TILE_SIZE_PX * (BOARD_HEIGHT_VISIBLE - r - 1),
                TILE_SIZE_PX,
                BOARD_WIDTH * TILE_SIZE_PX,
                WHITE);
        }
        rowClearPhase = ANIMATE_1;
    } else if (rowClearPhase == ANIMATE_1) {
        clearAnimationTimer++;
        if (clearAnimationTimer >= ROW_CLEAR_ANIMATION_DURATION) {
            rowClearPhase = REDRAW;
        }
    } else if (rowClearPhase == REDRAW) {
        redrawEntireBoard();
        clearAnimationTimer = 0;
        rowClearPhase = INACTIVE;
    }
}

// Let other functions know to pause gameplay for a little bit while we handle
// row clearing
bool isClearingRow() {
    return rowClearPhase != INACTIVE;
}

// Used to end the game if the last tile didn't go outside the bounds of the
// board, but the next piece can't even be put on the board because it collides
// with tiles
void forceGameOver() {
    rowCount[BOARD_HEIGHT - 3] = 1;
}

// Checks if a tile has been placed outside the bounds of the gameboard
bool isGameOver() {
    return rowCount[BOARD_HEIGHT - 3] > 0;
}

// Whether or not to show the danger indicator
bool isInDanger() {
    return rowCount[BOARD_HEIGHT - 10] > 0;
}

// Whether or not to show the critical indicator
bool isInCritical() {
    return rowCount[BOARD_HEIGHT - 7] > 0;
}

// Gets the tile type at a given position. May be NONE.
enum TILE tileAtPosition(int x, int y) {
    if (x >= BOARD_WIDTH || x < 0 || y >= BOARD_HEIGHT || y < 0) {
        return NONE;
    }
    
    return board[y][x];
}

// Checks whether there's a tile on the board at a position
bool hasTileAtPosition(int x, int y) {
    return tileAtPosition(x, y) != NONE;
}

// Draws a tile on the gameboard. x and y are coordinates with respect to the
// gameboard where the bottom left corner is (0, 0)
void drawTile(int x, int y, const unsigned short* tile) {
    drawTileAtPixel(TILE_SIZE_PX * x + BOARD_OFFSET_X_PX, TILE_SIZE_PX * (BOARD_HEIGHT_VISIBLE - 1 - y), tile);
}

// Implementation of tile drawing. x and y are pixel offsets with the standard
// gameboy coordinate scheme
void drawTileAtPixel(int x, int y, const unsigned short* tile) {
    drawImage3(x, y, TILE_SIZE_PX, TILE_SIZE_PX, tile);
}

// Gets the tile sprite associated with a tile type
const unsigned short* tileSpriteForType(enum TILE type) {
    switch(type) {
        case F: return TILE_F_data;
        case F_M: return TILE_F_MIRRORED_data;
        case I: return TILE_I_data;
        case N: return TILE_N_data;
        case N_M: return TILE_N_MIRRORED_data;
        case L: return TILE_L_data;
        case L_M: return TILE_L_MIRRORED_data;
        case P: return TILE_P_data;
        case P_M: return TILE_P_MIRRORED_data;
        case T: return TILE_T_data;
        case U: return TILE_U_data;
        case V: return TILE_V_data;
        case W: return TILE_W_data;
        case X: return TILE_X_data;
        case Y: return TILE_Y_data;
        case Y_M: return TILE_Y_MIRRORED_data;
        case Z: return TILE_Z_data;
        case Z_M: return TILE_Z_MIRRORED_data;
        case NONE:
        default: return TILE_EMPTY_data;
    }
}
