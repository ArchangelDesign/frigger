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
const char f_dashes[] PROGMEM = "--------------------";
const char f_fridge_door_open[] PROGMEM = "fridge door open";
const char f_freezer_door_open[] PROGMEM = "freezer door open";
const char f_both_door_open[] PROGMEM = "both doors open";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

void init_display() {
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
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);
  display.println(flash_get(f_frigger));
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(flash_get(f_dashes));
}

void print_to_screen(int x, int y, char *text)
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.print(text);
}

bool fg_refresh_screen(void *) {
    display.clearDisplay();
    print_home_screen();
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
    }
    print_to_screen(0, 25, buf);
    char t[] = "H: %d M: %d S: %d";
    
    sprintf(buf, t, total_open_hours, total_open_minutes, total_open_seconds / 2);
    print_to_screen(0, 35, buf);
    sprintf(buf, "CURRENT: %d", current_open_time);
    print_to_screen(0, 45, buf);
    display.display();
    return true;
}

#endif