#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);


//CS GPIO CONFIG
 #define ADXL362_CS_GPIO_NODE DT_NODELABEL(spi3)
 #define ADXL362_CS_GPIO_PIN 8
 const struct device* gpio_dt_spec = DEVICE_DT_GET(ADXL362_CS_GPIO_NODE);

int main(void)
{
	int err;
	printk("Program started \n");
	err = gpio_pin_configure(gpio_dt_spec,ADXL362_CS_GPIO_PIN,GPIO_OUTPUT);
	if (err < 0)
	{
		printk("GPIO pin configure failed with error %d\n", err);
		return 0;
	}
	k_msleep(1000);
	printk("getting into while loop \n");
	return 0;
}

