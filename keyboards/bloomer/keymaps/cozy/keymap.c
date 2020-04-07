/* Copyright 2019 Paul Ewing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "click_repeater.h"

#include <stdlib.h>

// Layers

enum bloomer_layers {
  DEFAULT,
  FN,
  OSRS
};

#define TO_OSRS TO(OSRS)
#define TO_DEF  TO(DEFAULT)


enum my_keycodes {
  FOO = SAFE_RANGE,
  SH_TOG,
  SH_BTN1,
  RP_BTN1
};

#define RGB_N RGB_MOD  // Rotate to next RGB mode
#define RGB_P RGB_RMOD // Rotate to next RGB mode

//#define SH_LCLK LSFT(KC_BTN1)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[DEFAULT] = LAYOUT(
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_PSCR, KC_SLCK, KC_PAUS, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
    KC_EQL,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_INS,  KC_HOME, KC_PGUP, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_CAPS, KC_END,  KC_PGDN, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
    KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LEFT, KC_UP,   KC_RGHT, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    KC_LCTL, KC_LGUI, KC_LALT, KC_GRV,  MO(FN),  KC_BSPC, KC_DEL,  KC_DOWN, KC_ENT,  KC_SPC,  KC_LBRC, KC_RBRC, KC_RALT, KC_RGUI, KC_RCTL
),
[FN] = LAYOUT(
    TO_OSRS, _______, _______, _______, _______, _______, RGB_TOG, RGB_M_P, RGB_M_B, _______, _______, _______, _______, _______, RESET,
    _______, _______, _______, _______, _______, _______, RGB_HUI, RGB_SAI, RGB_VAI, _______, _______, _______, _______, _______, _______,
    _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, RGB_HUD, RGB_SAD, RGB_VAD, _______, KC_GRV,  KC_LBRC, KC_RBRC, _______, _______,
    _______, KC_F5,   KC_F6,   KC_F7,   KC_F8,   _______,                            KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   _______, RGB_N,   _______, RGB_P,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),
[OSRS] = LAYOUT(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, RP_BTN1, SH_TOG,  _______, _______, _______, _______, _______, _______, _______, _______, _______,
    TO_DEF,  _______, _______, KC_BTN1, SH_BTN1, _______,                            _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_1,    KC_6,    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, KC_BTN1, _______, _______, _______, _______, _______, _______, _______, _______, _______
)
};

static bool shift_enabled = false;

static struct click_repeater_t click_repeater = {
    .state = CR_DISABLED,
    .previous_button_down = 0,
    .previous_button_up = 0,
    .click_duration = 0,
    .click_duration_min = 20,
    .click_duration_max = 50,
    .wait_duration = 0,
    .wait_duration_min = 90,
    .wait_duration_max = 140,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case SH_TOG:
      if (record->event.pressed) {
          if (shift_enabled)
              unregister_code(KC_LSFT);
          else
              register_code(KC_LSFT);
          shift_enabled = !shift_enabled;
      }
      return false; // Skip all further processing of this key
    case SH_BTN1:
      if (record->event.pressed) {
          register_code(KC_LSFT);
          register_code(KC_BTN1);
      } else {
          unregister_code(KC_BTN1);
          unregister_code(KC_LSFT);
      }
      return false;
    case RP_BTN1:
      if (record->event.pressed)
          cr_enable(&click_repeater);
      else
          cr_disable(&click_repeater);
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}

void matrix_scan_user(void) {
    static bool rng_seeded = false;
    if (!rng_seeded) {
        srand(timer_read32());
        rng_seeded = true;
    }

    cr_poll(&click_repeater);
}
