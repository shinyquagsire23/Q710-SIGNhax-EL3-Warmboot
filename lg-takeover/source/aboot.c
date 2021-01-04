#include "aboot.h"

void aboot_init()
{
    //sbl_framebuffer = (uint8_t*)0x90001000;
    
    aboot_fastboot_ack = (void*)(0x8f63d300);
    aboot_fastboot_register = (void*)(0x8f63d1a4);
    aboot_boot_linux = (void*)(0x8f63979c);
    aboot_screen_puts = (void*)(0x8f608044);
    aboot_dev_tree_appended = (void*)(0x8f62651c);
    aboot_boot_linux_from_mmc = (void*)(0x8f63aa38);
    aboot_do_reboot = (void*)(0x8f633b08);
}
