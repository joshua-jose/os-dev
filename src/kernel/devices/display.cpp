#include "display.hpp"
// Memory is volatile - Don't optimise it away even if it seems nothing is happening
// 16 bits per character, and the pointer is constant
volatile uint16_t* const vmem = (volatile uint16_t*)VGA_START;

static int text_column = 0;
static int text_line = 0;
static uint8_t colour_code = 0x02;

void display_putchar(char c, uint8_t colour_code){
    int offset = (text_line*VGA_LINE_LENGTH)+text_column; // Position on screen
    uint16_t code = (colour_code << 8) | c;
    vmem[offset] = code;
};

void display_putchar(char c, uint8_t colour_code, int offset){
    uint16_t code = (colour_code << 8) | c;
    vmem[offset] = code;
};

void display_clear(){
    for (int i=0; i<NUM_CHARS;i++)
        display_putchar(' ', 0x0f, i);
    text_column = 0;
    text_line = 0;
    terminal_update_cursor();
}

void terminal_init(){
    // very VGA text mode specific
    int cursor_start = 0; // start and end scanlines (between 0 and 15... for some reason)
    int cursor_end = 15;

    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

// VGA text mode specific 
void terminal_update_cursor(){
    uint16_t pos = text_line * VGA_LINE_LENGTH + text_column;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
};

void terminal_scroll(){
    for (int i=VGA_LINE_LENGTH; i<(VGA_LINE_LENGTH-1)*VGA_SCREEN_HEIGHT;i++)
        vmem[i] = vmem[i+VGA_LINE_LENGTH];
    terminal_clear_line(VGA_SCREEN_HEIGHT-1);
    
}

void terminal_inc_column(int n){
    text_column += n;
    if (text_column > VGA_LINE_LENGTH) 
        text_column = VGA_LINE_LENGTH;
    terminal_update_cursor();
};
void terminal_dec_column(int n){
    text_column -= n;
    if (text_column < 0) 
        text_column = 0;
    terminal_update_cursor();
};

void terminal_clear_line(int line){
    for (int i=0; i<VGA_LINE_LENGTH; i++)
        display_putchar(' ', 0x0f, (line*VGA_LINE_LENGTH)+i);
}

bool terminal_handle_ansi(char c){
    static bool escape_start = false;
    static bool ansi_csi = false;
    static int ansi_n = 0;

    if(ansi_csi){
        if (c >= '0' && c <= '9'){
            ansi_n = (int)c - 0x30; // Convert char to int
            return true;
        }
        switch (c){
            case 'D':
                terminal_dec_column(ansi_n);
            break; 

            case 'C':
                terminal_inc_column(ansi_n);
            break; 
            case 'K':
                if (ansi_n==2) 
                    terminal_clear_line(text_line);
            break;

            case 'J':
                if (ansi_n==3)
                    display_clear();
            break;
        }
        ansi_n = 0;
        ansi_csi = false;
        return true;
    }

    if (escape_start && c == '['){
        ansi_csi = true;
        escape_start = false;
        return true;
    }
    if (c == '\e'){
        escape_start = true;
        return true;
    }
    return false;
};

void terminal_putc(char c){
    if (terminal_handle_ansi(c))
        return;

    if (c == '\n'){
        text_column = 0;
        text_line++; // Goes to start of next line
    }
    else if (c == '\r')
        text_column = 0;
    
    else if (c == '\b'){ 
        // if not at start of column, go up
        if (text_column > 0)
            text_column--;
        else{
            // Go to end of line above
            text_column = VGA_LINE_LENGTH-1;
            if (text_line > 0)
                text_line--;
        }
    }
    else{
        display_putchar(c, colour_code);
        text_column++;

        if (text_column % VGA_LINE_LENGTH == 0){
            text_column = 0;
            text_line++;
        };
    }

    if (text_line >= VGA_SCREEN_HEIGHT){
        text_line = VGA_SCREEN_HEIGHT-1;
        text_column = 0;
        terminal_scroll();
    }
    
    terminal_update_cursor();
};
void terminal_fg_colour(){};
void terminal_bg_colour(){};

