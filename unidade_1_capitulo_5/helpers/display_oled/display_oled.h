#ifndef SETUP_DISPLAY
#define SETUP_DISPLAY

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

void setup_display();
void write_display(char *text[]);

#endif