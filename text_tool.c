#include <stdio.h>

typedef struct {
    const char* name;  // Цвет текста
    unsigned char code; // Код цвета
} ColorCode;

// Массив с кодами цветов
ColorCode colorCodes[] = {
    { "black",        0x00 },
    { "blue",         0x01 },
    { "green",        0x02 },
    { "cyan",         0x03 },
    { "red",          0x04 },
    { "magenta",      0x05 },
    { "brown",        0x06 },
    { "light gray",   0x07 },
    { "dark gray",    0x08 },
    { "light blue",   0x09 },
    { "light green",  0x0A },
    { "light cyan",   0x0B },
    { "light red",    0x0C },
    { "light magenta",0x0D },
    { "light yellow", 0x0E },
    { "white",        0x0F }
};

#define NUM_COLORS (sizeof(colorCodes) / sizeof(colorCodes[0]))


// Функция для получения кода цвета по его имени (независимо от регистра)
unsigned char getColorCode(const char* name) {
    for (int i = 0; i < NUM_COLORS; i++) {
        if (strncasecmp(colorCodes[i].name, name, strlen(name)) == 0) {
            return colorCodes[i].code;
        }
    }
    return 0xFF; // Возвращает 0xFF, если не найдено
}
/*
int main() {
    const char* colorName = "blue";  // Имя цвета для поиска
    unsigned char code = getColorCode(colorName);

    if (code != 0xFF) {
        printf("Код цвета '%s': 0x%02X\n", colorName, code);
    } else {
        printf("Цвет '%s' не найден.\n", colorName);
    }

    return 0;
}
*/
