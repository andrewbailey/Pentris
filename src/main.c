#include <stdlib.h>

#include "gbalib.h"
#include "main.h"
#include "gameboard.h"
#include "dashboard.h"
#include "place.h"
#include "background/SPLASH.h"
#include "background/BACKGROUND.h"
#include "background/ENDGAME.h"
#include "music.h"

#define EASY_GRAVITY 90
#define HARD_GRAVITY 45
#define GRAVITY_STEP 2
#define MIN_GRAVITY  5

enum GAMESTATE {
    STATE_MENU_INIT,
    STATE_MENU,
    STATE_PLAY_INIT,
    STATE_PLAY,
    STATE_END_INIT,
    STATE_END
};

// the current state of the game
static enum GAMESTATE globalState;

// used to set the game's seed
static int vblankCount;

// used to control pentomino gravity
static int advancementTimer; // how many vblank's ago the last drop was
static int gravityDelay; // how many vblanks until a tile is forced to drop

// used to keep track of which buttons are being held (and therefore ignored)
static unsigned int previousButtonState;

int main() {
    // Enter Display mode 3
    REG_DISPCNT = MODE_3 | BG2_EN;
    
    // Set initial game conditions
    advancementTimer = 0;
    gravityDelay = 90;
    previousButtonState = 0xFFFF;
    globalState = STATE_MENU_INIT;
    
    // Start playing music
    initMusic();
    
    // Begin game loop
    while (TRUE) {
        onUpdate();
        
        // Keep track of which buttons are being held
        previousButtonState = BUTTONS;
        
        vblankCount++;
        waitForVblank();
        
        onDraw();
    }
}

void startGame(int startingGravity) {
    // Set the game's seed
    srand(vblankCount);
    globalState = STATE_PLAY_INIT;
    
    // set the gravity
    gravityDelay = startingGravity;
}

void onUpdate() {
    // Restart the game if select is pressed
    if (keyPressStart(BUTTON_SELECT)
            || (globalState == STATE_END && keyPressStart(BUTTON_A))) {
        globalState = STATE_MENU_INIT;
    }
    
    if (globalState == STATE_MENU_INIT) {
        resetBoard();
        resetPlacer();
    } else if (globalState == STATE_MENU && keyPressStart(BUTTON_ANY)) {
        if (keyPressStart(BUTTON_A)) {
            startGame(EASY_GRAVITY);
        } else if (keyPressStart(BUTTON_B)) {
            startGame(HARD_GRAVITY);
        }
    } else if (globalState == STATE_PLAY_INIT) {
        
    } else if (globalState == STATE_PLAY) {
        if (isGameOver()) {
            globalState = STATE_END_INIT;
        } else {
            onGameTick();
        }
    }
}

// Returns true if a specified button was pressed since the last game tick
// (and only if it was pressed since the last game tick)
bool keyPressStart(int button) {
    return KEY_DOWN_NOW(button) != (~(previousButtonState) & button)
            && KEY_DOWN_NOW(button);
}

// When the game is active, each the period before each vblank draw cycle
// triggers a game tick, in which inputs and events are processed
void onGameTick() {
    if (keyPressStart(BUTTON_LEFT) || keyPressStart(BUTTON_RIGHT)) {
        handleTileMove();
    } else if (keyPressStart(BUTTON_A)) {
        handleTileDrop();
    } else if (keyPressStart(BUTTON_L) || keyPressStart(BUTTON_R)) {
        handleTileRotate();
    } else if (keyPressStart(BUTTON_DOWN)) {
        advancementTimer = 0;
        handleTileMove();
    } else if (!isClearingRow()) {
        advancementTimer++;
        if (advancementTimer >= gravityDelay) {
            advancementTimer = 0;
            applyGravity();
        }
    }
    checkRowClear();
}

// Called when a single line is cleared to make the game even harder >;)
void speedUp() {
    if (gravityDelay > MIN_GRAVITY) {
        gravityDelay -= GRAVITY_STEP;
    }
}

// Dispatch draw calls. This function is called during VBlank
void onDraw() {
    if (globalState == STATE_MENU_INIT) {
        // Draw splash screen and move to idle menu state
        drawImage3(0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, SPLASH_data);
        globalState = STATE_MENU;
    } else if (globalState == STATE_PLAY_INIT) {
        // Draw game background, initialize the game, and move to active state
        drawImage3(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, BACKGROUND_data);
        setScore(0);
        generateNext();
        popPiece();
        globalState = STATE_PLAY;
    } else if (globalState == STATE_END_INIT) {
        // Draw the game over screen and move to the idle end state
        drawImage3(0, 0, ENDGAME_WIDTH, ENDGAME_HEIGHT, ENDGAME_data);
        // Draw the final score
        drawScore(122, 68);
        globalState = STATE_END;
    } else if (globalState == STATE_PLAY) {
        // Delegate game draw calls
        drawRowClear();
        drawDash();
        drawPlacingTile();
    }
    // Menus have static images, so if we're on a menu in its idle state, we
    // don't have to do anything here.
}
