#ifndef SBL_H
#define SBL_H

#include <stdint.h>
#include <stdbool.h>

void sbl_init();
void sbl_switch_partition(const uint8_t* guid);
void sbl_restore_partition(void);

const uint8_t raw_resources_a_guid[16];
const uint8_t aboot_a_guid[16];
const uint8_t sbl1_a_guid[16];
const uint8_t sec_guid[16];

uint8_t* sbl_framebuffer;
uint8_t** sbl_res_guid;

void (*sbl_fbfill)(uint32_t bgcolor, uint32_t fgcolor);
void (*sbl_fbupdate)(void);
void (*sbl_fbupdate2)(int unk);
void (*sbl_coninit)(void *buf, uint32_t size);
void (*sbl_reset)(bool idk);
void (*sbl_sleep_us)(uint32_t count);
void (*sbl_screen_log_msg)(char* str);
void (*sbl_uart_log_msg)(char* str);
void (*sbl_dload_enter)(void);
void (*sbl_get_image_data)(void* dst, uint32_t offset, uint32_t size);
void (*sbl_start)(void* pbl_info);
void (*sbl_process_configs)(void);
void (*sbl_deinit_screen)(void);
int (*sbl_pre_aboot)(void* unk);

#endif // SBL_H
