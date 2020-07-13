#ifndef DASHBOARD_H
#define DASHBOARD_H

void incrementScore(int dScore);
void setScore(int score);
void generateNext();
void indicateLineClear();
enum TILE getNext();
void drawDash();
void drawScore();

#endif //DASHBOARD_H
