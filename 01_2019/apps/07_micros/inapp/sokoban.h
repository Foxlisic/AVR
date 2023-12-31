#define SOKOBAN_MAX_LEVEL 4

enum SokobanSprite {

    SOKOBAN_BRICK   = 1,
    SOKOBAN_BOX     = 2,
    SOKOBAN_PLACE   = 3,
    SOKOBAN_WOOD    = 4,
    SOKOBAN_PLAYER  = 5,
};

// Уровень сокобана
static const byte sokoban_level[SOKOBAN_MAX_LEVEL][64] =
{
    {   // Уровень 1
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x55, 0x40, 0x00, 0x00,
        0x00, 0x40, 0x40, 0x00, 0x00,
        0x00, 0x60, 0x40, 0x00, 0x00,
        0x05, 0x42, 0x50, 0x00, 0x00,
        0x04, 0x22, 0x10, 0x00, 0x00,
        0x54, 0x45, 0x10, 0x15, 0x54,
        0x40, 0x45, 0x15, 0x50, 0xf4,
        0x48, 0x20, 0x00, 0x00, 0xf4,
        0x55, 0x45, 0x44, 0x50, 0xf4,
        0x00, 0x40, 0x05, 0x55, 0x54,
        0x00, 0x55, 0x54, 0x00, 0x00,
        // Положение игрока
        11, 9, 0, 0
    },
    {   // Уровень 2
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x15, 0x55, 0x55, 0x40, 0x00,
        0x1f, 0x04, 0x00, 0x54, 0x00,
        0x1f, 0x04, 0x82, 0x04, 0x00,
        0x1f, 0x06, 0x55, 0x04, 0x00,
        0x1f, 0x00, 0x05, 0x04, 0x00,
        0x1f, 0x04, 0x42, 0x14, 0x00,
        0x15, 0x54, 0x58, 0x84, 0x00,
        0x01, 0x20, 0x88, 0x84, 0x00,
        0x01, 0x00, 0x40, 0x04, 0x00,
        0x01, 0x55, 0x55, 0x54, 0x00,
        // Положение игрока
        8, 6, 0, 0
    },
    {   // Уровень 3
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x15, 0x55, 0x40,
        0x00, 0x00, 0x10, 0x00, 0x40,
        0x00, 0x00, 0x12, 0x61, 0x40,
        0x00, 0x00, 0x12, 0x09, 0x00,
        0x00, 0x00, 0x16, 0x21, 0x00,
        0x15, 0x55, 0x52, 0x11, 0x50,
        0x1f, 0xf0, 0x52, 0x08, 0x10,
        0x17, 0xf0, 0x08, 0x20, 0x10,
        0x1f, 0xf0, 0x55, 0x55, 0x50,
        0x15, 0x55, 0x40, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        // Положение игрока
        15, 2, 0, 0
    },
    {   // Уровень 4
        0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x55, 0x50, 0x54, 0x00, 
        0x00, 0x7c, 0x11, 0x45, 0x00, 
        0x00, 0x7c, 0x15, 0x01, 0x00, 
        0x00, 0x7c, 0x00, 0xa1, 0x00, 
        0x00, 0x7c, 0x11, 0x21, 0x00, 
        0x00, 0x7d, 0x51, 0x21, 0x00, 
        0x00, 0x55, 0x21, 0x81, 0x00, 
        0x00, 0x01, 0x09, 0x21, 0x00, 
        0x00, 0x01, 0x20, 0x81, 0x00, 
        0x00, 0x01, 0x05, 0x01, 0x00, 
        0x00, 0x01, 0x55, 0x55, 0x00,
        // Положение игрока
        13, 2, 0, 0
    }    
     
};

// Спрайт Марио 16x16 (4x16)
static const byte mario[64] = {

    0x00, 0x2a, 0xa8, 0x00, 0x00, 0xaa, 0xaa, 0xa0,
    0x00, 0x55, 0xf7, 0x00, 0x01, 0x77, 0xf7, 0xf0,
    0x01, 0x75, 0xfd, 0xfc, 0x01, 0x5f, 0xf5, 0x50,
    0x00, 0x3f, 0xff, 0xc0, 0x00, 0x56, 0x54, 0x00,
    0x01, 0x56, 0x59, 0x50, 0x05, 0x56, 0xa9, 0x54,
    0x0f, 0xdb, 0xae, 0x7c, 0x0f, 0xfa, 0xaa, 0xfc,
    0x0f, 0xea, 0xaa, 0xbc, 0x00, 0xaa, 0x2a, 0x80,
    0x01, 0x54, 0x05, 0x50, 0x05, 0x54, 0x05, 0x54,
};
