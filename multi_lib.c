size_t strlen(const char* s) {
 size_t len = 0;
 while (*s++) {
     len++;
 }
 return len;
}

int strncasecmp(const char* s1, const char* s2, size_t n) {
while (n--) {
    char c1 = *s1++;
    char c2 = *s2++;
    
    if (c1 >= 'A' && c1 <= 'Z') c1 += 32; // Приводим к нижнему регистру
    if (c2 >= 'A' && c2 <= 'Z') c2 += 32; // Приводим к нижнему регистру

    if (c1 != c2) {
	return c1 - c2;
    }
    if (c1 == '\0') {
	return 0; // Строки равны
    }
}
return 0; // Строки равны по первой n символам
}


void msprintf(char *buffer, const char *format, const char *str_value, int int_value) {
    while (*format) {
        if (*format == '%') {
            format++; // Пропускаем символ '%'
            if (*format == 's') {
                while (*str_value) {
                    *buffer++ = *str_value++;
                }
            } else if (*format == 'd') {
                // Форматируем целое число в строку
                char temp[20]; // Временный буфер
                int n = int_value;
                int i = 0;
                if (n < 0) {
                    *buffer++ = '-';
                    n = -n;
                }
                do {
                    temp[i++] = (n % 10) + '0';
                    n /= 10;
                } while (n > 0);
                // Записываем число в обратном порядке
                while (i > 0) {
                    *buffer++ = temp[--i];
                }
            }
        } else {
            *buffer++ = *format;
        }
        format++;
    }
    *buffer = '\0'; // Завершаем строку
}
