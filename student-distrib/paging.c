/* paging.c (MP3.1): Functions used to set up page directory and page tables
 * vim:ts=4 noexpandtab
 */

/* Some links to check out: 
 * (https://wiki.osdev.org/Setting_Up_Paging)
 * (https://wiki.osdev.org/Paging)
 * (https://courses.engr.illinois.edu/ece391/sp2021/secure/references/descriptors.pdf)
 * (https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C)
 * (https://en.wikipedia.org/wiki/Control_register#Control_registers_in_x86_series)
 */


#include "paging.h"

/* NOTES: Kernel already loaded at FOUR_MB and should be a single 4MB page.
          VidMem already loaded at VIDEO (see paging.h) and should be a single 4KB page.
          For MP3.1, every page but the VidMem and Kernel pages should be "Not Present" 
          Kyle said something about flushing the TLB? */

/*
 * init_paging
 *    DESCRIPTION: Sets the necessary register values for the processor to do paging
 *    INPUTS/OUTPUTS: none
 *    SIDE EFFECTS: Prepares page directory and page table and enables paging
 */
void init_paging() {
    
    int i;

    for(i=0;i < ONE_KB;i++){
            //fill all of directory w/ blank pages b/c unused
                page_directory[i].pd_mb.present = 0; 
                page_directory[i].pd_mb.read_write = 1; //left as 1 b/c all pages are marked read/write for mp3
                page_directory[i].pd_mb.user_supervisor = 1;
                page_directory[i].pd_mb.page_write_through = 0;
                page_directory[i].pd_mb.page_cache_disabled = 1;
                page_directory[i].pd_mb.accessed = 0;
                page_directory[i].pd_mb.dirty = 0;
                page_directory[i].pd_mb.page_size = 1;   //left 1 to indicate directory is 4mb
                page_directory[i].pd_mb.global_bit = 0;
                page_directory[i].pd_mb.available = 0;
                page_directory[i].pd_mb.page_attr_index = 0;
                page_directory[i].pd_mb.reserved = 0;
                page_directory[i].pd_mb.base_addr = i;
            //}

    }      

    // Blank/populate page table
    for(i=0; i<ONE_KB; i++){
        page_tab_desc_t page;
        
        page.present = 0;
        page.read_write = 1;
        page.user_supervisor = 1;
        page.page_write_through = 0;
        page.page_cache_disabled = 0;
        page.accessed = 0;
        page.dirty = 0;
        page.page_attr_tab_index = 0;
        page.global_bit = 0;
        page.avail = 0;
        
        // Initialize one page table entry for vidmem
        if(i == (VIDMEM>>12)) {
            page.present = 1;   
            page.page_base_address = VIDMEM >> 12;
        }
        else
            page.page_base_address = i;

        page_table_one[i] = page;
    }

    // Initializes the memory belonging to the video memory (4 KB within the first 4 MB page)
    page_directory[0].pd_kb.present = 1;        //present b/c page is being initialized
    page_directory[0].pd_kb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[0].pd_kb.user_supervisor = 0;    //1 for user-level pages
    page_directory[0].pd_kb.page_write_through = 0; //we always want writeback, so 0
    page_directory[0].pd_kb.page_cache_disabled = 0; //0 for video memory pages
    page_directory[0].pd_kb.accessed = 0;   //not used at all in mp3
    page_directory[0].pd_kb.reserved = 0;   //all reserved bits should be set to 0
    page_directory[0].pd_kb.page_size = 0;  //0 if 4K page directory entry
    page_directory[0].pd_kb.global_bit = 0; //0 b/c not kernel page
    page_directory[0].pd_kb.available = 0;  //not used at all in mp3
    page_directory[0].pd_kb.page_table_addr = (unsigned)page_table_one >> 12;   //shift address of table so that lower 12 bits are removed

    // Initializes the memory belonging to the kernel (4 MB - 8 MB)
    page_directory[1].pd_mb.present = 1;    //present b/c page is being initialized
    page_directory[1].pd_mb.read_write = 1;     //all pages are marked read/write for mp3
    page_directory[1].pd_mb.user_supervisor = 0;    //0 for kernel pages
    page_directory[1].pd_mb.page_write_through = 0;    //we always want writeback, so 0
    page_directory[1].pd_mb.page_cache_disabled = 1;    //1 for program code and data pages (kernel pages)
    page_directory[1].pd_mb.accessed = 0;   //not used at all in mp3
    page_directory[1].pd_mb.dirty = 0;      //not used at all in mp3
    page_directory[1].pd_mb.page_size = 1;  //1 if 4M page directory entry
    page_directory[1].pd_mb.global_bit = 1; // the kernel is mapped into every process’s address space
    page_directory[1].pd_mb.available = 0;  //not used at all in mp3
    page_directory[1].pd_mb.page_attr_index = 0;  //not used at all in mp3
    page_directory[1].pd_mb.reserved = 0;       //reserved bits are always set to 0
    page_directory[1].pd_mb.base_addr = 1;      // Kernel is at first 4MB

    for(i=2;i<ONE_KB;i++){
            page_directory[i].pd_mb.present = 0; 
            page_directory[i].pd_mb.read_write = 1; //left as 1 b/c all pages are marked read/write for mp3
            page_directory[i].pd_mb.user_supervisor = 0;
            page_directory[i].pd_mb.page_write_through = 0;
            page_directory[i].pd_mb.page_cache_disabled = 0;
            page_directory[i].pd_mb.accessed = 0;
            page_directory[i].pd_mb.dirty = 0;
            page_directory[i].pd_mb.page_size = 1;   //left 1 to indicate directory is 4mb
            page_directory[i].pd_mb.global_bit = 0;
            page_directory[i].pd_mb.available = 0;
            page_directory[i].pd_mb.page_attr_index = 0;
            page_directory[i].pd_mb.reserved = 0;
            page_directory[i].pd_mb.base_addr = i;

    }       

    flush_tlb();

    // Load and enable page directory address in register CR3
    asm volatile ("movl  %0, %%eax;"                //loads page directory
                  "movl %%eax, %%cr3;"
                  "movl %%cr4, %%eax;"              
                  "orl $0x00000010, %%eax;"         
                  "movl %%eax, %%cr4;"
                  "movl %%cr0, %%eax;"              //enables page directory
                  "orl $0x80000000, %%eax;"
                  "movl %%eax, %%cr0;"
                :                                   // no outputs
                : "r" (page_directory)              // no inputs
                : "eax", "cc"        // clobbers eax and condition codes
    );


    /* Enable paging by setting the 31st bit in the CR0 register to 1
    asm volatile (              
                :               // no outputs
                :               // no inputs
                : "eax", "cc"        // clobbers eax and condition codes
    ); */

    /* Enable Page Size Extension (4 MB pages) by setting corresponding bit to 1
    asm volatile (
                  :             // no outputs
                  :             // no inputs
                  : "eax", "cc"      // clobbers eax and condition codes
    ); */
    

}


/*  
 * flush_tlb
 *    DESCRIPTION: Used to flush TLB after making a change to the paging structure
 *    INPUTS/OUTPUTS: NONE
 *    SIDE EFFECTS: Flushes TLB for paging
 *    NOTES: See (https://wiki.osdev.org/TLB)
 */ 
void flush_tlb() {
    asm volatile ("movl	%%cr3, %%eax;"
	              "movl	%%eax, %%cr3;"
                :               // no inputs
                :               // no outputs
                : "eax"        // clobbers eax
    );
}