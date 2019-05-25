/*
 * Pentomino tile arrangements are kept as unsigned shorts where each bit
 * corresponds to a position on the following grid:
 * 
 *      |     |     |     |     
 *      |     |  0  |     |     
 *      |     |     |     |     
 * -----------------------------
 *      |     |     |     |     
 *      |  2  |  1  |  5  |     
 *      |     |     |     |     
 * -----------------------------
 *      |     |     |     |     
 *   9  | 10  |     |  4  |  3   
 *      |     |     |     |     
 * -----------------------------
 *      |     |     |     |     
 *      | 11  |  7  |  8  |     
 *      |     |     |     |     
 * -----------------------------
 *      |     |     |     |     
 *      |     |  6  |     |     
 *      |     |     |     |     
 * 
 * where index 0 represents the far right bit. We don't need to assign indices
 * to corners, because it's impossible for a pentomino to have a tile in those
 * sections if it's centered. Similarly, we can guarentee that the center tile
 * is always filled and therefore don't need to assign it an index.
 * 
 * When rotating pentominoes clockwise, we simply swap the following bits:
 * 0 -> 3        1  -> 4        2  -> 5
 * 3 -> 6        4  -> 7        5  -> 8
 * 6 -> 9        7  -> 10       8  -> 11
 * 9 -> 0        10 -> 1        11 -> 2
 * 
 * or more easily: pentomino << 3 | pentomino >> 9
 * 
 * To rotate pentominoes counterclockwise, we simply do the opposite.
 * 
 * The tile arrangement of pentominoes (further referred to as a pentomino's
 * data) is defined below as constants under this scheme:
 */

#define TILE_F_DATA 0x96
#define TILE_FM_DATA 0x4A2
#define TILE_I_DATA 0xC3
#define TILE_L_DATA 0x183
#define TILE_LM_DATA 0x883
#define TILE_N_DATA 0x1E
#define TILE_NM_DATA 0x622
#define TILE_P_DATA 0xC84
#define TILE_PM_DATA 0x1B0
#define TILE_T_DATA 0xC14
#define TILE_U_DATA 0x886
#define TILE_V_DATA 0x603
#define TILE_W_DATA 0x8B0
#define TILE_X_DATA 0x492
#define TILE_Y_DATA 0x8C2
#define TILE_YM_DATA 0xD2
#define TILE_Z_DATA 0x186
#define TILE_ZM_DATA 0x8A2

typedef struct {
    POINT pos;
    TILE tile;
    unsigned short data;
} PENTOMINO;

bool hasTile(PENTOMINO pent, int tileIndex);
void drawPentominoOnBoard(PENTOMINO pent);
void drawPentominoAtOffset(int x, int y, PENTOMINO pent);
void erasePentomino(PENTOMINO pent);
bool isPentominoValid(PENTOMINO pent);
PENTOMINO rotateCW(PENTOMINO pent);
PENTOMINO rotateCCW(PENTOMINO pent);
PENTOMINO moveDown(PENTOMINO pent);
PENTOMINO moveLeft(PENTOMINO pent);
PENTOMINO moveRight(PENTOMINO pent);
PENTOMINO getPentomino(TILE tileType);
PENTOMINO getPentominoWithPos(TILE tileType, int x, int y);
