#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#define MY_SPI_MASTER DT_NODELABEL(my_spi_master)

#define DEVICE_ID 0xAD

// SPI master functionality
const struct device *spi_dev;

struct spi_cs_control spim_cs = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(adxl362)),
	.delay = 0,
};

static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.frequency = 4000000,
	.slave = 0,
	.cs = &spim_cs,
};

static void spi_init(void)
{
	spi_dev = DEVICE_DT_GET(MY_SPI_MASTER);
	if(!device_is_ready(spi_dev)) {
		printk("SPI master device not ready!\n");
	}
	else{
		printk("SPI master device ready!\n");
	}
	
	if(!device_is_ready(spim_cs.gpio.port)){
		printk("SPI master chip select device not ready!\n");
	}
	else{
		printk("SPI master chip select device ready!\n");
	}
}




static int spi_test(void)
{
	static uint8_t tx_buffer[3];
	static uint8_t rx_buffer[3];

	const struct spi_buf tx_buf = {
		.buf = tx_buffer,
		.len = sizeof(tx_buffer)
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};

	struct spi_buf rx_buf = {
		.buf = rx_buffer,
		.len = sizeof(rx_buffer),
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1
	};


	/*
	first 2 bytes of tx specify read/write action and on which register to perform
	last byte of rx contains the respons from the accelerometer
	*/
	tx_buffer[0] = 0x0B;
	tx_buffer[1] = 0x00;

	int error = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}


	if(DEVICE_ID == rx_buffer[2])
	{
		printk("Successfull ACC communication\n");
	}
	else
	{
		printk("Faild ACC communication\n");
	}

	return 0;
}


int main(void)
{
	printk("Start program \n");
	spi_init();
	spi_test();
	return 0;
}	