/*
File: Lab_6_JHB.c
Author: Shreya Jampana
Email: sjampana@hmc.edu
Date: 10/18/25
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "/Users/shreyajampana/e155-lab6/mcu/src/main.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_GPIO.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_FLASH.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_RCC.h"
#include "/Users/shreyajampana/e155-lab6/mcu/lib/DS1722.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* tempStr = "<p>Temperature Resolution Control:</p><form action=\"8-bit\"><input type=\"submit\" value=\"8-bit Resolution\"></form>\
    </p><form action=\"9-bit\"><input type=\"submit\" value=\"9-bit Resolution\"></form>\
    </p><form action=\"10-bit\"><input type=\"submit\" value=\"10-bit Resolution\"></form>\
    </p><form action=\"11-bit\"><input type=\"submit\" value=\"11-bit Resolution\"></form>\
    </p><form action=\"12-bit\"><input type=\"submit\" value=\"12-bit Resolution\"></form>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(LED_PIN, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // Add SPI initialization code
  configureTemp();


  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // Add SPI code here for reading temperature
    float tempOutput = getTemp();
    char tempString[16] = getTemp(); 
    sprintf(tempString, %s ,tempOutput)

    // Update string with current LED state
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, tempStr); // button for resolution

    sendString(USART, "<h2>LED Status</h2>");

    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    // Adding temperature things
    sendString(USART, "<h2>Temperature Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, tempString);
    sendString(USART, "</p>");

    // Adding resolution button
    sendString(USART, "<h2>Temperature Resolution</h2>");

    sendString(USART, webpageEnd);
  }
}