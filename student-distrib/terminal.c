/* terminal.c - terminal driver functions
 *  vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"

/*
 * terminal_open
 *    DESCRIPTION: Initalizes all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Initializes all terminal-related vars
 */
int terminal_open() {
    int i = 0;
    for(i = 0; i < KEYBOARD_BUF_SIZE; i++) {
        terminal_buf[i] = 0;
    }
    return 0;
}

/*
 * terminal_close
 *    DESCRIPTION: Clears all terminal-related variables
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURN VALUE: Always 0
 *    SIDE EFFECTS: Clears all terminal-related vars
 */
int terminal_close(int32_t fd) {
    return 0;
}

/*
 * terminal_read
 *    DESCRIPTION: Reads from the buffer 
 *    INPUTS: file descriptor, buf -- ptr to output buffer that we copy keyboard_buf to, n_bytes
 *    OUTPUTS: copies buf to terminal_buf
 *    RETURN VALUE: Number of bytes written
 *    SIDE EFFECTS: Writes to buffer pointed to by input
 */
int terminal_read(int32_t fd, char * buf, int32_t n_bytes) {
    
    // NULL check input and return 0 if NULL to signify no bytes read
    if(buf == 0 || n_bytes == 0)
        return 0; 

    // Let keyboard know how many bytes the buffer is
    terminal_buf_n_bytes = n_bytes;

    // Block until enter ('\n') has been pressed
    while(!enter_flag);

    // Copy keyboard_buf into input buf (only copy until '\n')
    (void)strncpy((int8_t *)terminal_buf, (int8_t *)keyboard_buf, n_bytes);
    (void)strncpy((int8_t *)buf, (int8_t *)terminal_buf, n_bytes);

    // Reset so the keyboard buffer can write to its normal size
    terminal_buf_n_bytes = KEYBOARD_BUF_SIZE;

    terminal_buf_i = keyboard_buf_i;

    enter_flag = 0;

    clear_keyboard_buf();

    // Return keyboard_buf index, which is the same as the number of bytes read from keyboard_buf including '\n'
    return terminal_buf_i;
}

/*
 * terminal_write
 *    DESCRIPTION: Writes the bytes from input buf to the screen
 *    INPUTS: buf -- bytes to write to screen
 *    OUTPUTS: none
 *    RETURN VALUE: number of bytes/chars written to screen
 *    SIDE EFFECTS: writes to video memory using putc
 */
int terminal_write(int32_t fd, char * buf, int32_t n_bytes) {

    int i;      // Loop index

    // NULL check input
    if(buf == 0 || n_bytes < 0)
        return -1;

    // Print n_bytes worth of chars
    for(i = 0; i < n_bytes; i++) {
        putc(buf[i]);
    }
    return i;
}
