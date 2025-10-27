// STM32L432KC_SPI.c
// Shreya Jampana
// sjampana@hmc.edu
// 10/18/25
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_SPI.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_GPIO.h"


// Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase. 
void initSPI(int br, int cpol, int cpha) {

    // Turning on the SPI1 clock in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    // Enabling GPIOs
    pinMode(SPI_SCLK, GPIO_ALT);
    pinMode(SPI_CIPO, GPIO_ALT);
    pinMode(SPI_COPI, GPIO_ALT);
    pinMode(SPI_CS, GPIO_OUTPUT);

    // Setting the output speed type to be high
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);

    // Setting to AFSEL5
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

    // Setting the baud rate divider
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);

    // Setting STM to be the mstr
    SPI1->CR1 |= (SPI_CR1_MSTR); 

    // Setting the clock polarity and clock phase
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);// clock polarity
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpha);// clock phase

    // Setting an 8-bit data size 
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1);

    // Enabling SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

// Transmits a character (1 byte) over SPI and returns the received character.
char spiSendReceive(char send) {
    
    // Waiting until the transmit buffer is empty
    while(!(SPI1->SR & SPI_SR_TXE)); //do we need the first part of this?

    // Putting the send input into the data register
    *(volatile char *) (&SPI1->DR) = send;

    // Waiting until the data is received
    while(!(SPI1->SR & SPI_SR_RXNE));

    // Getting and returning received character
    char receive = (volatile char) SPI1->DR;
    return receive; 

    // Pulling down chip select
    digitalWrite(SPI_CS, 0);
}