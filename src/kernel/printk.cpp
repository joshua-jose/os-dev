#include <cstdint>
#include "kernel/kapi.hpp"

 #define LINE_LENGTH 80
 #define SCREEN_HEIGHT 25

void enable_cursor(uint8_t cursor_start=0, uint8_t cursor_end=15) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int column, int line) {
    uint16_t pos = line * LINE_LENGTH + column;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void clear_line(int line){
    for (int i=0; i<LINE_LENGTH;i++)
        vmem[(line*LINE_LENGTH)+i] = 0x0f20;
}

void clear_screen(){
    for (int i=0; i<(LINE_LENGTH*SCREEN_HEIGHT);i++)
        vmem[i] = 0x0f20;
}

void scroll(){
    for (int i=LINE_LENGTH; i<(LINE_LENGTH-1)*SCREEN_HEIGHT;i++)
        vmem[i] = vmem[i+LINE_LENGTH];
    clear_line(SCREEN_HEIGHT-1);
    
}

// 0x0f : white text, black background
void kputc(char c, uint8_t colour_code=0x02){
    static int column = 0;
    static int line = 0;

    static bool escape_start = false;
    static bool ansi_csi = false;
    static int ansi_n = 0;

    // ANSI handling
    // ---------------------------------------------------------
    if(ansi_csi){
        if (c >= '0' && c <= '9'){
            ansi_n = (int)c - 0x30; // Convert char to int
            return;
        }
        switch (c){
            case 'D':
                column -= ansi_n;
                if (column < 0) column = 0;
                update_cursor(column, line);
                ansi_n = 0;
            break; 

            case 'C':
                column += ansi_n;
                if (column > LINE_LENGTH) column = LINE_LENGTH;
                update_cursor(column, line);
                ansi_n = 0;
            break; 
            case 'K':
                if (ansi_n==2) clear_line(line);
                ansi_n = 0;
            break;

            case 'J':
                if (ansi_n==3) {
                    clear_screen();
                    column = 0;
                    line = 0;
                    update_cursor(column, line);
                }
                ansi_n = 0;
            break;
        }
        ansi_csi = false;
        return;
    }

    if (escape_start && c == '['){
        ansi_csi = true;
        escape_start = false;
        return;
    }
    if (c == '\e'){
        escape_start = true;
        return;
    }
    // ---------------------------------------------------------

    if (c == '\n'){
        column = 0;
        line++; // Goes to start of next line
    }
    else if (c == '\r')
        column = 0;
    
    else if (c == '\b'){ 
        // if not at start of column, go up
        if (column > 0)
            column--;
        else{
            // Go to end of line above
            column = LINE_LENGTH-1;
            if (line > 0)
                line--;
        }
    }
    else{
        int offset = (line*LINE_LENGTH)+column; // Position on screen
        uint16_t code = (colour_code << 8) | c;
        vmem[offset] = code;
        column++;

        if (column % LINE_LENGTH == 0){
            column = 0;
            line++;
        };
    }

    if (line >= SCREEN_HEIGHT){
        line = SCREEN_HEIGHT-1;
        column = 0;
        scroll();
    }
    
    update_cursor(column, line);
}

void printk(const char* in_string,int start_column, uint8_t colour_code){

    (void) start_column;
    // Loop until character is null
    for (int i=0; in_string[i]!='\0'; i++){
        // Send character to display
        kputc(in_string[i], colour_code);
    }
}


void fputc(char c){
    kputc(c);
}
