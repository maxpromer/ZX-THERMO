#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "MLX90614.h"

void i2c_setClock(uint32_t clock) {
	// Reset speed of I2C
	i2c_config_t conf;

	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = CHAIN_SDA_GPIO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = CHAIN_SCL_GPIO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = clock;

	i2c_param_config(I2C_NUM_1, &conf);
}

MLX90614::MLX90614(int bus_ch, int dev_addr) {
	channel = bus_ch;
	address = dev_addr;
}

void MLX90614::init(void) {
	state = s_detect;
	i2c_setClock(100E3);
}

int MLX90614::prop_count(void) {
	// not supported
	return 0;
}

bool MLX90614::prop_name(int index, char *name) {
	// not supported
	return false;
}

bool MLX90614::prop_unit(int index, char *unit) {
	// not supported
	return false;
}

bool MLX90614::prop_attr(int index, char *attr) {
	// not supported
	return false;
}

bool MLX90614::prop_read(int index, char *value) {
	// not supported
	return false;
}

bool MLX90614::prop_write(int index, char *value) {
	// not supported
	return false;
}

void MLX90614::process(Driver *drv) {
	i2c = (I2CDev *)drv;

	switch (state) {
		case s_detect:
			// detect i2c device
			if (i2c->detect(channel, address) == ESP_OK) {
				error = false;
				initialized = true;
				state = s_poll;
			} else {
				state = s_error;
			}
			break;

		case s_poll:
			if (is_tickcnt_elapsed(tickcnt, 100)) {
				tickcnt = get_tickcnt();
				
				uint8_t reg;
				uint8_t data[3];
				
				reg = 0x06;
				
				if (i2c->read(channel, address, &reg, 1, data, 3) != ESP_OK) {
					state = s_error;
					break;
				}

				tempAmbient = (data[1] << 8) | data[0];
        		tempAmbient = (tempAmbient * 0.02) - 273.15;

				reg = 0x07;
				
				if (i2c->read(channel, address, &reg, 1, data, 3) != ESP_OK) {
					state = s_error;
					break;
				}

				tempObject = (data[1] << 8) | data[0];
        		tempObject = (tempObject * 0.02) - 273.15;
			}
			break;

		case s_error:
			// set error flag
			error = true;
			// clear initialized flag
			initialized = false;
			// get current tickcnt
			tickcnt = get_tickcnt();
			// goto wait and retry with detect state
			state = s_wait;
			tempObject = -99;
			tempAmbient  = -99;
			break;

		case s_wait:
			// delay 1000ms before retry detect
			if (is_tickcnt_elapsed(tickcnt, 1000)) {
				state = s_detect;
			}
			break;
	}
}

double MLX90614::readObjectTempC() {
	return tempObject;
}

double MLX90614::readAmbientTempC() {
	return tempAmbient;
}
