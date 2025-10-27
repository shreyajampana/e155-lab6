// DS1722.h
// Shreya Jampana
// sjampana@hmc.edu
// 10/18/25
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef DS1722_H
#define DS1722_H

#include <stdint.h>
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

// write comments
void configureTemp();

void setResolution(int res);

void getTemp();

#endif