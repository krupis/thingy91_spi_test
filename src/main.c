#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

// Define the CS pin
#define CS_PIN 8
const struct device *gpio_dev;

int main(void)
{
	int err;
	printk("Program started printk\n");
	printf("Program started pritnf \n");
	LOG_DBG("Program started log_dbg\n");
	gpio_pin_configure(gpio_dev, CS_PIN, GPIO_OUTPUT);

	if (err < 0)
	{
		printk("GPIO pin configure failed with error %d\n", err);
		return 0;
	}
	k_msleep(1000);
	printk("getting into while loop \n");
	return 0;
}