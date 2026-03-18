typedef struct {
    const char* name;  // Цвет текста
    unsigned char code; // Код цвета
} ColorCode;

// Массив с кодами цветов
ColorCode colorCodes[] = {
    { "black",        0  },
    { "blue",         1  },
    { "green",        2  },
    { "cyan",         3  },
    { "red",          4  },
    { "magenta",      5  },
    { "brown",        6  },
    { "light gray",   7  },
    { "dark gray",    8  },
    { "light blue",   9  },
    { "light green",  10 },
    { "light cyan",   11 },
    { "light red",    12 },
    { "light magenta",13 },
    { "light yellow", 14 },
    { "white",        15 }
};

/* VGA background color codes (attribute bytes) — white foreground (fg = 7) */

ColorCode vga_bg_codes[16] = {
    { "black"        , (0  << 4) },
    { "blue"         , (1  << 4) }, 
    { "green"        , (2  << 4) }, 
    { "cyan"         , (3  << 4) }, 
    { "red"          , (4  << 4) }, 
    { "magenta"      , (5  << 4) },
    { "brown"        , (6  << 4) }, 
    { "light gray"   , (7  << 4) }, 
    { "dark gray"    , (8  << 4) }, 
    { "light blue"   , (9  << 4) }, 
    { "light green"  , (10 << 4) },
    { "light cyan"   , (11 << 4) },
    { "light red"    , (12 << 4) },
    { "light magenta", (13 << 4) },
    { "light yellow" , (14 << 4) },
    { "white"        , (15 << 4) }
};


#define NUM_COLORS (sizeof(colorCodes) / sizeof(colorCodes[0]))
#define BG_NUM_COLORS (sizeof(vga_bg_codes) / sizeof(vga_bg_codes[0]))

// обозначения клавиш
/* The following array is taken from 
    http://www.osdever.net/bkerndev/Docs/keyboard.htm
   All credits where due
*/

unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

// Функция для получения кода цвета по его имени (независимо от регистра)
unsigned char getColorCode(const char* name) {
    for (int i = 0; i < NUM_COLORS; i++) {
        if (strncasecmp(colorCodes[i].name, name, strlen(name)) == 0) {
            return colorCodes[i].code;
        }
    }
    return 15; // Возвращает 0xFF, если не найдено
}

unsigned char getBGColorCode(const char* name) {
    for (int i = 0; i < BG_NUM_COLORS; i++) {
        if (strncasecmp(vga_bg_codes[i].name, name, strlen(name)) == 0) {
            return vga_bg_codes[i].code;
        }
    }
    return 0; // Возвращает 0xFF, если не найдено
}

unsigned char colors(const char* name_symbol_color, const char* name_begraund_color){
   return getBGColorCode(name_begraund_color) | getColorCode(name_symbol_color);

}

