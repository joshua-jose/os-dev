#include "keyboard.hpp"
#include "esh/esh.h"
#include <ctype.h>

extern esh_t* esh;

static uint8_t keydown_bitmap[24]; // 192 (8*24) key support
static bool caps_lock = false;

void keyboard_recieve_scancode(scancode_t scancode){
    static bool escape_code_sent = false; // Have we got 0xE0 before this?
    bool escaped = false; // Is this the key after an 0xE0?
    bool released = false;

    // Handle two byte keys
    if (escape_code_sent){
        escape_code_sent = false;
        escaped = true;
    }

    /* Signifies start of special two code scancode:
    if (scancode == ESCAPE_CODE)
        escape_code_sent = true;
    equivalent: */
    escape_code_sent = (scancode == ESCAPE_CODE);    
    if (escape_code_sent) return; // No more handling to do, just set the bool for the next scancode and return
    released = (scancode & RELEASED_BIT);

    if (scancode == KEY_CAPS_LOCK) // If caps lock key is pressed, toggle caps lock bool 
        caps_lock = !caps_lock; 
    
    // Set the correct bit in the bitmap
    keyboard_bitmap_set(scancode, escaped);

    if (released) return; // Don't handle on key release

    // Turn into key
    keyboard_scancode_to_key(scancode, escaped);
}

void keyboard_scancode_to_key(scancode_t scancode, bool escaped){
    //bool released = (scancode & RELEASED_BIT);

    if (escaped){
        char ansi_code = '\0';
        char key = '\0';
        if (scancode == KEY_RIGHT_CTRL)     keyboard_bitmap_set(KEY_LEFT_CTRL);
        if (scancode == KEY_HOME)           ansi_code = 'H';
        if (scancode == KEY_END)            ansi_code = 'F';
        if (scancode == KEY_CURSOR_UP)      ansi_code = 'A';
        if (scancode == KEY_CURSOR_DOWN)    ansi_code = 'B';
        if (scancode == KEY_CURSOR_LEFT)    ansi_code = 'D';
        if (scancode == KEY_CURSOR_RIGHT)   ansi_code = 'C';
        if (scancode == KEY_DELETE)         key = (char)127;

        if (ansi_code != '\0'){
            esh_rx(esh, '\e');
            esh_rx(esh, '[');
            esh_rx(esh, ansi_code);
        }
        else
            esh_rx(esh, key);
        
    }

    else if (scancode <= 58){ // Normal ASCII single byte key
         bool should_be_caps = caps_lock;
         bool should_be_shifted = false;
        if (keyboard_bitmap_get(KEY_LEFT_SHIFT) | keyboard_bitmap_get(KEY_RIGHT_SHIFT)){
            should_be_caps = !should_be_caps; // shift key inverts whether its caps
            should_be_shifted = true;
        }

        char key = scancode_to_char[scancode];
        if (should_be_shifted)
            key = shifted_scancode_to_char[scancode];
        else if (should_be_caps && isalpha(key))
            key = toupper(key);
        
        // If key is ASCII, send to terminal
        esh_rx(esh, key);
    }
}

void keyboard_bitmap_set(scancode_t scancode,bool escaped){
    bool released = (scancode & RELEASED_BIT); // This key is a released key if bit 0x80 is set
    
    int bitmap_table = scancode / BITMAP_TABLE_SIZE; 
    int bitmap_entry = scancode % BITMAP_TABLE_SIZE;

    // The last single byte scancode is 0x58, so shift it to above that
    // div by 8 because thats the size of our bitmap
    // shift into higher tables for extra keys
    if (escaped) bitmap_table += SINGLE_BYTE_END / BITMAP_TABLE_SIZE; 
    // The table value is from the scancode with the released bit
    // This will not flip the same bit that was set, so we need to convert it to a "make code" (without the extra bit)
    // These 8s are from uint8_t, the size of the bitmap table
    if (released) bitmap_table -= RELEASED_BIT / BITMAP_TABLE_SIZE;

    keydown_bitmap[bitmap_table] &= ~(1 << bitmap_entry); // (x & !1) will clear that bit
    keydown_bitmap[bitmap_table] |= (!released) << bitmap_entry; // or it with our desired value
    // Use the trick of boolean values converting to either 0 or 1, then or the entry with that.
}

bool keyboard_bitmap_get(scancode_t scancode, bool escaped){
    int bitmap_table = scancode / BITMAP_TABLE_SIZE; 
    int bitmap_entry = scancode % BITMAP_TABLE_SIZE;

    if (escaped) bitmap_table += SINGLE_BYTE_END / BITMAP_TABLE_SIZE; 

    return keydown_bitmap[bitmap_table] & (1<<bitmap_entry);
}