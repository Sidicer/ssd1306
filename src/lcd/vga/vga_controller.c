/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "vga_controller_base.h"
// Never include vga_controller.h here!!!
#include "intf/ssd1306_interface.h"
#include "lcd/lcd_common.h"

extern uint16_t ssd1306_color;

uint8_t s_vga_buffer[64*40/2] = {0};

static uint8_t s_vga_command = 0xFF;
static uint8_t s_vga_arg = 0;
static uint8_t s_column = 0;
static uint8_t s_column_end = 0;
static uint8_t s_cursor_x = 0;
static uint8_t s_cursor_y = 0;

static void vga_controller_init(void)
{
    s_vga_command = 0xFF;
}

static void vga_controller_stop(void)
{
    s_vga_command = 0xFF;
}

static void vga_controller_close(void)
{
}

static void vga_controller_put_pixel4(uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t mask;
    if ((x&1))
    {
        mask = 0x0F;
        color <<= 4;
    }
    else
    {
        mask = 0xF0;
    }
    uint16_t addr = (x + y*ssd1306_lcd.width)/2;
    s_vga_buffer[addr] &= mask;
    s_vga_buffer[addr] |= color;
}

static void vga_controller_send_byte4(uint8_t data)
{
    if (s_vga_command == 0xFF)
    {
        s_vga_command = data;
        return;
    }
    if (s_vga_command == 0x40)
    {
        uint8_t color = ((data & 0x80) >> 5) | ((data & 0x10) >> 3) | ((data & 0x02)>>1);
        vga_controller_put_pixel4(s_cursor_x, s_cursor_y, color);
/*        if (s_page == 0xFF)
        {
            s_cursor_x++;
            if (s_cursor_x > s_column_end)
            {
                s_cursor_x = s_column;
                s_cursor_y++;
            }
        }
        else*/
        {
            s_cursor_y++;
            if ((s_cursor_y & 0x07) == 0)
            {
                s_cursor_y -= 8;
                s_cursor_x++;
            }
        }
        return;
    }
    if (!s_vga_command)
    {
        s_vga_command = data;
        s_vga_arg = 0;
    }
    if (s_vga_command == 0x01)
    {
        // set block
        if (s_vga_arg == 1) { s_column = data; s_cursor_x = data; }
        if (s_vga_arg == 2) { s_column_end = data; }
        if (s_vga_arg == 3) { s_cursor_y = data; }
    }
    s_vga_arg++;
    // command mode
}

static void vga_controller_send_bytes(const uint8_t *buffer, uint16_t len)
{
    while (len--)
    {
        ssd1306_intf.send(*buffer);
        buffer++;
    }
}

#if defined(SSD1306_BUILTIN_VGA_SUPPORT)
static inline void init_vga_crt_driver(uint8_t enable_jitter_fix)
{
    cli();
    if (enable_jitter_fix)
    {
        // Configure Timer 0 to calculate jitter fix
        TIMSK0=0;
        TCCR0A=0;
        TCCR0B=1;
        OCR0A=0;
        OCR0B=0;
        TCNT0=0;
    }

    // Timer 1 - vertical sync pulses
    pinMode (V_SYNC_PIN, OUTPUT);
    TCCR1A=(1<<WGM10) | (1<<WGM11) | (1<<COM1B1);
    TCCR1B=(1<<WGM12) | (1<<WGM13) | (1<<CS12) | (1<<CS10); //1024 prescaler
    OCR1A = 259;  // 16666 / 64 us = 260 (less one)
    OCR1B = 0;    // 64 / 64 us = 1 (less one)
    TIFR1 = (1<<TOV1);   // clear overflow flag
    TIMSK1 = (1<<TOIE1);  // interrupt on overflow on timer 1

    // Timer 2 - horizontal sync pulses
    pinMode (H_SYNC_PIN, OUTPUT);
    TCCR2A=(1<<WGM20) | (1<<WGM21) | (1<<COM2B1); //pin3=COM2B1
    TCCR2B=(1<<WGM22) | (1<<CS21); //8 prescaler
    OCR2A = 63;   // 32 / 0.5 us = 64 (less one)
    OCR2B = 7;    // 4 / 0.5 us = 8 (less one)
//    TIFR2 = (1<<TOV2);    // int on start of h-sync pulse
//    TIMSK2 = (1<<TOIE2);  // int on start of h-sync pulse
    TIFR2 = (1<<OCF2B);   // on end of h-sync pulse
    TIMSK2 = (1<<OCIE2B); // on end of h-sync pulse

  // Set up USART in SPI mode (MSPIM)

    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
    PORTC = 0;

   sei();
}

void ssd1306_vgaController_init_enable_output(void)
{
    ssd1306_vgaController_init_no_output();
    init_vga_crt_driver(1);
}

void ssd1306_vgaController_init_enable_output_no_jitter_fix(void)
{
    ssd1306_vgaController_init_no_output();
    init_vga_crt_driver(0);
//    set_sleep_mode (SLEEP_MODE_IDLE);
}
#endif

void ssd1306_vgaController_init_no_output(void)
{
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = vga_controller_init;
    ssd1306_intf.stop = vga_controller_stop;
    ssd1306_intf.send = vga_controller_send_byte4;
    ssd1306_intf.send_buffer = vga_controller_send_bytes;
    ssd1306_intf.close = vga_controller_close;
}

void ssd1306_debug_print_vga_buffer(void (*func)(uint8_t))
{
    for(int y = 0; y < ssd1306_lcd.height; y++)
    {
        for(int x = 0; x < ssd1306_lcd.width; x++)
        {
            uint8_t color = (s_vga_buffer[(y*ssd1306_lcd.width + x)/2] >> ((x&1)<<2)) & 0x0F;
            if (color)
            {
                func('#');
                func('#');
            }
            else
            {
                func(' ');
                func(' ');
            }
        }
        func('\n');
    }
    func('\n');
}
