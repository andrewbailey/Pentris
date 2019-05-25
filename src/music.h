// Interrupts
#define REG_IE  *(volatile unsigned short*) 0x4000200
#define REG_IF  *(volatile unsigned short*) 0x4000202
#define REG_IME *(volatile unsigned short*) 0x4000208
#define IRQ_ENABLE    1
#define IRQ_VBLANK    (1 << 0)
#define IRQ_HBLANK    (1 << 1)
#define IRQ_VCOUNT    (1 << 2)
#define IRQ_TIMER(n)  (1 << (3 + (n))) /* 0, 1, 2 or 3 */
#define IRQ_COM       (1 << 7)
#define IRQ_DMA(n)    (1 << (8 + (n)))
#define IRQ_KEYPAD    (1 << 12)
#define IRQ_CARTRIDGE (1 << 13)
typedef void (*irqptr)(void);
#define REG_ISR_MAIN *(volatile irqptr*) 0x3007FFC

// Timers
#define REG_TMD(n)   *(volatile unsigned short*)(0x4000100 + ((n) << 2))
#define REG_TMCNT(n) *(volatile unsigned short*)(0x4000102 + ((n) << 2))
#define TM_FREQ_1    0
#define TM_FREQ_64   1
#define TM_FREQ_256  2
#define TM_FREQ_1024 3
#define TM_CASCADE   (1 << 2)
#define TM_IRQ       (1 << 6)
#define TM_ENABLE    (1 << 7)

// Legacy sound
#define REG_SNDDMGCNT *(volatile unsigned short*) 0x4000080
#define REG_SNDDSCNT  *(volatile unsigned short*) 0x4000082
#define REG_SNDSTAT   *(volatile unsigned short*) 0x4000084
#define REG_SND1SWP   *(volatile unsigned short*) 0x4000060
#define REG_SND1CNT   *(volatile unsigned short*) 0x4000062
#define REG_SND1FRQ   *(volatile unsigned short*) 0x4000064
#define REG_SND2CNT   *(volatile unsigned short*) 0x4000068
#define REG_SND2FRQ   *(volatile unsigned short*) 0x400006C
#define REG_SND4CNT   *(volatile unsigned short*) 0x4000078
#define REG_SND4FRQ   *(volatile unsigned short*) 0x400007C
#define MASTER_SND_EN (1 << 7)
#define DMG_VOL_25    0
#define DMG_VOL_50    1
#define DMG_VOL_100   2
#define LEFT_VOL(n)  (n)
#define RIGHT_VOL(n) ((n) << 4)
#define CHAN_EN_L(n) (1 << ((n) + 7))  /* n is 1-based! */
#define CHAN_EN_R(n) (1 << ((n) + 11))
#define DSA_VOL_50   (0 << 2)
#define DSA_VOL_100  (1 << 2)
#define DSB_VOL_50   (0 << 3)
#define DSB_VOL_100  (1 << 3)
#define DSA_EN_R     (1 << 8)
#define DSA_EN_L     (1 << 9)
#define DSA_TM_1     (1 << 10)
#define DSA_RST_FIFO (1 << 11)
#define DSB_EN_R     (1 << 12)
#define DSB_EN_L     (1 << 13)
#define DSB_TM_1     (1 << 14)
#define DSB_RST_FIFO (1 << 15)

// Streaming sound
#define REG_DMASRC(n)       *(volatile unsigned int*)(0x40000B0 + ((n) * 12))
#define REG_DMADST(n)       *(volatile unsigned int*)(0x40000B4 + ((n) * 12))
#define REG_DMACNT(n)       *(volatile unsigned int*)(0x40000B8 + ((n) * 12))
#define DMA_REPEAT          (1 << 25)
#define DMA_32              (1 << 26)
#define START_ON_FIFO_EMPTY (3 << 28)
#define DMA_ON              (1 << 31)
#define REG_DISPSTAT        *(volatile unsigned short*)0x4000004
#define INT_VBLANK_ENABLE   (1 << 3)

// Things that weren't in the powerpoint
#define REG_FIFO_A             0x40000A0

void initMusic();

