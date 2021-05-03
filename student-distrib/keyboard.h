/* keyboard.h - declarations and macros for keyboard
 *  vim:ts=4 noexpandtab
 */

// http://www.philipstorr.id.au/pcbook/book3/scancode.htm

#define KEYBOARD_PORT           0x60
#define KEYBOARD_IRQ            0x01
#define LEFT_SHIFT_PRESSED      0x2A
#define RIGHT_SHIFT_PRESSED     0x36
#define LEFT_SHIFT_RELEASED     0xAA
#define RIGHT_SHIFT_RELEASED    0xB6
#define CAPS_LOCK_PRESSED       0x3A
#define LEFT_CTRL_PRESSED       0x1D
#define LEFT_CTRL_RELEASED      0x9D
#define LEFT_ALT_PRESSED        0x38
#define LEFT_ALT_RELEASED       0xB8
#define UP_ARROW                0x48    // Added in EC
#define DOWN_ARROW              0x50    // Added in EC
#define RIGHT_ARROW             0x4D    // Added in EC
#define LEFT_ARROW              0x4B    // Added in EC
#define KEYBOARD_BUF_SIZE       128
#define KEYBOARD_BUF_CHAR_MAX   127

// Line buffer for keyboard entries
char keyboard_buf[KEYBOARD_BUF_SIZE];

// Holds the arg "n_bytes" passed into terminal_read
int terminal_buf_n_bytes;

// Keyboard buffer index to keep track of buffer pos
int keyboard_buf_bytes_written;

// Var to keep track of cursor position on the keyboard buffer
int keyboard_cursor_pos;

// Tracks if enter was pressed so terminal_read can continue
volatile int enter_flag;

// Keyboard flags
int left_shift_flag;

int right_shift_flag;

int ctrl_flag;

int caps_flag;

int alt_flag;

// Initialize the keyboard by enabling the PIC IRQ
void init_keyboard();

// Clear keyboard buffer
void clear_keyboard_buf();

// Handles Keyboard interrupts
extern void keyboard_handler();
