#include "gpio.h"
#include "sbl.h"

#define TLMM_VOL_UP_BTN_GPIO    85

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
			uint8_t dir, uint8_t pull,
			uint8_t drvstr, uint32_t enable)
{
	GPIO_CONFIG_ADDR(gpio) = pull | (func << 2) | (drvstr << 6) | (enable << 9);
	return;
}

void gpio_set_dir(uint32_t gpio, uint32_t dir)
{
	GPIO_IN_OUT_ADDR(gpio) = dir;

	return;
}

uint32_t gpio_status(uint32_t gpio)
{
	return GPIO_IN_OUT_ADDR(gpio) & GPIO_IN;
}

/* Return 1 if vol_up pressed */
int gpio_get_volume_up()
{
	uint8_t status = 0;

	gpio_tlmm_config(TLMM_VOL_UP_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

	/* Wait for the gpio config to take effect - debounce time */
	sbl_sleep_us(10);

	/* Get status of GPIO */
	status = gpio_status(TLMM_VOL_UP_BTN_GPIO);

	/* Active high signal. */
	return !status;
}
