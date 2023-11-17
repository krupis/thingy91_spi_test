#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define ADXL362_WRITE_REG 0x0A
#define ADXL362_READ_REG 0x0B
#define ADXL362_READ_FIFO 0x0D

#define ADXL362_REG_DEVID_AD 0x00
#define ADXL362_REG_DEVID_MST 0x01
#define ADXL362_REG_PARTID 0x02

static int readRegister(uint8_t reg, uint8_t *values, uint8_t size);

#define DEFAULT_ADXL362_NODE DT_ALIAS(adxl362)
BUILD_ASSERT(DT_NODE_HAS_STATUS(DEFAULT_ADXL362_NODE, okay),
			 "ADXL362 not specified in DT");

// DEVICE TREE STRUCTURE
const struct device *adxl1362_sens = DEVICE_DT_GET(DEFAULT_ADXL362_NODE);

// SPI CONFIG
static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.frequency = 1000000, // 1 mhz
	.slave = 0,
	//.cs = &ctrl,
};

// CS GPIO CONFIG
#define MY_GPIO0 DT_NODELABEL(gpio0)
#define GPIO0_8_CS 8
#define GPIO0_7_CS 8
const struct device *gpio0_dev = DEVICE_DT_GET(MY_GPIO0);

int main(void)
{
	int err;
	err = gpio_pin_configure(gpio0_dev, GPIO0_8_CS, GPIO_OUTPUT);
	if (err < 0)
	{
		printk("GPIO0 8 pin configure failed with error %d\n", err);
		return 0;
	}
	err = gpio_pin_configure(gpio0_dev, GPIO0_7_CS, GPIO_OUTPUT);
	if (err < 0)
	{
		printk("GPIO0 7 pin configure failed with error %d\n", err);
		return 0;
	}
	gpio_pin_set(gpio0_dev, GPIO0_7_CS, 1); // disable by default
	gpio_pin_set(gpio0_dev, GPIO0_8_CS, 1); // disable by default

	uint8_t values[1];
	while (1)
	{
		int ret = readRegister(ADXL362_REG_DEVID_AD, values, 1);
		if (ret == 0)
		{
			printk("Read chip ID failed \n");
			k_msleep(1000);
		}
		else
		{
			printk("Register chip ID: %d\n", values[0]);
			k_msleep(1000);
		}
	}
	return 0;
}

static int readRegister(uint8_t reg, uint8_t *values, uint8_t size)
{
	int err;
	uint8_t tx_buffer[2];
	tx_buffer[0] = ADXL362_READ_REG;
	tx_buffer[1] = reg;

	struct spi_buf tx_spi_buf = {
		.buf = tx_buffer,
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

	//  chip select pin set to low
	gpio_pin_set(gpio0_dev, GPIO0_8_CS, 0);
	err = spi_write(adxl1362_sens, &spi_cfg, &spi_tx_buffer_set);
	if (err < 0)
	{
		printk("SPI write failed with error %d\n", err);
		return 0;
	}
	err = spi_read(adxl1362_sens, &spi_cfg, &spi_rx_buffer_set);
	// chip select pin set to high
	gpio_pin_set(gpio0_dev, GPIO0_8_CS, 1);
	if (err < 0)
	{
		printk("SPI read failed with error %d\n", err);
		return 0;
	}
}
