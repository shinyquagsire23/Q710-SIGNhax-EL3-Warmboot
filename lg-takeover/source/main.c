#include "types.h"
#include "utils.h"
#include "sbl.h"
#include "aboot.h"
#include "text.h"
#include "gpio.h"
#include "tinyprintf.h"

#include <string.h>
#include "elf.h"

void bootlog_replace(char* str)
{
    printf("%s\n", str);
    sbl_sleep_us(100000);
}

uint32_t __attribute__ ((naked)) __attribute__ ((target ("arm"))) emmc_redirect_shim(uint32_t offset_lo, uint32_t offset_hi, void* dst, uint32_t len)
{
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("ldr r9, =0x8f61c23c");
    __asm__("bx r9");
}

void* kernel_data = NULL;
uint32_t emmc_redirect(uint32_t offset_lo, uint32_t offset_hi, void* dst, uint32_t len)
{
    char tmp[0x100];
	
	bool redir = false;
	
	uint32_t offs = 0;

    if (offset_lo >= 0x15000000 && offset_lo < 0x17000000)
    {
        redir = true;
        offs = 0x15000000;
    }
    
    if (offset_lo >= 0x19000000 && offset_lo < 0x1b000000)
    {
        redir = true;
        offs = 0x19000000;
    }
    
    if (redir && kernel_data)
    {
        memcpy(dst, kernel_data + (offset_lo - offs), len);
        
        snprintf(tmp, 0x100, "redir %08lx%08lx size %08lx to %p, from %p\n", offset_hi, offset_lo, len, dst, kernel_data + (offset_lo - 0x19000000));
	    aboot_screen_puts(tmp, 0xFFFFFF);

        return 0;
    }
    
    return emmc_redirect_shim(offset_lo, offset_hi, dst, len);
}

void uart_putc_replace(int which, char c)
{
    char tmp[0x10];
    if (*(uint8_t*)0x8f6bd08c == 0) return;
    if (c == 0) return;
    if (c == '\r') return;
    snprintf(tmp, 0x10, "%c", c);
    aboot_screen_puts(tmp, 0xFFFFFF);
}

void cmd_boot_hybrid(const char *arg, void *data, unsigned sz)
{
    char tmp[0x40];
    kernel_data = (void*)0xC0000000;
    memcpy(kernel_data, data, sz);
    //memcpy((void*)0x8f6ff000, data, sz);
    
    // force crash
    //*(uint32_t*)0x8f6398dc = 0xFFFFFFFF;
    
    // sigchecks disable
    //*(uint32_t*)0x8f63af90 = 0xE3A00000;
    
    snprintf(tmp, 0x40, "CHECK %08lx\n", *(uint32_t*)0x8f6ff000);
    aboot_screen_puts(tmp, 0xFFFFFF);
    
    aboot_boot_linux_from_mmc();
    
    aboot_fastboot_ack("OKAY", "");
	//udc_stop();
}

void fastboot_extra()
{
    aboot_fastboot_register("boot", cmd_boot_hybrid);
}

uint8_t bootlog_replace_stash[0x10];

int pre_aboot(void* unk)
{
    printf("\n\nTEST %08lx %08lx %p\n", *(uint32_t*)0x8f600000, *(uint32_t*)0x8f63c36c, pre_aboot);
    
    // uart en patch
    *(uint32_t*)0x8f60f544 = 0;
    
    strcpy((char*)0x8f66e884, "LG_Stylo__4");
    strcpy((char*)0x8f66e894, "Plus");
    //strcpy((char*)0x8f679dec, " buildvariant=eng");

    // Adjust MMU mappings so we can hook
    *(uint32_t*)0x8f6721E4 = 0x1FF;
    *(uint32_t*)0x8f6721E8 = 0x1c0c;
    
    *(uint32_t*)0x8f6721f4 = 0xFF;
    
    *(uint32_t*)0x8f67221c = 0xC0000000;
    *(uint32_t*)0x8f672220 = 0xC0000000;
    *(uint32_t*)0x8f672224 = 0x1FF;
    *(uint32_t*)0x8f672228 = 0x1c0c;
    
    // partition size related
    *(uint32_t*)0x8f63ab30 = 0xe3a00301;
    *(uint32_t*)0x8f63ab34 = 0xe3a01000;
    
    // Allow download mode reboot
    //*(uint32_t*)0x8f60c444 = 0xE3A00001;
    //*(uint32_t*)0x8f60c448 = 0xE12FFF1E;
    //*(uint32_t*)0x8f6a52a4 = 0x1;

    // Patch sig check memcmp
    *(uint32_t*)0x8f608774 = 0xE3A00000;
    
    // Verified boot status = 0
    /**(uint32_t*)0x8f608fa4 = 0xE3A00000;
    *(uint32_t*)0x8f608fa8 = 0xE12FFF1E;
    
    // Patch res sig check
    *(uint32_t*)0x8f6094E4 = 0xE3A00001;
    *(uint32_t*)0x8f6094E8 = 0xE12FFF1E;*/
    
    // no sigchecks
    *(uint32_t*)0x8f608b68 = 0xE3A00000;
    *(uint32_t*)0x8f608b6c = 0xE12FFF1E;

    // No fastboot display
    *(uint32_t*)0x8f60d664 = 0;
    *(uint32_t*)0x8f63c3a0 = 0;
    *(uint32_t*)0x8f60d19c = 0; // no dlmode screen clear
    *(uint32_t*)0x8f60d1a8 = 0; // no dlmode image
    
    // Force mode
    *(uint32_t*)0x8f60683c = 0xe3a00000; // LAF 6, serial info 5, factory wipe 3, normal boot 0
    *(uint32_t*)0x8f606840 = 0xE12FFF1E;
    
    // disable USB barfing
    *(uint32_t*)0x8f62ea8c = 0xE12FFF1E;
    *(uint32_t*)0x8f6079b4 = 0x0; // get_image_info barf
    
    // Enable UART
    *(uint32_t*)0x8f6001A0 = 0;
    *(uint32_t*)0x8f6001A4 = 0;
    *(uint32_t*)0x8f6001A8 = 0;
    *(uint32_t*)0x8f6001AC = 0;

#if 1
    // Redirect uart putc to screen
    // No uart init check
    *(uint32_t*)0x8f60f53c = 0;
    *(uint32_t*)0x8f60f540 = 0;
    *(uint32_t*)0x8f60f544 = 0;

    // Don't call dputc in screen putc
    *(uint32_t*)0x8f608088 = 0;

    // Redirect uart putc to screen
    *(uint32_t*)0x8f61e028 = 0xe59f3000;
    *(uint32_t*)0x8f61e02C = 0xe12fff13;
    *(uint32_t*)0x8f61e030 = (uint32_t)uart_putc_replace;
#endif

    // Slot into nops for emmc redirect
    uint32_t nop_loc = (uint32_t)emmc_redirect_shim & 0xFFFFFFFE;
    *(uint32_t*)(nop_loc + 0) = *(uint32_t*)0x8f61c230;
    *(uint32_t*)(nop_loc + 4) = *(uint32_t*)0x8f61c234;
    *(uint32_t*)(nop_loc + 8) = *(uint32_t*)0x8f61c238;

    // Force fastboot
    *(uint32_t*)0x8f63c36c = 0x0;

    // Disable image length check
    *(uint32_t*)0x8f608d74 = 0xEA00001E;

    // Image size > partition size check
    //*(uint32_t*)0x8f63aed8 = 0x00000000;
    
    // Force fastboot
#if 1
    for (uint32_t i = 0x8f63c4e0; i <= 0x8f63c518; i += 4)
    {
        *(uint32_t*)i = 0x0;
    }
#endif

#if 1    
    // Fastboot extra registration
    *(uint32_t*)0x8f63dd4c = 0xe59f3004;
    *(uint32_t*)0x8f63dd50 = 0xe12fff33;
    *(uint32_t*)0x8f63dd54 = 0xEA000000;
    *(uint32_t*)0x8f63dd58 = (uint32_t)fastboot_extra;
    *(uint32_t*)0x8f63dd5C = 0;
#endif    
    // crash instead of entering kernel
    //*(uint32_t*)0x8f6398dc = 0xFFFFFFFF;

    //disable_mmu_caches();

    sbl_sleep_us(2000000);
    
    // Release screen prints and turn off screen
    memcpy((void*)0x08020C4C, bootlog_replace_stash, 0x10);
    sbl_fbfill(0x000000, 0xFFFFFF);
    sbl_deinit_screen();
    
    aboot_init();

    return sbl_pre_aboot(unk);
}

void continue_sbl1()
{
    // Disable pre funcs which we already ran.
    *(uint32_t*)0x0805B0C8 = 0;
    /**(uint32_t*)0x0805B0C8 = 0x08021610+1;
    *(uint32_t*)0x0805B0CC = 0x801EDE4+1;
    *(uint32_t*)0x0805B0D0 = 0x0801ECC0+1;
    *(uint32_t*)0x0805B0D4 = 0;*/
    
    // Log to screen
    memcpy(bootlog_replace_stash, (void*)0x08020C4C, 0x10);
    /**(uint16_t*)0x08020C4C = 0x4900;
    *(uint16_t*)0x08020C4E = 0x4708;
    *(uint32_t*)0x08020C50 = (uint32_t)bootlog_replace;*/
    
    // pmic idk
    *(uint8_t*)0x0805BF47 = 0;
    
    // Hook prior to aboot
    *(uint32_t*)0x0805B304 = (uint32_t)pre_aboot;
    
    // Change boot reason
    //*(uint32_t*)0x860065C = 0;
        
    sbl_process_configs();
}

void __attribute__ ((naked)) __attribute__ ((target ("arm"))) reboot_test(void)
{
    __asm__("ldr r0, =0xC0000000\n"
		    "ldr r1, =0xABCD1234\n"
		    "str r1, [r0]\n"
		    "ldr r0, =0x100000\n"
		    "bx r0\n");
}

int main(void)
{
    sbl_init();
    screen_init();
    sbl_fbupdate();
    
    printf(">   *hacker voice*  I'm in...\n>   Hello from EL3!\n");
    
    // UART patches
    *(uint16_t*)0x080207F6 = 0;
    *(uint16_t*)0x080207FC = 0;
    *(uint16_t*)0x08020800 = 0;
    *(uint16_t*)0x08020804 = 0;

    /*printf("Press Vol Up to continue booting...\n");
    while (1)
    {
        if (gpio_get_volume_up()) break;
        sbl_sleep_us(5000);
    }*/
    
    printf("%08lx %08lx\n", *(volatile uint32_t*)0x1940000, *(uint32_t*)0xC0000000);
    
    *(uint32_t*)0xC0000000 = 0xFFFFFFFF;
    sbl_sleep_us(100000);

    continue_sbl1();

    return 0;
}
