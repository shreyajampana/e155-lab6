/*
File: Lab_6.c
Author: Shreya Jampana
Email: sjampana@hmc.edu
Date: 10/18/25
DescriptionL This is the main.c file containing the UART code for the webpage. It also contains
the SPI initialization and send/receive function calls, as well as the code for blinking the 
onboard LED and displaying the temperature on the website. 
*/

int res;

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\src\main.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_GPIO.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_FLASH.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\STM32L432KC_RCC.h"
#include "C:\Users\sjampana\Downloads\e155-lab6\e155-lab6\mcu\lib\DS1722.h"

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

int updatetempStatus(char request[])
{
	if (inString(request, "8-bit")==1) {
      res = 8;
	} else if (inString(request, "9-bit")==1) {
		  res = 9;
	} else if (inString(request, "10-bit")==1) {
		  res = 10;
  } else if (inString(request, "11-bit")==1) {
		  res = 11;
  } else if (inString(request, "12-bit")==1) {
		  res = 12;
  } 
    return res;
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

    // Update string with current LED state
    int led_status = updateLEDStatus(request);

    // Updating resolution with user request
    int res_status = updatetempStatus(request);
    setResolution(res_status);

    float tempOutput = getTemp();
    delay_millis(TIM15, 100);

    char resStatusStr[50];
    if (res_status == 8) {
      sprintf(resStatusStr, "Temperature: %.0f C", tempOutput);
    } else if (res_status == 9) {
      sprintf(resStatusStr, "Temperature: %.1f C", tempOutput);
    } else if (res_status == 10) {
      sprintf(resStatusStr, "Temperature: %.2f C", tempOutput);
    } else if (res_status == 11) {
      sprintf(resStatusStr, "Temperature: %.3f C", tempOutput);
    } else if (res_status == 12) {
      sprintf(resStatusStr, "Temperature: %.4f C", tempOutput);
    }

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // Finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code

    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, "<h2>LED Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, "<h2>Temperature Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, resStatusStr);
    sendString(USART, "</p>");

    // Adding resolution button
    sendString(USART, "<h2>Temperature Resolution</h2>");
    sendString(USART, tempStr); // button for controlling temperature resolution

    sendString(USART, webpageEnd);
  }
}