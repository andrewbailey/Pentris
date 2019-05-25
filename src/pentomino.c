#include "gbalib.h"
#include "gameboard.h"
#include "pentomino.h"

// Private prototypes
bool _isTilePosValid(int x, int y);
bool _isPentominoBottomValid(PENTOMINO pent);
bool _isPentominoLeftValid(PENTOMINO pent);
bool _isPentominoRightValid(PENTOMINO pent);

// Returns whether a pentomino has a tile at a specified tile index.
// See pentomino.h for a list of indices and an explanation of why they're used
bool hasTile(PENTOMINO pent, int tileIndex) {
    return (pent.data & (1 << tileIndex)) != 0;
}

// Private implementaion of drawing a pentomino
// cX and cY represent the pixel coordinate of the top left corner of the center
// tile in the pentomino
void _drawPentomino(int cX, int cY, PENTOMINO pent) {
    const unsigned short *tile = tileSpriteForType(pent.tile);
    
    // Draw the center tile since it's always there
    drawTileAtPixel(cX, cY, tile);
    
    // Draw index 0 & 1
    if (hasTile(pent, 1)) {
        drawTileAtPixel(cX, cY - 1 * TILE_SIZE_PX, tile);
        // If there's not a tile in index 1, then there can't be a tile
        // in index 0
        if (hasTile(pent, 0)) {
            drawTileAtPixel(cX, cY - 2 * TILE_SIZE_PX, tile);    
        }
    }
    
    // Draw index 3 & 4
    if (hasTile(pent, 4)) {
        drawTileAtPixel(cX + 1 * TILE_SIZE_PX, cY, tile);
        // If there's not a tile in index 4, then there can't be one in index 3
        if (hasTile(pent, 3)) {
            drawTileAtPixel(cX + 2 * TILE_SIZE_PX, cY, tile);
        }
    }
    
    // Draw index 6 & 7
    if (hasTile(pent, 7)) {
        drawTileAtPixel(cX, cY + 1 * TILE_SIZE_PX, tile);
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(pent, 6)) {
            drawTileAtPixel(cX, cY + 2 * TILE_SIZE_PX, tile);
        }
    }
    
    // Draw index 9 & 10
    if (hasTile(pent, 10)) {
        drawTileAtPixel(cX - 1 * TILE_SIZE_PX, cY, tile);
        // If there's not a tile in index 10, then there can't be one in index 9
        if (hasTile(pent, 9)) {
            drawTileAtPixel(cX - 2 * TILE_SIZE_PX, cY, tile);
        }
    }
    
    // Draw index 2
    if (hasTile(pent, 2)) {
        drawTileAtPixel(cX - 1 * TILE_SIZE_PX, cY - 1 * TILE_SIZE_PX, tile);
    }
    
    // Draw index 5
    if (hasTile(pent, 5)) {
        drawTileAtPixel(cX + 1 * TILE_SIZE_PX, cY - 1 * TILE_SIZE_PX, tile);
    }
    
    // Draw index 8
    if (hasTile(pent, 8)) {
        drawTileAtPixel(cX + 1 * TILE_SIZE_PX, cY + 1 * TILE_SIZE_PX, tile);
    }
    
    // Draw index 11
    if (hasTile(pent, 11)) {
        drawTileAtPixel(cX - 1 * TILE_SIZE_PX, cY + 1 * TILE_SIZE_PX, tile);
    }
    
}

// Draws a pentomino at the position on the poard that its pos field
// corresponds to
void drawPentominoOnBoard(PENTOMINO pent) {
    int cX = pent.pos.x * TILE_SIZE_PX + BOARD_OFFSET_X_PX;
    int cY = (BOARD_HEIGHT - 5 - pent.pos.y) * TILE_SIZE_PX;

    _drawPentomino(cX, cY, pent);
}

// Draws a pentomino at a given pixel position. x and y here represent the
// pixel coordinate of the entire pentomino
void drawPentominoAtOffset(int x, int y, PENTOMINO pent) {
    int cX = x + (pent.pos.x + 2) * TILE_SIZE_PX;
    int cY = y + (pent.pos.y + 2) * TILE_SIZE_PX;

    _drawPentomino(cX, cY, pent);
}

// Erases a pentomino from the board
void erasePentomino(PENTOMINO pent) {
    pent.tile = NONE;
    drawPentominoOnBoard(pent);
}

// Checks if a pentomino is valid after being moved down
// This is a slightly optimized computation of isPentominoValid that ignores
// certain tile positions and prioritizes others
bool _isPentominoBottomValid(PENTOMINO pent) {
int cX = pent.pos.x;
    int cY = pent.pos.y;
    
    // Make sure that the center is valid first
    if (hasTileAtPosition(cX, cY) || cY < 0) {
        return FALSE;
    }
    
    // Check index 6 & 7
    if (hasTile(pent, 7)) {
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(pent, 6)
                && (hasTileAtPosition(cX, cY - 2) || cY - 2 < 0)) {
            return FALSE;
        }
        if (hasTileAtPosition(cX, cY - 1) || cY - 1 < 0) {
            return FALSE;
        }
    }
    
    // Check index 8
    if (hasTile(pent, 8) && (hasTileAtPosition(cX + 1, cY - 1) || cY - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 11
    if (hasTile(pent, 11) && (hasTileAtPosition(cX - 1, cY - 1) || cY - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 3 & 4
    if (hasTile(pent, 4)) {
        // If there's not a tile in index 4, then there can't be one in index 3
        if (hasTile(pent, 3) && (hasTileAtPosition(cX + 2, cY))) {
            return FALSE;
        }
        if (hasTileAtPosition(cX + 1, cY)) {
            return FALSE;
        }
    }
    
    // Check index 9 & 10
    if (hasTile(pent, 10)) {
        // If there's not a tile in index 10, then there can't be one in index 9
        if (hasTile(pent, 9) && (hasTileAtPosition(cX - 2, cY))) {
            return FALSE;
        }        
        if (hasTileAtPosition(cX - 1, cY)) {
            return FALSE;
        }
    }

    // Check index 2
    if (hasTile(pent, 2) && (hasTileAtPosition(cX - 1, cY + 1))) {
        return FALSE;
    }
    
    // Check index 5
    if (hasTile(pent, 5) && (hasTileAtPosition(cX + 1, cY + 1))) {
        return FALSE;
    }
    
    return TRUE;
}

// Checks if a pentomino is valid after being moved left
// This is a slightly optimized computation of isPentominoValid that ignores
// certain tile positions and prioritizes others
bool _isPentominoLeftValid(PENTOMINO pent) {
    int cX = pent.pos.x;
    int cY = pent.pos.y;
    
    // Make sure that the center is valid first
    if (cX < 0 || cX >= BOARD_WIDTH || hasTileAtPosition(cX, cY)) {
        return FALSE;
    }
    
    // Check index 9 & 10
    if (hasTile(pent, 10)) {
        // If there's not a tile in index 10, then there can't be one in index 9
        if (hasTile(pent, 9)
                && (hasTileAtPosition(cX - 2, cY) || cX - 2 < 0)) {
            return FALSE;
        }
        if (hasTileAtPosition(cX - 1, cY) || cX - 1 < 0) {
            return FALSE;
        }
    }
    
    // Check index 2
    if (hasTile(pent, 2)
            && (hasTileAtPosition(cX - 1, cY + 1) || cX - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 11
    if (hasTile(pent, 11) && (hasTileAtPosition(cX - 1, cY - 1) || cX - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 0 & 1
    if (hasTile(pent, 1)) {
        // If there's not a tile in index 1, then there can't be one in index 0
        if (hasTile(pent, 0) && hasTileAtPosition(cX, cY + 2)) {
            return FALSE;
        }
        if (hasTileAtPosition(cX, cY + 1)) {
            return FALSE;
        }
    }
    
    // Check index 6 & 7
    if (hasTile(pent, 7)) {
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(pent, 6) && (hasTileAtPosition(cX, cY - 2))) {
            return FALSE;
        }
        if (hasTileAtPosition(cX, cY - 1)) {
            return FALSE;
        }
    }
    
    // Check index 5
    if (hasTile(pent, 5)
            && (hasTileAtPosition(cX + 1, cY + 1) || cX + 1 >= BOARD_WIDTH)) {
        return FALSE;
    }
    
    // Check index 8
    if (hasTile(pent, 8) && (hasTileAtPosition(cX + 1, cY - 1)
            || cX + 1 >= BOARD_WIDTH)) {
        return FALSE;
    }
    
    return TRUE;
}

// Checks if a pentomino is valid after being moved right
// This is a slightly optimized computation of isPentominoValid that ignores
// certain tile positions and prioritizes others
bool _isPentominoRightValid(PENTOMINO pent) {
    int cX = pent.pos.x;
    int cY = pent.pos.y;
    
    // Make sure that the center is valid first
    if (cX < 0 || cX >= BOARD_WIDTH || hasTileAtPosition(cX, cY)) {
        return FALSE;
    }
    
    // Check index 3 & 4
    if (hasTile(pent, 4)) {
        // If there's not a tile in index 4, then there can't be one in index 3
        if (hasTile(pent, 3)
                && (hasTileAtPosition(cX + 2, cY) || cX + 2 >= BOARD_WIDTH)) {
            return FALSE;
        }
        if (hasTileAtPosition(cX + 1, cY) || cX + 1 >= BOARD_WIDTH) {
            return FALSE;
        }
    }
    
    // Check index 5
    if (hasTile(pent, 5)
            && (hasTileAtPosition(cX + 1, cY + 1) || cX + 1 >= BOARD_WIDTH)) {
        return FALSE;
    }
    
    // Check index 8
    if (hasTile(pent, 8) && (hasTileAtPosition(cX + 1, cY - 1)
            || cX + 1 >= BOARD_WIDTH)) {
        return FALSE;
    }
    
    // Check index 0 & 1
    if (hasTile(pent, 1)) {
        // If there's not a tile in index 1, then there can't be one in index 0
        if (hasTile(pent, 0) && hasTileAtPosition(cX, cY + 2)) {
            return FALSE;
        }
        if (hasTileAtPosition(cX, cY + 1)) {
            return FALSE;
        }
    }
    
    // Check index 6 & 7
    if (hasTile(pent, 7)) {
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(pent, 6) && (hasTileAtPosition(cX, cY - 2))) {
            return FALSE;
        }
        if (hasTileAtPosition(cX, cY - 1)) {
            return FALSE;
        }
    }
    
    // Check index 2
    if (hasTile(pent, 2)
            && (hasTileAtPosition(cX - 1, cY + 1) || cX - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 11
    if (hasTile(pent, 11)
            && (hasTileAtPosition(cX - 1, cY - 1) || cX - 1 < 0 || cY - 1 < 0)) {
        return FALSE;
    }
    
    return TRUE;
}

// Checks all tiles on a pentomino to make sure that it doens't go outside the
// bounds of the board and that it doesn't collide with a tile already on the
// board
bool isPentominoValid(PENTOMINO pent) {
    int cX = pent.pos.x;
    int cY = pent.pos.y;
    
    // Make sure that the center is valid first
    if (cX < 0 || cX >= BOARD_WIDTH || hasTileAtPosition(cX, cY) || cY < 0) {
        return FALSE;
    }
    
    // Check index 0 & 1
    if (hasTile(pent, 1)) {
        if (hasTileAtPosition(cX, cY + 1)) {
            return FALSE;
        }
        // If there's not a tile in index 1, then there can't be a tile
        // in index 0
        if (hasTile(pent, 0) && hasTileAtPosition(cX, cY + 2)) {
            return FALSE;
        }
    }
    
    // Check index 3 & 4
    if (hasTile(pent, 4)) {
        if (hasTileAtPosition(cX + 1, cY) || cX + 1 >= BOARD_WIDTH) {
            return FALSE;
        }
        // If there's not a tile in index 4, then there can't be one in index 3
        if (hasTile(pent, 3)
                && (hasTileAtPosition(cX + 2, cY) || cX + 2 >= BOARD_WIDTH)) {
            return FALSE;
        }
    }
    
    // Check index 6 & 7
    if (hasTile(pent, 7)) {
        if (hasTileAtPosition(cX, cY - 1) || cY - 1 < 0) {
            return FALSE;
        }
        // If there's not a tile in index 7, then there can't be one in index 6
        if (hasTile(pent, 6)
                && (hasTileAtPosition(cX, cY - 2) || cY - 2 < 0)) {
            return FALSE;
        }
    }
    
    // Check index 9 & 10
    if (hasTile(pent, 10)) {
        if (hasTileAtPosition(cX - 1, cY) || cX - 1 < 0) {
            return FALSE;
        }
        // If there's not a tile in index 10, then there can't be one in index 9
        if (hasTile(pent, 9)
                && (hasTileAtPosition(cX - 2, cY) || cX - 2 < 0)) {
            return FALSE;
        }
    }

    // Check index 2
    if (hasTile(pent, 2)
            && (hasTileAtPosition(cX - 1, cY + 1) || cX - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 5
    if (hasTile(pent, 5)
            && (hasTileAtPosition(cX + 1, cY + 1) || cX + 1 >= BOARD_WIDTH)) {
        return FALSE;
    }
    
    // Check index 8
    if (hasTile(pent, 8) && (hasTileAtPosition(cX + 1, cY - 1)
            || cX + 1 >= BOARD_WIDTH || cY - 1 < 0)) {
        return FALSE;
    }
    
    // Check index 11
    if (hasTile(pent, 11)
            && (hasTileAtPosition(cX - 1, cY - 1) || cX - 1 < 0 || cY - 1 < 0)) {
        return FALSE;
    }
    
    return TRUE;
}

// Returns a copy of the pentomino after it has been rotated clockwise.
// If the rotation isn't valid and would either go outside the bounds of the 
// board, or would conflict with a placed tile, then the original pentomino
// is returned
PENTOMINO rotateCW(PENTOMINO pent) {
    PENTOMINO rotated = pent;
    
    unsigned short data = rotated.data;
    rotated.data = (data << 3 | data >> 9) & 0xFFF;
    
    if (isPentominoValid(rotated)) {
        return rotated;
    } else {
        return pent;
    }
}

// Returns a copy of the pentomino after it has been rotated counterclockwise.
// If the rotation isn't valid and would either go outside the bounds of the 
// board, or would conflict with a placed tile, then the original pentomino
// is returned
PENTOMINO rotateCCW(PENTOMINO pent) {
    PENTOMINO rotated = pent;
    
    unsigned short data = rotated.data;
    rotated.data = (data >> 3 | data << 9) & 0xFFF;
    
    if (isPentominoValid(rotated)) {
        return rotated;
    } else {
        return pent;
    }
}

// Returns a copy of the pentomino after it has been moved down by 1 tile.
// If the translation isn't valid and would either go outside the bounds of the 
// board, or would conflict with a placed tile, then the original pentomino
// is returned
PENTOMINO moveDown(PENTOMINO pent) {
    PENTOMINO shifted = pent;
    shifted.pos.y -= 1;
    
    if (_isPentominoBottomValid(shifted)) {
        return shifted;
    } else {
        return pent;
    }
}

// Returns a copy of the pentomino after it has been moved left by 1 tile.
// If the translation isn't valid and would either go outside the bounds of the 
// board, or would conflict with a placed tile, then the original pentomino
// is returned
PENTOMINO moveLeft(PENTOMINO pent) {
    PENTOMINO shifted = pent;
    shifted.pos.x -= 1;
    
    if (_isPentominoLeftValid(shifted)) {
        return shifted;
    } else {
        return pent;
    }
}

// Returns a copy of the pentomino after it has been moved right by 1 tile.
// If the translation isn't valid and would either go outside the bounds of the 
// board, or would conflict with a placed tile, then the original pentomino
// is returned
PENTOMINO moveRight(PENTOMINO pent) {
    PENTOMINO shifted = pent;
    shifted.pos.x += 1;
    
    if (_isPentominoRightValid(shifted)) {
        return shifted;
    } else {
        return pent;
    }
}

// Returns a populated pentomino with predefined data. The position of this tile
// will be (0, 0)
PENTOMINO getPentomino(TILE tileType) {
    return getPentominoWithPos(tileType, 0, 0);
}

// Returns a populated pentomino with predifined data and a specified coordinate
PENTOMINO getPentominoWithPos(TILE tileType, int x, int y) {
    PENTOMINO val = {{x,y}, tileType, 0};
    switch (tileType) {
        case F: 
            val.data = TILE_F_DATA;
            break;
        case F_M:
            val.data = TILE_FM_DATA;
            break;
        case I:
            val.data = TILE_I_DATA;
            break;
        case N:
            val.data = TILE_N_DATA;
            break;
        case N_M:
            val.data = TILE_NM_DATA;
            break;
        case L:
            val.data = TILE_L_DATA;
            break;
        case L_M:
            val.data = TILE_LM_DATA;
            break;
        case P:
            val.data = TILE_P_DATA;
            break;
        case P_M:
            val.data = TILE_PM_DATA;
            break;
        case T:
            val.data = TILE_T_DATA;
            break;
        case U:
            val.data = TILE_U_DATA;
            break;
        case V:
            val.data = TILE_V_DATA;
            break;
        case W:
            val.data = TILE_W_DATA;
            break;
        case X:
            val.data = TILE_X_DATA;
            break;
        case Y:
            val.data = TILE_Y_DATA;
            break;
        case Y_M:
            val.data = TILE_YM_DATA;
            break;
        case Z:
            val.data = TILE_Z_DATA;
            break;
        case Z_M:
            val.data = TILE_ZM_DATA;
            break;
        default:
            break;
    }
    return val;
}
