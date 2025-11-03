// DS1722.c
// Shreya Jampana
// sjampana@hmc.edu
// 10/18/25
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include <stdio.h>
#include <stdint.h>
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_SPI.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_GPIO.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\DS1722.h"

//MSb first due to SPI communication
void configureTemp() {
    // Initializing SPI
    initSPI(0b110, 0, 1);
}

void setResolution(int res) {
    // Creating the "base template" for the configuration register
    int config = 0b11100000;

    // Reassigning config based on user input of resolution
    if (res == 8) {
        config = 0b11100000;
    }
    else if (res == 9) {
        config = 0b11100010;
    }
    else if (res == 10) {
        config = 0b11100100;
    }
    else if (res == 11) {
        config = 0b11100110;
    }
    else if (res == 12) {
        config = 0b11101000;
    }
    else {
        config = 0b11100000; // default = 8-bit
    }

    // Writing the new config value to the configuration register
    digitalWrite(SPI_CS, 1);
    spiSendReceive((char)0x80);   
    spiSendReceive((char)config);
    digitalWrite(SPI_CS, 0);
}


float getTemp(void) {
    // Read the data (from MSb first due to SPI)
    digitalWrite(SPI_CS, 1);
    spiSendReceive(0x02);
    char temp_msb = spiSendReceive(0x00); //dummy 
    digitalWrite(SPI_CS, 0);

    digitalWrite(SPI_CS, 1);
    spiSendReceive(0x01);
    char temp_lsb = spiSendReceive(0x00); //dummy
    digitalWrite(SPI_CS, 0);

    // Combining MSB and LSB
    int16_t raw_data = (uint16_t)temp_msb << 8 | temp_lsb;

    // Converting from longer integer to a temperature
    float temp = (float)(raw_data) / (float)256;

    return temp;
}


