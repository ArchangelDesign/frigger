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
#define PIN_FREEZER_DOOR 11
#define PIN_TEST 10
#define ADDR_SECONDS 0
#define ADDR_MINUTES 1
#define ADDR_HOURS 2
#define MAX_OPEN_TIME 60

bool fridge_door_open = false;
bool freezer_door_open = false;
bool is_in_testing = false;
uint8_t total_open_hours = 0;
uint8_t total_open_minutes = 0;
uint8_t total_open_seconds = 0;
uint16_t current_open_time = 0;

void fg_init_controller() {
    pinMode(PIN_FRIDGE_DOOR, INPUT_PULLUP);
    pinMode(PIN_FREEZER_DOOR, INPUT_PULLUP);
    pinMode(PIN_TEST, INPUT_PULLUP);
    // rom_store(ADDR_HOURS, 0);
    // rom_store(ADDR_MINUTES, 0);
    // rom_store(ADDR_SECONDS, 0);
    total_open_hours = rom_read(ADDR_HOURS);
    total_open_minutes = rom_read(ADDR_MINUTES);
    total_open_seconds = rom_read(ADDR_SECONDS);
}

/**
 * Invoked every 500ms
 */
bool fg_controller_callback(void *) {
    fridge_door_open = digitalRead(PIN_FRIDGE_DOOR) == HIGH;
    freezer_door_open = digitalRead(PIN_FREEZER_DOOR) == HIGH;
    is_in_testing = digitalRead(PIN_TEST) == LOW;
    if (!fridge_door_open && !freezer_door_open) {
        current_open_time = 0;
        return true;
    }
    if (fridge_door_open || freezer_door_open) {
        if (current_open_time < 1)
            buzzer_1_beep();
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

        if (current_open_time > MAX_OPEN_TIME) {
            buzzer_beep(3);
        }
    }
    
    return true;
}

#endif