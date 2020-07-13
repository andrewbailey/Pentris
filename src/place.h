#ifndef PLACE_H
#define PLACE_H

bool isGameStarted();
void resetPlacer();
void popPiece();
void handleTileMove();
void handleTileDrop();
void handleTileRotate();
void applyGravity();
void drawPlacingTile();

#endif //PLACE_H
