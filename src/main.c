#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define ADXL362_WRITE_REG 0x0A
#define ADXL362_READ_REG 0x0B
#define ADXL362_READ_FIFO 0x0D

#define ADXL362_REG_DEVID_AD 0x00

static int readRegister(uint8_t reg, uint8_t *values, uint8_t size);

#define MY_SPI_MASTER DT_ALIAS(adxl362)
struct spi_dt_spec spec = SPI_DT_SPEC_GET(DT_NODELABEL(adxl362), SPI_WORD_SET(8) | SPI_MODE_GET(0), 1);

int main(void)
{
	int err;
	printk("Program started \n");
	if (spi_is_ready(&spec) == true)
	{
		printk("SPI is ready \n");
	}
	else
	{
		printk("SPI is not ready \n");
	}

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
			printk("Register chip ID:%.2x\n", values[0]);
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

	err = spi_transceive_dt(&spec, &spi_tx_buffer_set, &spi_rx_buffer_set);

	if (err)
	{
		printk("SPI error: %d\n", err);
		return 0;
	}
	return 1;
}