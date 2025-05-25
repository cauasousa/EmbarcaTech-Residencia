#include <stdio.h>
#include <string.h>
#include "../../inc/ssd1306.h"
#include "hardware/i2c.h"

#ifndef SETUP_DISPLAY
#define SETUP_DISPLAY

#define I2C_SDA 14
#define I2C_SCL 15

void setup_display();
void write_display(char *text);

#endif