/*
 * Filename    : types.h
 * Created     : 2025-05-08
 * Description : Minimal custom types for the gazelle RTOS.
 * Author      : Gabe DiFiore
 */

#ifndef TYPES_H
#define TYPES_H

#include <avr/io.h>

// Unsigned integers
//typedef unsigned char      uint8_t;
//typedef unsigned short     uint16_t;
//typedef unsigned long      uint32_t;

// Signed integers
//typedef signed char        int8_t;
//typedef signed short       int16_t;
//typedef signed long        int32_t;

// Boolean type (optional)
typedef uint8_t            bool;
#define true               1
#define false              0

#endif // TYPES_H
