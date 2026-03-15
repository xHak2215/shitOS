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

char* int_to_str(int N) {
    int i = 0;

    // Save the copy of the number for sign
    int sign = N;
    
    // Allocate enough space for the string
    // Maximum digits in an integer + negative sign + null terminator
    char* str = 12 * sizeof(char); // 11 for int + 1 for null terminator
    
    // Handle the special case when N is 0
    if (N == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // Extract digits from the number and add them to the string
    while (N > 0) {
        // Convert integer digit to character and store it in the str
        str[i++] = N % 10 + '0';
        N /= 10;
    }

    // If the number was negative, add a minus sign to the string
    if (sign < 0) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }

    return str; // Return the pointer to the string
}
