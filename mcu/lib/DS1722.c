// DS1722.c
// Shreya Jampana
// sjampana@hmc.edu
// 10/18/25
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_SPI.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_GPIO.h"

//MSb first due to SPI communication
void configureTemp() {
    // Initializing SPI
    initSPI(101, 0, 1);

    // Pulling up chip select
    digitalWrite(SPI_CS, 1);
    
    // Sending the write address of the configuration register
    spiSendReceive((char)0x80);

    // Sending the value to set the configuration register
    spiSendReceive((char)0b11100000);

    // Pulling up chip select
    digitalWrite(SPI_CS, 0);
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
        config = 0b11100000; // default = 9-bit
    }

    // Writing the new config value to the configuration register
    digitalWrite(SPI_CS, 1);
    spiSendReceive((char)0x80);   
    spiSendReceive((char)config);
    digitalWrite(SPI_CS, 0);
}


float getTemp(void) {
    int8_t temp_msb;
    int8_t temp_lsb;
    
    // Read the data (from MSb first due to SPI)
    digitalWrite(SPI_CS, 1);
    temp_msb = spiSendReceive((char)0x01); 
    digitalWrite(SPI_CS, 0);

    digitalWrite(SPI_CS, 1);
    temp_lsb = spiSendReceive((char)0x02);
    digitalWrite(SPI_CS, 0);

    // Combining MSB and LSB
    int16_t raw_data = (uint16_t)temp_msb << 8 | temp_lsb;

    // Converting from longer integer to a temperature
    float temp = (float)(raw_data) / 256;

    return temp;
}
