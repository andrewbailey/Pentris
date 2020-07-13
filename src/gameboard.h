#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#define BOARD_HEIGHT 24 // the top 4 rows are invisible
#define BOARD_HEIGHT_VISIBLE 20 // excludes the top 4 rows
#define BOARD_WIDTH 12
#define BOARD_OFFSET_X_PX 8
#define TILE_SIZE_PX 8

typedef struct {
    int x;
    int y;
} POINT;

typedef enum TILE {
    NONE = 0,
    F = 1,
    F_M = 2,
    I = 3,
    L = 4,
    L_M = 5,
    N = 6,
    N_M = 7,
    P = 8,
    P_M = 9,
    T = 10,
    U = 11,
    V = 12,
    W = 13,
    X = 14,
    Y = 15,
    Y_M = 16,
    Z = 17,
    Z_M = 18
} TILE;

void resetBoard();
void setTileAtPosition(TILE tile, int x, int y);
void checkRowClear();
void redrawEntireBoard();
void drawRowClear();
bool isClearingRow();
void forceGameOver();
bool isInDanger();
bool isInCritical();
bool isGameOver();
enum TILE tileAtPosition(int x, int y);
bool hasTileAtPosition(int x, int y);
void drawTile(int x, int y, const unsigned short* tile);
const unsigned short* tileSpriteForType(enum TILE type);
void drawTileAtPixel(int x, int y, const unsigned short* tile);

#endif //GAMEBOARD_H
