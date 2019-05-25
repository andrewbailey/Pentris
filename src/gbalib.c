#include "gbalib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int x, int y, unsigned short color) {
    videoBuffer[OFFSET(x, y, 240)] = color;
}

void drawRect(int x, int y, int height, int width, volatile unsigned short color) {
    for (int i = 0; i < height; i++) {
        memFill((void*) &color, &videoBuffer[x + (y + i) * 240], width);
    }
}

void waitForVblank() {
    while (SCANLINECOUNTER > 160);
    while (SCANLINECOUNTER < 160);
}

void drawScreen(const unsigned short* image) {
    memCopy((void*) image, videoBuffer, 240 * 160);
}

void drawImage3(int x, int y, int width, int height, const unsigned short* image) {
    if (x == 0 && y == 0 && width == 240 && height == 160) {
        drawScreen(image);
    } else {
        for (int i = 0; i < height; i++) {
			int memPos = x + (y + i) * 240;
			if (memPos > 0 && memPos < 240 * 160) {
                memCopy(
                    (void*) &image[i * width],
                    &videoBuffer[memPos],
                    width);
			}
        }
    }
}

void memCopy(void* src, void* dest, int size) {
    DMA[DMA_CHANNEL_3].src = src;
    DMA[DMA_CHANNEL_3].dst = dest;
    DMA[DMA_CHANNEL_3].cnt = size 
        | DMA_SOURCE_INCREMENT
        | DMA_DESTINATION_INCREMENT
        | DMA_ON;
}

void memFill(void* fill, void* dest, int size) {
    DMA[DMA_CHANNEL_3].src = fill;
    DMA[DMA_CHANNEL_3].dst = dest;
    DMA[DMA_CHANNEL_3].cnt = size 
        | DMA_SOURCE_FIXED
        | DMA_DESTINATION_INCREMENT
        | DMA_ON;
}

void memCopy32(void* fill, void* dest, int size) {
    DMA[DMA_CHANNEL_3].src = fill;
    DMA[DMA_CHANNEL_3].dst = dest;
    DMA[DMA_CHANNEL_3].cnt = size 
        | DMA_32
        | DMA_SOURCE_INCREMENT
        | DMA_DESTINATION_INCREMENT
        | DMA_ON;
}

void memFill32(void* fill, void* dest, int size) {
    DMA[DMA_CHANNEL_3].src = fill;
    DMA[DMA_CHANNEL_3].dst = dest;
    DMA[DMA_CHANNEL_3].cnt = size 
        | DMA_32
        | DMA_SOURCE_FIXED
        | DMA_DESTINATION_INCREMENT
        | DMA_ON;
}

void drawChar(int x, int y, char ch, unsigned short color) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 6; j++) {
            if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
                setPixel(x + j, y + i, color);
            }
        }
    }
}

void drawString(int x, int y, char *str, unsigned short color) {
    while (*str) {
        drawChar(x, y, *str++, color);
        x += 6;
    }
}
