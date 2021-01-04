#ifndef ABOOT_H
#define ABOOT_H

#include <stdint.h>

void aboot_init();

//uint8_t* sbl_framebuffer;

void (*aboot_fastboot_ack)(char* resp, char* fmt, ...);
void (*aboot_fastboot_register)(char* cmd, void* func);
void (*aboot_boot_linux)(void *kernel, unsigned *tags, 
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size);
void (*aboot_screen_puts)(char* str, uint32_t color);
void* (*aboot_dev_tree_appended)(void* a, uint32_t a2, uint32_t b, void* c);
void* (*aboot_boot_linux_from_mmc)(void);
void* (*aboot_do_reboot)(uint32_t reason);
#endif // ABOOT_H
