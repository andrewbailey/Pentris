#include "music.h"
#include "korobeiniki.h"

#define SONGLEN KOROBEINIKILEN
#define SONGFREQ KOROBEINIKIFREQ
#define SONGDATA korobeiniki

int vblankcnt;

// This is all from the advanced lecture slides
void vblankHandler() {
    REG_IME &= ~IRQ_ENABLE;
    if(!vblankcnt--) {
        REG_TMD(0) = 0;
        REG_TMCNT(0) = 0;
        initMusic(); // loop
    }
    REG_IF = REG_IF;
    REG_IME |= IRQ_ENABLE;
}

void initMusic() {
    REG_IME &= ~IRQ_ENABLE;
    REG_SNDSTAT = MASTER_SND_EN;
    REG_DMACNT(1) = 0;
    REG_SNDDSCNT = DMG_VOL_100 | DSA_EN_L | DSA_EN_R | DSA_RST_FIFO;
    vblankcnt = (int)(59.77 * SONGLEN / SONGFREQ);
    REG_TMD(0) = -((1 << 24) / SONGFREQ);
    REG_TMCNT(0) = TM_ENABLE | TM_FREQ_1;
    REG_DMASRC(1) = (int) SONGDATA;
    REG_DMADST(1) = (int) REG_FIFO_A;
    REG_DMACNT(1) = DMA_ON | START_ON_FIFO_EMPTY | DMA_32 | DMA_REPEAT;
    REG_IE |= IRQ_VBLANK;
    REG_DISPSTAT |= INT_VBLANK_ENABLE;
    REG_ISR_MAIN = vblankHandler;
    REG_IME |= IRQ_ENABLE;
}
