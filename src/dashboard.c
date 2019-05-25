#include <stdlib.h>
#include <stdio.h>

#include "gbalib.h"
#include "gameboard.h"
#include "dashboard.h"
#include "pentomino.h"
#include "place.h"

#include "info/ALERT_WARNING.h"
#include "info/ALERT_DANGER.h"
#include "info/SINGLE.h"
#include "info/DOUBLE.h"
#include "info/TRIPLE.h"
#include "info/TETRIS.h"
#include "info/PENTRIS.h"

#define MAX_SCORE_DIGITS 15
#define SCORE_DIGITS_FORMAT "8"
#define BG_COLOR (unsigned short) 0x1084

#define LINE_INDICATOR_TIME 60
#define LINE_INDICATOR_X 131
#define LINE_INDICATOR_Y 48
#define SCORE_AREA_X 122
#define SCORE_AREA_Y 30
#define NEXT_BOX_X 152
#define NEXT_BOX_Y 76
#define WARNING_X 132
#define DANGER_X 137
#define ALERT_Y 139

// The player's score
int score = 0;
// The next pentomino
enum TILE nextPentomino = NONE;
// Flags to cull unnecessary redrawing
bool redrawScore = FALSE;
bool redrawNext = FALSE;
bool wasInDanger = FALSE;
bool wasInCritical = FALSE;

// The number of lines that were cleared (used to show a single/double/triple/
// tetris/pentris indicator)
int clearedLines;
// Used to keep track of how long the cleared lines indicator has been shown
// and to clear it after LINE_INDICATOR_TIME vblanks
int clearedLinesTimer;

// Called externally to increment the score and redraw it on the next draw cycle
void incrementScore(int dScore) {
    setScore(score + dScore);
}

// Implementaion to set score and redraw it on the next draw cycle
void setScore(int newScore) {
    score = newScore;
    redrawScore = TRUE;
}

// Prepares the next pentomino on the random stack
void generateNext() {
    if (redrawNext && isGameStarted()) {
        return;
    }
    redrawNext = TRUE;
    // If you want to rig the game, comment these lines out:
    int random = rand();
    nextPentomino = random % 18 + 1;
    
    // If you only want to be given line pieces, uncomment this line:
    //nextPentomino = I;
    // If you want to cycle through all the pentominoes, uncomment this line:
    //nextPentomino = (nextPentomino) % 18 + 1;
}

// Called in gameboard.c when lines are cleared so that an accurate lin clear
// indicator can be drawn on the next draw cycle
void indicateLineClear() {
    clearedLines++;
}

// Pops the random pentomino stack
enum TILE getNext() {
    enum TILE next = nextPentomino;
    generateNext();
    return next;
}

// Simple switch block to get the line clear sprite for a given number of lines
// being cleared
const unsigned short* getLineClearIndicator() {
    switch(clearedLines) {
        case 1: return SINGLE_data;
        case 2: return DOUBLE_data;
        case 3: return TRIPLE_data;
        case 4: return TETRIS_data;
        case 5:
        default: return PENTRIS_data;
    }
}

// Used internally to remove the warning indicator from the screen
void _clearWarning() {
    drawRect(
        WARNING_X,
        ALERT_Y,
        ALERT_WARNING_HEIGHT,
        ALERT_WARNING_WIDTH,
        BG_COLOR);
}

// Since pentomino data has the restriction that the center tile must always
// be enabled, not all tiles can have their center of mass placed at the center
// of the grid described in pentomino.h. Because of this, certain tiles appear
// off-center when rendered in the 'next' window. This function is called to
// return a hard-coded pixel offset to apply to the next pentomino in the random
// stack to improve the center alignment with the frame.
POINT _getNextAlignmentOffset() {
    POINT p = {0, 0};
    switch (nextPentomino) {
        case L:
        case N:
            p.x = -4;
            p.y = 4;
            break;
        case L_M:
        case N_M:
            p.x = 4;
            p.y = 4;
            break;
        case U:
        case P:
            p.x = 4;
            break;
        case P_M:
            p.x = -4;
            break;
        case V:
            p.x = 8;
            p.y = 8;
            break;
        case Y:
            p.x = 4;
            p.y = -4;
            break;
        case Y_M:
            p.x = -4;
            p.y = -4;
            break;
        default:
            // Any pentominoes not enumerated above have their center tile
            // at the center of gravity, and therefore don't need an offset
            break;
    }
    return p;
}

// Handles the magic of dashboard.c. Called in main in the onDraw phase to
// update all the information in the right panel. Fields that aren't changed
// are not updated to cull additional drawing work
void drawDash() {
    if (redrawScore) {
        // clear the frame's background
        drawRect(SCORE_AREA_X, SCORE_AREA_Y, 8, MAX_SCORE_DIGITS * 6, BG_COLOR);
        drawScore(SCORE_AREA_X, SCORE_AREA_Y);
        redrawScore = FALSE;
    }

    if (redrawNext) {
        POINT offset = _getNextAlignmentOffset();
        // clear the frame's background
        drawRect(NEXT_BOX_X, NEXT_BOX_Y, TILE_SIZE_PX * 5, TILE_SIZE_PX * 5, BG_COLOR);
        // draw the pentomino preview
        drawPentominoAtOffset(NEXT_BOX_X + offset.x, NEXT_BOX_Y + offset.y, getPentomino(nextPentomino));
        redrawNext = FALSE;
    }
    
    if (clearedLines) {
        if (clearedLinesTimer) {
            // If there's already an alert happening, and the player is REALLY
            // good, make sure to erase it before showing another message
            drawRect(
                LINE_INDICATOR_X,
                LINE_INDICATOR_Y,
                PENTRIS_HEIGHT,
                PENTRIS_WIDTH,
                BG_COLOR);
        }
        // Draw the new cleared line indicator
        drawImage3(
            LINE_INDICATOR_X,
            LINE_INDICATOR_Y,
            PENTRIS_WIDTH,
            PENTRIS_HEIGHT,
            getLineClearIndicator());
        
        // Initialize the timer and reset the cleared lines counter
        clearedLinesTimer++;
        clearedLines = 0;
    } else if (clearedLinesTimer) {
        // Wait a while before removing the cleared lines indicator
        clearedLinesTimer++;
        if (clearedLinesTimer >= LINE_INDICATOR_TIME) {
            drawRect(
                LINE_INDICATOR_X,
                LINE_INDICATOR_Y,
                PENTRIS_HEIGHT,
                PENTRIS_WIDTH,
                BG_COLOR);
            clearedLinesTimer = 0;
        }
    }
    
    // Draw alert messages
    if (isInCritical()) {
        // If we're changing alert levels, make sure to clear the previous state
        if (wasInDanger) {
            _clearWarning();
            wasInDanger = FALSE;
        }
        if (!wasInCritical) {
            // Draw the DANGER indicator
            drawImage3(
                DANGER_X,
                ALERT_Y,
                ALERT_DANGER_WIDTH,
                ALERT_DANGER_HEIGHT,
                ALERT_DANGER_data);
            wasInCritical = TRUE;
        }
    } else if (isInDanger()) {
        // If we're changing alert levels, make sure to clear the previous state
        if (wasInCritical) {
            _clearWarning();
            wasInCritical = FALSE;
        }
        if (!wasInDanger) {
            // Draw the WARNING indicator
            drawImage3(
                WARNING_X,
                ALERT_Y,
                ALERT_WARNING_WIDTH,
                ALERT_WARNING_HEIGHT,
                ALERT_WARNING_data);
            wasInDanger = TRUE;
        }
    } else if (wasInCritical || wasInDanger) {
        // If we're leaving a danger state, clear the indicator
        _clearWarning();
        wasInCritical = FALSE;
        wasInDanger = FALSE;
    }
}

// Used internally and at the game over screen to draw the score to the screen
void drawScore(int x, int y) {
    // convert the score to a string and print it
    char buffer[MAX_SCORE_DIGITS];
    sprintf(buffer, "%0"SCORE_DIGITS_FORMAT"d", score);
    drawString(x, y, buffer, WHITE);
}



