#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define SPI_MESSAGE 0x5A
static int readRegister(uint8_t reg, uint8_t* values, uint8_t size);

#define DEFAULT_ADXL362_NODE DT_ALIAS(adxl362)
BUILD_ASSERT(DT_NODE_HAS_STATUS(DEFAULT_ADXL362_NODE, okay),
			 "ADXL362 not specified in DT");

// DEVICE TREE STRUCTURE
const struct device *adxl1362_sens = DEVICE_DT_GET(DEFAULT_ADXL362_NODE);

// CHIP SELECT CONTROL
struct spi_cs_control ctrl = SPI_CS_CONTROL_INIT(DT_NODELABEL(adxl362), 0);

// SPI CONFIG
static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.frequency = 1000000, // 1 mhz
	.slave = 0,
	//.cs = &ctrl,
};

//CS GPIO CONFIG
 #define ADXL362_CS_GPIO_NODE DT_NODELABEL(spi3)
 #define ADXL362_CS_GPIO_PIN 8
 const struct device* cs_gpio_dev = DEVICE_DT_GET(ADXL362_CS_GPIO_NODE);

int main(void)
{
	printk("Program started \n");
	gpio_pin_configure(cs_gpio_dev,ADXL362_CS_GPIO_PIN,GPIO_OUTPUT);
	k_msleep(1000);
	// while(1){
	// 	printk("Hello world \n");
	// 	k_msleep(1000);
	// }

	printk("getting into while loop \n");
	uint8_t values[1];
	while(1)
	{
		int ret = readRegister(0x00,values,1);
		if (ret == 0)
		{
			printk("Read chip ID failed \n");
			return 0;
		}
		printk("Register chip ID: %d\n",values[0]);
		k_msleep(1000);
	}
	
	/*
	int err;
	uint8_t cmd = SPI_MESSAGE;
	struct spi_buf tx_buf = {.buf = &cmd, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
	while (1)
	{
		LOG_INF("SPI writing test data \n");
		// gpio_pin_set(cs_gpio_dev,ADXL362_CS_GPIO_PIN,1);
		err = spi_write(adxl1362_sens, &spi_cfg, &tx_bufs);
		if (err)
		{
			LOG_ERR("SPI write failed with error %d\n", err);
			return err;
		}
		// gpio_pin_set(cs_gpio_dev,ADXL362_CS_GPIO_PIN,0);

		k_sleep(K_MSEC(1000));
	}
	*/

	return 0;
}

static int readRegister(uint8_t reg, uint8_t* values, uint8_t size)
{
	int err;
	uint8_t tx_buffer[1];
	tx_buffer[0] = reg;

	struct spi_buf tx_spi_buf = {
		.buf = &tx_buffer,
		.len = sizeof(tx_buffer)};

	struct spi_buf_set spi_tx_buffer_set = {
		.buffers = &tx_spi_buf,
		.count = 1};

	struct spi_buf rx_spi_buf = {
		.buf = values,
		.len = size};

	struct spi_buf_set spi_rx_buffer_set = {
		.buffers = &rx_spi_buf,
		.count = 1};

	// gpio pin set
	gpio_pin_set(cs_gpio_dev,ADXL362_CS_GPIO_PIN,0);

	err = spi_write(adxl1362_sens, &spi_cfg, &spi_tx_buffer_set);
	if (err < 0)
	{
		printk("SPI write failed with error %d\n", err);
		return 0;
	}
	err = spi_read(adxl1362_sens, &spi_cfg, &spi_rx_buffer_set);
	gpio_pin_set(cs_gpio_dev,ADXL362_CS_GPIO_PIN,1);

	if (err < 0)
	{
		printk("SPI read failed with error %d\n", err);
		return 0;
	}
	
}