#include <stdint.h>
#include <string.h>
#include <stdarg.h> 
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "multi_lib.c"
#include "keyboard_map.c"
#include "text_tool.c" 

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

#define TEXT_STYLE 0x07
#define MAX_INPUT_SIZE 256

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;
int cursor = 0; // счетчик позиции можно сказать курсора
char* input_text_buffer[MAX_INPUT_SIZE];
char* input_key_buffer;
unsigned int input_length = 0;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void) {
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void){
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}


void keyboard_handler_main(void){
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;
               
		// буфер котроый хранит последний введеный синвол
		input_key_buffer = keycode;
		// хранит все написаное, предположительное использование это ждать пока в input_key_buffer не окажеться ENTER_KEY_CODE а после исполнять команду и сбрасывать буфер
                
                char key_str[2] = { keycode, '\0' };

		mstrcat(input_text_buffer, key_str);
	}
}

// очистка экрана
void clear(int style) {
    cursor = 0;
    unsigned int i = 0;

    while (i < SCREENSIZE) {
	vidptr[i++] = ' ';
	vidptr[i++] = style;
    }
}

// переход на новую строку(сомнительно)
void print_newline(void){
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

//вывод текста на экран
void print(const char* str, int style) {
    while (*str) {
        // Проверка на символ новой строки
        if (*str == '\n') {
            // Переход на новую строку в видеопамяти
            cursor += (COLUMNS_IN_LINE*2 - strlen(str)*2) - 10; // Переход на новую строку ((80 символов - длина уже выведеного текста) * 2 байта) 
            str++;     // Переход к следующему символу
            continue; // Переходим на следующую итерацию
        }

        // Запись символа в видеопамять
        vidptr[cursor] = *str;       // Запись символа
        vidptr[cursor + 1] = style; // Запись атрибута цвета
        str++;                     // Переход к следующему символу
        cursor += 2;              // Увеличение счётчика на 2 (символ + атрибут)
    }
}

void kernel_main() {
    clear(TEXT_STYLE);

    char command[MAX_INPUT_SIZE];

    print("shitOS\n", TEXT_STYLE);
    print("@", getColorCode("red"));
    print("@", getColorCode("green"));
    print("@", getColorCode("blue"));
   
    cursor = 3840;
    print(">", TEXT_STYLE); 
    
    idt_init();
    kb_init();
    // Бесконечный цикл
    while(1) {
	print(keyboard_map[(const char) input_text_buffer], TEXT_STYLE);

        if (input_key_buffer == ENTER_KEY_CODE) {
	    cursor = 3840; // Обновление позиции курсора
            //wordwise_32_memset(input_text_buffer, 0, sizeof(input_text_buffer)); // Очистка буфера
	    for (int i = 0; i==MAX_INPUT_SIZE; i++){
	        input_text_buffer[i]="";
	    }
            print(">                                                          ", TEXT_STYLE); // Снова отображение приглашения
            return;
        }
       asm volatile ("hlt");
    }
}
