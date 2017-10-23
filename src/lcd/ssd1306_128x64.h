/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of SSD1306 library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file ssd1306_128x64.h support for OLED 128x64 display
 */


#ifndef _SSD1306_128X64_H_
#define _SSD1306_128X64_H_

#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_DISPLAY_API LCD Display control functions
 * @{
 */

/**
 * Inits 128x64 OLED display over i2c (based on SSD1306 controller)
 */
void         ssd1306_128x64_i2c_init();

/**
 * @copydoc ssd1306_128x64_i2c_init
 */
void         ssd1306_init() __attribute__((deprecated));

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif // _SSD1306_128X64_H_