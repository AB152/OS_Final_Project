
#include "rtc.h"


/*
 * init_RTC
 *    DESCRIPTION: Initialize RTC and turns on IRQ8
 *    INPUTS: none
 *    OUTPUTS: none
 *    RETURNS: none
 *    SIDE EFFECTS: Turns on periodic interupts
 *    NOTES: See OSDev links in .h file to understand macros
 */ 


void init_RTC(){

disable_ints();                     // important that no interrupts happen (perform a CLI)
outb(RTC_PORT, DISABLE_NMI_B);		// select register B, and disable NMI
char prev = inb(CMOS_PORT);	        // read the current value of register B
outb(RTC_PORT, DISABLE_NMI_B);		// set the index again (a read will reset the index to register D)
outb(CMOS_PORT, prev | 0x40);	    // write the previous value ORed with 0x40. This turns on bit 6 of register B
enable_ints();                      // (perform an STI) and reenable NMI if you wish

}

