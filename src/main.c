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

static int readRegister(uint8_t reg, uint8_t *values, uint8_t size);

/* Devicetree node corresponding to the peripheral to be used via Zephyr SPI
 * driver (SPIM1), in the background transfer.
 */
#define SPI_DEV_NODE DT_NODELABEL(spi3)
const struct device *const spi_dev = DEVICE_DT_GET(SPI_DEV_NODE);

static const struct spi_config spi_dev_cfg = {
	.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) |
				 SPI_TRANSFER_MSB,
	.frequency = 1000000,
	.cs = {
		.gpio = GPIO_DT_SPEC_GET(SPI_DEV_NODE, cs_gpios),
	},
};

int main(void)
{
	int err;
	printk("Program started \n");

	if (!device_is_ready(spi_dev))
	{
		printk("%s is not ready\n", spi_dev->name);
		return 0;
	}

	uint8_t values[1];
	while (1)
	{
		int ret = readRegister(ADXL362_REG_DEVID_MST, values, 1);
		if (ret == 0)
		{
			printk("Read chip ID failed \n");
			k_msleep(1000);
		}
		else
		{
			printk("Register chip ID: %.2x\n", values[0]);
			k_msleep(1000);
		}
	}
	return 0;
}

// According to the ADXL362 datasheet (https://www.analog.com/media/en/technical-documentation/data-sheets/adxl362.pdf)
// Figure 36, we follow multi byte structure where first byte is ADXL362_READ_REG and second byte is the register address
static int readRegister(uint8_t reg, uint8_t *values, uint8_t size)
{
	int err;
	uint8_t tx_buffer[2];
	tx_buffer[0] = ADXL362_READ_REG;
	tx_buffer[1] = reg;

	struct spi_buf tx_spi_buf = {
		.buf = tx_buffer,
		.len = 2};

	struct spi_buf_set spi_tx_buffer_set = {
		.buffers = &tx_spi_buf,
		.count = 1};

	struct spi_buf rx_spi_buf[] = {
		{.buf = NULL,
		 .len = sizeof(tx_buffer)},
		{.buf = values,
		 .len = size}};

	struct spi_buf_set spi_rx_buffer_set = {
		.buffers = rx_spi_buf,
		.count = ARRAY_SIZE(rx_spi_buf)};

	// struct spi_buf rx_spi_buf = {
	// 	.buf = values,
	// 	.len = size};

	// struct spi_buf_set spi_rx_buffer_set = {
	// 	.buffers = &rx_spi_buf,
	// 	.count = 1};

	err = spi_transceive(spi_dev, &spi_dev_cfg, &spi_tx_buffer_set, &spi_rx_buffer_set);
	if (err)
	{
		printk("SPI error: %d\n", err);
		return 0;
	}
	return 1;
}