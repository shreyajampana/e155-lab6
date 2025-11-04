// STM32L432KC_SPI.c
// Shreya Jampana
// sjampana@hmc.edu
// 10/18/25
// Description: This file contains code to intialize the SPI transaction
// and transmit and receive characters over SPI. 

#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_SPI.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_GPIO.h"


// Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase. 
void initSPI(int br, int cpol, int cpha) {

    // Turn on GPIOA and GPIOB clock domains (GPIOAEN and GPIOBEN bits in AHB1ENR)
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN);

    // Turning on the SPI1 clock in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Enabling GPIOs
    pinMode(SPI_SCLK, GPIO_ALT);
    pinMode(SPI_CIPO, GPIO_ALT);
    pinMode(SPI_COPI, GPIO_ALT);
    pinMode(SPI_CS, GPIO_OUTPUT); 

    // Setting the output speed type to be high
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);

    // Set to AF05 for SPI alternate functions
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);

    // Setting the baud rate divider
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);

    // Setting STM to be the mstr
    SPI1->CR1 |= (SPI_CR1_MSTR);

    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_SSM);

    // Setting the clock polarity and clock phase
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);

    // Setting an 8-bit data size 
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);
    SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);

    // Enabling SPI
    SPI1->CR1 |= (SPI_CR1_SPE);
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
}