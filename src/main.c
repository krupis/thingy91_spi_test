#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>



#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define SPI_MESSAGE 0x48


#define DEFAULT_ADXL362_NODE DT_ALIAS(adxl362)
BUILD_ASSERT(DT_NODE_HAS_STATUS(DEFAULT_ADXL362_NODE, okay),
			 "ADXL362 not specified in DT");

//DEVICE TREE STRUCTURE
const struct device* adxl1362_sens = DEVICE_DT_GET(DEFAULT_ADXL362_NODE);

//CHIP SELECT CONTROL
struct spi_cs_control ctrl = SPI_CS_CONTROL_INIT(DT_NODELABEL(adxl362), 0);

//SPI CONFIG
static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.frequency = 4000000, // 8 mhz
	.slave = 0,
    .cs = &ctrl,
};

int main(void)
{
	int err;
	uint8_t cmd = SPI_MESSAGE;
	struct spi_buf tx_buf = {.buf = &cmd, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
	while (1) {
		LOG_INF("SPI writing test data \n");
		err = spi_write(adxl1362_sens, &spi_cfg, &tx_bufs);
		if (err) {
			LOG_ERR("SPI write failed with error %d\n", err);
			return err;
		}
		k_sleep(K_MSEC(1000));
	}
	
	return 0;
}


