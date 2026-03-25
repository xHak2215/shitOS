#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE (BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES)

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C
#define BACKSPACE_KEY_CODE 0x0E

#define TEXT_STYLE 0x07
#define begraund_color "blue"
#define MAX_INPUT_SIZE 80

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* video memory begins at address 0xb8000 */
volatile char *vidptr = (volatile char*)0xb8000;
unsigned int cursor = 0; // позиция в байтах (символ + атрибут)

char input_text_buffer[MAX_INPUT_SIZE];
unsigned int input_length = 0;

#include "lib/multi_lib.c"
#include "lib/text_tool.c" 

struct IDT_entry {
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
} __attribute__((packed));

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void) {
    unsigned long keyboard_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    keyboard_address = (unsigned long)keyboard_handler;
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
    IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = INTERRUPT_GATE;
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

    /* Init PICs */
    write_port(0x20 , 0x11);
    write_port(0xA0 , 0x11);
    write_port(0x21 , 0x20);
    write_port(0xA1 , 0x28);
    write_port(0x21 , 0x00);
    write_port(0xA1 , 0x00);
    write_port(0x21 , 0x01);
    write_port(0xA1 , 0x01);

    /* mask interrupts except keyboard */
    write_port(0x21 , 0xFD);
    write_port(0xA1 , 0xFF);

    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;
    load_idt(idt_ptr);
}

void kb_init(void){
    /* enable IRQ1 (keyboard) only on master PIC */
    write_port(0x21 , 0xFD);
}

void command_handler(char *command) {	
     char* text_color = "white";

     void consol_print(char* std) {
          pos_print(std, colors(text_color, begraund_color), COLUMNS_IN_LINE*4);
     }

     if (starts_with(command, "help")) {
	   clear_console(" ", colors("white", begraund_color)); // очистка рабочей облости
	   consol_print("the shitOS vary vary potusgni OS\ncommand:\nclear - clear display");

     } else if (starts_with(command, "clear")){
        clear_screen(colors("black", begraund_color));

     } else if (starts_with(command, "dick")){
	    clear_console(" ", colors("white", begraund_color)); // очистка рабочей облости
            for (int i=0; i < 8; i++) {
                if (i > 5){
	           pos_print("   ", colors("white", "black"), (COLUMNS_IN_LINE * 4 + 10) + (80 * 2 * i));
	           pos_print("   ", colors("white", "black"), (COLUMNS_IN_LINE * 4 + 10) + (80 * 2 * i)+8);
	        } else{		
	           pos_print(" ", colors("white", "black"), (COLUMNS_IN_LINE * 4 + 16) + (80 * 2 * i));
	        }
	    }

     } else if (starts_with(command, "fetch")){ // не работает
	    clear_console(" ", colors("white", begraund_color));
            
	    int ram_size = 0;
	    char out[8] = "RAM size:";
	    append(out, ram_size / 0x100000);
	    consol_print(out);
     } else if (starts_with(command, "calc")){
            command
     }	     
}

/* assembly keyboard handler should call this C function */
void keyboard_handler_main(void) {
    unsigned char status;
    unsigned char keycode;

    /* End of Interrupt (EOI) */
    write_port(0x20, 0x20);

    status = read_port(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);
        if (keycode & 0x80) {
            /* key release — ignore */
            return;
        }

        /* convert to ASCII via keyboard_map (ensure keycode < 128) */
        char c = 0;
        if (keycode < 128) 
		c = keyboard_map[keycode];

        if (keycode == ENTER_KEY_CODE) {
            /* print newline and prompt, process command here if needed */
            // move cursor to next line start
          
            // optionally process input_text_buffer[0..input_length-1]
            // clear input buffer

	    command_handler(input_text_buffer); // обработка и исполнение команд

            input_length = 0;
            for (int i = 0; i == MAX_INPUT_SIZE; i++){
	        input_text_buffer[i] = "";
	    }
            
            if (cursor >= SCREENSIZE)
		    cursor = SCREENSIZE - (COLUMNS_IN_LINE*2);
            
            cursor = SCREENSIZE - (COLUMNS_IN_LINE*2);
            print(">                                                                               ", TEXT_STYLE);
            cursor = SCREENSIZE - (COLUMNS_IN_LINE*2)+2;
            return;

        } else if (keycode == BACKSPACE_KEY_CODE) {
            if (input_length > 0) {
                /* remove char from buffer and screen */
                input_length--;
                input_text_buffer[input_length] = '\0';
                if (cursor >= 2) {
                    cursor -= 2;
                    vidptr[cursor] = ' ';
                    vidptr[cursor+1] = TEXT_STYLE;
                }
            }
            return;

        } else if (c != 0) {
            /* printable char: add to buffer and show */
            if (input_length < MAX_INPUT_SIZE - 1) {
                input_text_buffer[input_length++] = c;
                input_text_buffer[input_length] = '\0';
                vidptr[cursor] = c;
                vidptr[cursor+1] = TEXT_STYLE;
                cursor += 2;
                if (cursor >= SCREENSIZE) {
                    /* naive wrap: move to start of last line */
                    cursor = SCREENSIZE - (COLUMNS_IN_LINE*2);
                }
            }
        }
    }
}

int set_cursor(int x, int y) { // плохо работает
    unsigned short pos = (unsigned short)(y * 80 + x);
    if (x > COLUMNS_IN_LINE && y > LINES);
	    return 1; // говорим об ошибке

    // Высший и младший байт позиции передаются в регистры VGA через порты 0x3D4/0x3D5
    write_port(0x3D4, 0x0F);
    write_port(0x3D5, (unsigned char)(pos & 0xFF));        /* low byte */
    write_port(0x3D4, 0x0E);
    write_port(0x3D5, (unsigned char)((pos >> 8) & 0xFF)); /* high byte */
}

void kernel_main() {

    clear_screen(colors("black", begraund_color));
    print("shitOS\n", colors("black", begraund_color));
    print("@", colors("red", begraund_color));
    print("@", colors("green", begraund_color));
    print("@", colors("light yellow", begraund_color));
    // set prompt near bottom (e.g., last line)
    cursor = (LINES - 1) * COLUMNS_IN_LINE * 2;
    //set_cursor(strlen(input_text_buffer), 25);
    print(">                                                                               ", TEXT_STYLE);
    cursor += 2;

    idt_init();
    kb_init();

    // main idle loop
    while (1) {
        asm volatile ("hlt");
    }
}

