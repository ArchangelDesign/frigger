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
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "rom.h"
#include "buzzer.h"

#define PIN_FRIDGE_DOOR 12
#define PIN_FREEZER_DOOR 2
#define PIN_TEST 10
#define PIN_RESET 7
#define RESET_TIME 10
#define ADDR_SECONDS 0
#define ADDR_MINUTES 1
#define ADDR_HOURS 2
#define ADDR_HOURS_MULTIPLIER 3
// Main alarm trigger
#define MAX_OPEN_TIME 90 // 45 seconds
// no sound period
#define QUIET_TIME 30 // 15 seconds
// quiet alarm trigger
#define QUIET_ALARM_TIME 60 // 30 seconds
// turn off main alarm
#define MAX_ALARM_TIME 140 // 70 seconds

bool fridge_door_open = false;
bool freezer_door_open = false;
bool is_in_testing = false;
bool is_in_reset = false;
bool reset_performed = false;
bool pre_alarm_active = false;
bool loud_alarm_active = false;
uint8_t reset_button_time = 0;
uint8_t total_open_hours = 0;
uint8_t total_open_minutes = 0;
uint8_t total_open_seconds = 0;
uint8_t total_open_hours_multiplier = 1;
uint16_t current_open_time = 0;

void fg_load_counters()
{
    total_open_hours = rom_read(ADDR_HOURS);
    total_open_minutes = rom_read(ADDR_MINUTES);
    total_open_seconds = rom_read(ADDR_SECONDS);
    total_open_hours_multiplier = rom_read(ADDR_HOURS_MULTIPLIER);
}

void fg_reset_counters()
{
    rom_store(ADDR_HOURS, 0);
    rom_store(ADDR_MINUTES, 0);
    rom_store(ADDR_SECONDS, 0);
    rom_store(ADDR_HOURS_MULTIPLIER, 0);
    total_open_hours_multiplier = 0;
    total_open_hours = 0;
    total_open_minutes = 0;
    total_open_seconds = 0;
}

void fg_init_controller() {
    pinMode(PIN_FRIDGE_DOOR, INPUT_PULLUP);
    pinMode(PIN_FREEZER_DOOR, INPUT_PULLUP);
    pinMode(PIN_TEST, INPUT_PULLUP);
    pinMode(PIN_RESET, INPUT_PULLUP);
    fg_load_counters();
}

uint16_t fg_get_open_hours()
{
    return total_open_hours + (256 * total_open_hours_multiplier);
}

/**
 * Invoked every 500ms
 */
bool fg_controller_callback(void *) {
    is_in_reset = digitalRead(PIN_RESET) == LOW;
    fridge_door_open = digitalRead(PIN_FRIDGE_DOOR) == HIGH;
    freezer_door_open = digitalRead(PIN_FREEZER_DOOR) == HIGH;
    is_in_testing = digitalRead(PIN_TEST) == LOW;
    if (is_in_reset) {
        reset_button_time++;
        if (reset_button_time > RESET_TIME) {
            fg_reset_counters();
            reset_performed = true;
            buzzer_beep(7);
        }
    } else {
        reset_button_time = 0;
        reset_performed = false;
    }
    
    if (!fridge_door_open && !freezer_door_open) {
        if (current_open_time > 0) {
            Serial.println("CLOSE");
            buzzer_short_beeps(3);
        }
        pre_alarm_active = false;
        loud_alarm_active = false;
        current_open_time = 0;
        return true;
    }
    
    if (current_open_time < 1) {
        buzzer_short_beeps(5);
        Serial.println("OPEN");
    }
    if (current_open_time > QUIET_TIME) {
        pre_alarm_active = true;
    }
    total_open_seconds++;
    current_open_time++;
    if (total_open_seconds > 120) {
        total_open_seconds = 0;
        total_open_minutes++;
    }
    if (total_open_minutes > 59) {
        total_open_hours++;
        total_open_minutes = 0;
    }
    rom_store(ADDR_HOURS, total_open_hours);
    rom_store(ADDR_MINUTES, total_open_minutes);
    rom_store(ADDR_SECONDS, total_open_seconds);

    if (current_open_time > MAX_OPEN_TIME && current_open_time < MAX_ALARM_TIME) {
        loud_alarm_active = true;
    }
    
    return true;
}

bool fg_alarm_timer(void*)
{
    if (loud_alarm_active) {
        buzzer_short_beeps(25);
        buzzer_beep(1);
    }
    if (pre_alarm_active) {
        buzzer_short_beeps(5);
    }
    return true;
}

#endif