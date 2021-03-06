/*
    ** Frigger - The fridge monitor **
    copyright (c) 2021 Raff Martinez-Marjanski.
    All rights reserved.
    Author: Raff Martinez-Marjanski <archangel.raffael@gmail.com>

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "flash.h"
#include "controller.h"
#include "buzzer.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET_PIN 4
#define SCREEN_ADDRESS 0x3C

const char f_frigger[] PROGMEM = "FRIGGER";
const char f_fridge_door_open[] PROGMEM = "fridge door open";
const char f_freezer_door_open[] PROGMEM = "freezer door open";
const char f_both_door_open[] PROGMEM = "both doors open";
const char f_done[] PROGMEM = "DONE";
const unsigned char PROGMEM bmp_dot[] = {
    0b00011000,
    0b00111100,
    0b01111110,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00011000
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

void fg_init_display() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
}

void print_home_screen()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(23, 0);
  display.setTextColor(SSD1306_WHITE);
  display.println(flash_get(f_frigger));
  display.drawLine(0, 20, 128, 20, SSD1306_WHITE);
  display.drawLine(0, 21, 128, 21, SSD1306_WHITE);
  display.drawBitmap(0, 3, bmp_dot, 8, 7, SSD1306_WHITE);
  display.drawBitmap(120, 3, bmp_dot, 8, 7, SSD1306_WHITE);
}

void print_to_screen(int x, int y, char *text)
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.print(text);
}

void print_to_screen_size(int x, int y, int size, char *text)
{
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.print(text);
}

void print_reset_screen()
{
    char buf[30];
    if (reset_performed) {
        sprintf(buf, "%s", flash_get(f_done));
        print_to_screen_size(10, 0, 2, buf);
        return;
    }
    sprintf(buf, "%s", "RESET");
    print_to_screen_size(10, 0, 2, buf);
    sprintf(buf, "%d", reset_button_time);
    print_to_screen_size(20, 20, 3, buf);

}

bool fg_refresh_screen(void *) {
    display.clearDisplay();

    if (is_in_reset) {
        print_reset_screen();
        display.display();
        return true;
    }

    char buf[30];
    
    if (fridge_door_open || freezer_door_open) {
        if (is_in_testing)
            buzzer_1_beep();
        if (fridge_door_open && !freezer_door_open)
            sprintf(buf, flash_get(f_fridge_door_open));
        else if (freezer_door_open && !fridge_door_open)
            sprintf(buf, flash_get(f_freezer_door_open));
        else if (freezer_door_open && fridge_door_open)
            sprintf(buf, flash_get(f_both_door_open));
        print_to_screen(0, 0, buf);
        sprintf(buf, "%d", current_open_time / 2);
        int x = current_open_time / 2 > 9 ? 20 : 40;
        print_to_screen_size(x, 20, 5, buf);
        sprintf(buf, "%s", pre_alarm_active ? "PA" : ".");
        print_to_screen_size(0, 30, 2, buf);
        display.display();
        return true;
    }
    print_home_screen();
    sprintf(buf, "%d:%d:%d", fg_get_open_hours(), total_open_minutes, total_open_seconds / 2);
    print_to_screen_size(0, 35, 2, buf);
    display.display();
    return true;
}

#endif