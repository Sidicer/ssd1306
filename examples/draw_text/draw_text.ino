/*
    Copyright (C) 2016-2017 Alexey Dynda

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
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5
 */

#include "ssd1306.h"

/* Do not include wire.h for Attiny controllers */
#ifdef SSD1306_WIRE_SUPPORTED
    #include <Wire.h>
#endif


void setup()
{
    /* Do not init Wire library for Attiny controllers */
#ifdef SSD1306_WIRE_SUPPORTED
    Wire.begin();
#endif
    /* Replace the line below with ssd1306_128x32_i2c_init() if you need to use 128x32 display */
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    ssd1306_charF6x8(0, 0, "Line 1. text");
    ssd1306_charF6x8(8, 1, "Line 2. 8 pixels to the right");
    ssd1306_charF6x8(0, 4, "Line 5. Bold text", STYLE_BOLD);
    ssd1306_charF6x8(0, 6, "Line 7. Italic text", STYLE_ITALIC);
}


void loop()
{
}




