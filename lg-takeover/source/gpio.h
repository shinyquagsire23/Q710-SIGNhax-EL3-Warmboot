#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "utils.h"


/* GPIO TLMM: Direction */
#define GPIO_INPUT      0
#define GPIO_OUTPUT     1

/* GPIO TLMM: Pullup/Pulldown */
#define GPIO_NO_PULL    0
#define GPIO_PULL_DOWN  1
#define GPIO_KEEPER     2
#define GPIO_PULL_UP    3

/* GPIO TLMM: Drive Strength */
#define GPIO_2MA        0
#define GPIO_4MA        1
#define GPIO_6MA        2
#define GPIO_8MA        3
#define GPIO_10MA       4
#define GPIO_12MA       5
#define GPIO_14MA       6
#define GPIO_16MA       7

/* GPIO TLMM: Status */
#define GPIO_ENABLE     0
#define GPIO_DISABLE    1

/* GPIO_IN_OUT register shifts. */
#define GPIO_IN         BIT(0)
#define GPIO_OUT        BIT(1)

#define TLMM_BASE_ADDR                     0x1000000
#define GPIO_CONFIG_ADDR(x)                (*(vu32*)(TLMM_BASE_ADDR + (x)*0x1000))
#define GPIO_IN_OUT_ADDR(x)                (*(vu32*)(TLMM_BASE_ADDR + 0x00000004 + (x)*0x1000))

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
			uint8_t dir, uint8_t pull,
			uint8_t drvstr, uint32_t enable);
void gpio_set_dir(uint32_t gpio, uint32_t dir);
uint32_t gpio_status(uint32_t gpio);

int gpio_get_volume_up();

#endif // GPIO_H
