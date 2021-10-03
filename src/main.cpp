/*
    ** Frigger - The fridge monitor **
    copyright (c) 2021 Raff Martinez-Marjanski.
    All rights reserved.
    Author: Raff Martinez-Marjanski <archangel.raffael@gmail.com>

    Frigger monitors opening/closing of the fridge (and freezer),
    sounds alarm if the door has been open for too long and summarizes
    the total open time storing it in EPROM (up to 256 hours).
    Connect pin `PIN_TEST` to the ground to put it in test mode
    which will continou to beep when either door is open.

    Hardware:
    - Arduino (any)
    - Active buzzer
    - 2 magnetic switches (or any NC contact switches)
    - 1306 OLED Display

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
#include <Arduino.h>
#include <arduino-timer.h>

#include "display.h"
#include "buzzer.h"
#include "controller.h"

Timer<10, millis> system_timer;

void setup() {
  Serial.begin(9600);
  Serial.println("starting...");
  init_display();
  print_home_screen();
  fg_setup_buzzer();
  fg_init_controller();
  system_timer.every(500, fg_controller_callback);
  system_timer.every(321, fg_refresh_screen);
}

void loop() {
  system_timer.tick();
}