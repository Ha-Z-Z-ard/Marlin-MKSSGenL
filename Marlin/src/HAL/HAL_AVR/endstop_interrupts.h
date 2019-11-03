/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * Endstop Interrupts
 *
 * Without endstop interrupts the endstop pins must be polled continually in
 * the temperature-ISR via endstops.update(), most of the time finding no change.
 * With this feature endstops.update() is called only when we know that at
 * least one endstop has changed state, saving valuable CPU cycles.
 *
 * This feature only works when all used endstop pins can generate either an
 * 'external interrupt' or a 'pin change interrupt'.
 *
 * Test whether pins issue interrupts on your board by flashing 'pin_interrupt_test.ino'.
 * (Located in Marlin/buildroot/share/pin_interrupt_test/pin_interrupt_test.ino)
 */

#include "../../module/endstops.h"

#include <stdint.h>

// One ISR for all EXT-Interrupts
void endstop_ISR() { endstops.update(); }

/**
 * Patch for pins_arduino.h (...\Arduino\hardware\arduino\avr\variants\mega\pins_arduino.h)
 *
 * These macros for the Arduino MEGA do not include the two connected pins on Port J (D14, D15).
 * So we extend them here because these are the normal pins for Y_MIN and Y_MAX on RAMPS.
 * There are more PCI-enabled processor pins on Port J, but they are not connected to Arduino MEGA.
 */
#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
  #define digitalPinHasPCICR(p)   (WITHIN(p, 10, 15) || WITHIN(p, 50, 53) || WITHIN(p, 62, 69))
  #define moreDigitalPinToPCICR(p)    digitalPinToPCICR(WITHIN(p, 14, 15) ? 10 : p)
  #define moreDigitalPinToPCICRbit(p) (WITHIN(p, 14, 15) ? 1 : digitalPinToPCICRbit(p))
  #define moreDigitalPinToPCMSK(p)    (WITHIN(p, 14, 15) ? (&PCMSK1) : digitalPinToPCMSK(p))
  #define moreDigitalPinToPCMSKbit(p) digitalPinToPCMSKbit(WITHIN(p, 14, 15) ? (p)+36 : p)
#endif


// Install Pin change interrupt for a pin. Can be called multiple times.
void pciSetup(const int8_t pin) {
  if (moreDigitalPinToPCMSK(pin) != nullptr) {
    SBI(*moreDigitalPinToPCMSK(pin), moreDigitalPinToPCMSKbit(pin));  // enable pin
    SBI(PCIFR, moreDigitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    SBI(PCICR, moreDigitalPinToPCICRbit(pin)); // enable interrupt for the group
  }
}

// Handlers for pin change interrupts
#ifdef PCINT0_vect
  ISR(PCINT0_vect) { endstop_ISR(); }
#endif

#ifdef PCINT1_vect
  ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#ifdef PCINT2_vect
  ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#ifdef PCINT3_vect
  ISR(PCINT3_vect, ISR_ALIASOF(PCINT0_vect));
#endif

void setup_endstop_interrupts() {
  #define _ATTACH(P) attachInterrupt(digitalPinToInterrupt(P), endstop_ISR, CHANGE)
  #if HAS_X_MAX
    #if (digitalPinToInterrupt(X_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(X_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(X_MAX_PIN), "X_MAX_PIN is not interrupt-capable");
      pciSetup(X_MAX_PIN);
    #endif
  #endif
  #if HAS_X_MIN
    #if (digitalPinToInterrupt(X_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(X_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(X_MIN_PIN), "X_MIN_PIN is not interrupt-capable");
      pciSetup(X_MIN_PIN);
    #endif
  #endif
  #if HAS_Y_MAX
    #if (digitalPinToInterrupt(Y_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Y_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(Y_MAX_PIN), "Y_MAX_PIN is not interrupt-capable");
      pciSetup(Y_MAX_PIN);
    #endif
  #endif
  #if HAS_Y_MIN
    #if (digitalPinToInterrupt(Y_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Y_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(Y_MIN_PIN), "Y_MIN_PIN is not interrupt-capable");
      pciSetup(Y_MIN_PIN);
    #endif
  #endif
  #if HAS_Z_MAX
    #if (digitalPinToInterrupt(Z_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z_MAX_PIN), "Z_MAX_PIN is not interrupt-capable");
      pciSetup(Z_MAX_PIN);
    #endif
  #endif
  #if HAS_Z_MIN
    #if (digitalPinToInterrupt(Z_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z_MIN_PIN), "Z_MIN_PIN is not interrupt-capable");
      pciSetup(Z_MIN_PIN);
    #endif
  #endif
  #if HAS_X2_MAX
    #if (digitalPinToInterrupt(X2_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(X2_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(X2_MAX_PIN), "X2_MAX_PIN is not interrupt-capable");
      pciSetup(X2_MAX_PIN);
    #endif
  #endif
  #if HAS_X2_MIN
    #if (digitalPinToInterrupt(X2_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(X2_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(X2_MIN_PIN), "X2_MIN_PIN is not interrupt-capable");
      pciSetup(X2_MIN_PIN);
    #endif
  #endif
  #if HAS_Y2_MAX
    #if (digitalPinToInterrupt(Y2_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Y2_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(Y2_MAX_PIN), "Y2_MAX_PIN is not interrupt-capable");
      pciSetup(Y2_MAX_PIN);
    #endif
  #endif
  #if HAS_Y2_MIN
    #if (digitalPinToInterrupt(Y2_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Y2_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(Y2_MIN_PIN), "Y2_MIN_PIN is not interrupt-capable");
      pciSetup(Y2_MIN_PIN);
    #endif
  #endif
  #if HAS_Z2_MAX
    #if (digitalPinToInterrupt(Z2_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z2_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z2_MAX_PIN), "Z2_MAX_PIN is not interrupt-capable");
      pciSetup(Z2_MAX_PIN);
    #endif
  #endif
  #if HAS_Z2_MIN
    #if (digitalPinToInterrupt(Z2_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z2_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z2_MIN_PIN), "Z2_MIN_PIN is not interrupt-capable");
      pciSetup(Z2_MIN_PIN);
    #endif
  #endif
  #if HAS_Z3_MAX
    #if (digitalPinToInterrupt(Z3_MAX_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z3_MAX_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z3_MAX_PIN), "Z3_MAX_PIN is not interrupt-capable");
      pciSetup(Z3_MAX_PIN);
    #endif
  #endif
  #if HAS_Z3_MIN
    #if (digitalPinToInterrupt(Z3_MIN_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z3_MIN_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z3_MIN_PIN), "Z3_MIN_PIN is not interrupt-capable");
      pciSetup(Z3_MIN_PIN);
    #endif
  #endif
  #if HAS_Z_MIN_PROBE_PIN
    #if (digitalPinToInterrupt(Z_MIN_PROBE_PIN) != NOT_AN_INTERRUPT)
      _ATTACH(Z_MIN_PROBE_PIN);
    #else
      static_assert(digitalPinHasPCICR(Z_MIN_PROBE_PIN), "Z_MIN_PROBE_PIN is not interrupt-capable");
      pciSetup(Z_MIN_PROBE_PIN);
    #endif
  #endif

  // If we arrive here without raising an assertion, each pin has either an EXT-interrupt or a PCI.
}
