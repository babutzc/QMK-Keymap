/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
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
#include <stdio.h>

bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;

uint16_t copy_paste_timer;
char wpm_str[10];

typedef union {
    uint32_t raw;
    struct {
        bool osIsWindows;
    };
} user_config_t;

user_config_t user_config;

enum custom_keycodes {
    Qwerty = SAFE_RANGE,
    Colemak,
    Undo,
    Cut,
    Copy,
    Paste,
    NxtWord,
    PrvWord
};

enum layers {
    COLEMAK = 0,
    QWERTY,
    NUMPAD,
    SYMBOLS,
    NAV,
    MEDIA,
    MOUSE
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: Colemak
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   ESC  |   Q  |   W  |   F  |   P  |   G  |                              |   J  |   L  |   U  |   Y  | ;  : |  | \   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   TAB |   A  |   R  |  S   |   T  |   D  |                               |   H  |   N  |   E  |   I  |   O  | ENTER  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |   LCTL |   Z  |   X  |   C  |   V  |   B  |Enter | ESC  |  |   "  |  -_  |   K  |   M  | ,  < | . >  | /  ? |  Del   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Play | GUI  | RALT | SPC  | LSFT |  | NUM  | SYMB | DEL  | MEDIA|LEADER|
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [COLEMAK] = LAYOUT(
      KC_ESC               , KC_Q,  KC_W   , KC_F  ,   KC_P ,   KC_G ,                                        KC_J,   KC_L ,  KC_U ,   KC_Y ,KC_SCLN, KC_BSLS,
      KC_LSFT              , KC_A,  KC_R   , KC_S  ,   KC_T ,   KC_D ,                                        KC_H,   KC_N ,  KC_E ,   KC_I ,  KC_O , KC_ENT,
      MT(MOD_LCTL,KC_TILD) , KC_Z,  KC_X   , KC_C  ,   KC_V ,   KC_B , Copy, Paste   , KC_QUOT, KC_MINS, KC_K,  KC_M ,KC_COMM, KC_DOT ,KC_SLSH, RSFT_T(KC_CAPS),
                                    KC_MPLY, KC_LGUI, KC_LOPT, KC_SPC ,LT(NUMPAD, KC_TAB) ,MO(MOUSE), LT(NAV, KC_BSPC), MO(SYMBOLS), MO(MEDIA), KC_LEAD
    ),

/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   ESC  |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |  | \   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   TAB  |   A  |   S  |  D   |   F  |   G  |                              |   H  |   J  |   K  |   L  | ;  : |  ENTER |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |Enter |  ESC |  |  "   |  -_  |   N  |   M  | ,  < | . >  | /  ? |  Del   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        | Play | GUI  | RALT | SPC  | LSFT |  | NUM  | SYMB | DEL  | MEDIA|LEADER|
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [QWERTY] = LAYOUT(
      KC_ESC               , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_BSLS,
      KC_LSFT              , KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                        KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN, KC_ENT,
      MT(MOD_LCTL,KC_TILD) , KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , XXXXXXX, Paste , KC_QUOT, KC_MINS, KC_N,  KC_M ,KC_COMM, KC_DOT ,KC_SLSH, RSFT_T(KC_CAPS),
                                 KC_MPLY, KC_LGUI, KC_LOPT, KC_SPC ,LT(NUMPAD, KC_TAB) ,MO(MOUSE), LT(NAV, KC_BSPC), MO(SYMBOLS), MO(MEDIA), KC_LEAD
    ),


/*
 * Num Layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  1 ! |  2 @ |  3 # |  4 $ |  5 % |                              |  /   |  7 & |  8 * |  9 ( |  -   |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  6 ^ |  7 & |  8 * |  9 ( |  0 ) |                              |  *   |  4 $ |  5 % |  6 ^ |  +   | Space  |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |      |      |      |      |      |  |      |      |  0 ) |  1 ! |  2 @ |  3 # |  =   | Enter  |
 * `----------------------+------+------+------+      +------|  |------+      +------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |  ,   |  .   |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [NUMPAD] = LAYOUT(
      _______,  KC_1  ,  KC_2  ,  KC_3  ,  KC_4  ,  KC_5  ,                                     KC_PSLS,  KC_7  ,  KC_8  ,  KC_9  , KC_PMNS, _______,
      _______,  KC_6  ,  KC_7  ,  KC_8  ,  KC_9  ,  KC_0  ,                                     KC_PAST,  KC_4  ,  KC_5  ,  KC_6  , KC_PLUS, KC_SPC ,
      _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______  ,  KC_1  ,  KC_2  ,  KC_3  , KC_PEQL, KC_ENT ,
                                 _______, _______, _______, _______, KC_0, KC_PCMM, KC_DOT, _______, _______,  _______
    ),

/*
 * Symbol Layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  ` ~   |   !  |  @   |  #   |  $   |  %   |                              |  ^   |  {   |  }   |      |  €   |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   ~    |      |      | - _  | = +  |  &   |                              |  *   |  (   |  )   |      |  |   |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |  _   |  +   |      |      |      |  |      |      |      | [ {  | ] }  |      | \ |  |        |
 * `----------------------+------+------+------+      +------|  |------+      +------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [SYMBOLS] = LAYOUT(
       KC_TILD, KC_EXLM,  KC_AT , KC_HASH, KC_DLR , KC_PERC,                                     KC_CIRC, KC_LCBR, KC_RCBR, _______,ALGR(KC_5),_______,
      _______, _______, KC_QUOT, KC_MINS, KC_EQL , KC_AMPR,                                     KC_ASTR, KC_LPRN, KC_RPRN, _______, KC_PIPE, _______,
      _______, _______, _______, KC_UNDS, KC_PLUS, _______, XXXXXXX, _______, _______, XXXXXXX, _______, KC_LBRC, KC_RBRC, _______, KC_BSLS, _______,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
 
/*
 * Navigation Layer: Cursor, Text Navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |WheelU|      |      |                              |      |PrvWord| Up |NxtWord|      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |       | LCTL |WheelD|LSFT | Bksp |                              | Bksp | Left | Down |Right | Del  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        | Undo | Cut  | Copy |Paste |      |      |      |  |      |      |      | Home |      | End  |      |        |
 * `----------------------+------+------+------+      +------|  |------+      +------+------+------+----------------------'
 *                        |      |      |      |      |Space |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [NAV] = LAYOUT(
      _______, _______, _______, KC_WH_U, _______, _______,                                     _______, PrvWord, KC_UP  , NxtWord, _______, _______,
      _______, _______, KC_MS_BTN1, KC_WH_D, KC_MS_BTN2, KC_BSPC,                                     KC_BSPC, KC_LEFT, KC_DOWN,KC_RIGHT, KC_DEL , _______,
      _______,  Undo  ,   Cut  ,  Copy  , Paste  , _______, XXXXXXX, _______, _______, XXXXXXX, _______, KC_HOME, _______, KC_END , _______, _______,
                                 _______, _______, _______, _______, KC_SPC , _______, _______, _______, _______, _______
    ),

/*
 * Mouse Layer: Cursor, Text Navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |WheelU|      |      |                              |      |PrvWord| Up |NxtWord|      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |       | LCTL |WheelD|LSFT | Bksp |                              | Bksp | Left | Down |Right | Del  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        | Undo | Cut  | Copy |Paste |      |      |      |  |      |      |      | Home |      | End  |      |        |
 * `----------------------+------+------+------+      +------|  |------+      +------+------+------+----------------------'
 *                        |      |      |      |      |Space |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [MOUSE] = LAYOUT(
      _______, _______, _______, KC_WH_U, _______, _______,                                     _______, PrvWord, KC_MS_UP  , NxtWord, _______, _______,
      _______, _______, KC_MS_BTN1, KC_WH_D, KC_MS_BTN2, KC_BSPC,                                     KC_BSPC, KC_MS_L, KC_MS_D,KC_MS_R, KC_DEL , _______,
      _______,  Undo  ,   Cut  ,  Copy  , Paste  , _______, XXXXXXX, _______, _______, XXXXXXX, _______, KC_HOME, _______, KC_END , _______, _______,
                                 _______, _______, _______, _______, KC_SPC , _______, _______, _______, _______, _______
    ),    

/*
 *
 * Adjust Layer: Media
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |  F9  | F10  | F11  | F12  |      |                              | SAI  |      | Vol+ |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  F5  |  F6  |  F7  |  F8  |      |                              | HUI  | Prev | Vol- |  Nxt |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |  F1  |  F2  |  F3  |  F4  |      |      |      |  |      |      | VAI  | Mute | Play |      |      |        |
 * `----------------------+------+------+------+      +------|  |------+      +------+------+------+----------------------'
 *                       |Qwerty|Colemak|      |      |      |  | RGB  |      | RGB  |      |      |
 *                        | Dflt | Dflt |      |      |      |  |Toggle|      | Mode |      |      |
 *                        `----------------------------------'  `----------------------------------'
 */
    [MEDIA] = LAYOUT(
      _______,  KC_F9 , KC_F10 , KC_F11 , KC_F12 , _______,                                     RGB_SAI, _______, KC_VOLU, _______, _______, _______,
      _______,  KC_F5 ,  KC_F6 ,  KC_F7 ,  KC_F8 , _______,                                     RGB_HUI, KC_MPRV, KC_VOLD, KC_MNXT, _______, _______,
      _______,  KC_F1 ,  KC_F2 ,  KC_F3 ,  KC_F4 , _______, XXXXXXX, _______, _______, XXXXXXX, RGB_VAI, KC_MUTE, KC_MPLY, _______, _______, _______,
                                 Qwerty , Colemak, _______, _______, _______, RGB_TOG, _______, RGB_MOD, _______, _______
    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case Qwerty:
            if (record->event.pressed) {
                set_single_persistent_default_layer(QWERTY);
            }
            break;
        case Colemak:
            if (record->event.pressed) {
                set_single_persistent_default_layer(COLEMAK);
            }
            break;
        case Undo:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_Z));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(G(KC_Z));
                }
            }
            break;
        case Cut:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_X));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(G(KC_X));
                }
            }
            break;
        case Copy:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_C));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(G(KC_C));
                }
            }
            break;
        case Paste:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_V));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(G(KC_V));
                }
            }
            break;
        case PrvWord:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_LEFT));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(A(KC_LEFT));
                }
            }
            break;
        case NxtWord:
            if (record->event.pressed) {
                if (user_config.osIsWindows == 1) {
                    tap_code16(C(KC_RGHT));
                } else if (user_config.osIsWindows == 0) {
                    tap_code16(A(KC_RGHT));
                }
            }
            break;
    }
    return true;
};


LEADER_EXTERNS();


void matrix_scan_user(void) {
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 1250) {
      unregister_code(KC_LALT);
      is_alt_tab_active = false;
    }
  }

    LEADER_DICTIONARY() {
    leading = false;
        leader_end();

        // Set current OS indicator to macOs
        SEQ_ONE_KEY(KC_M) {
            user_config.osIsWindows = false;
            eeconfig_update_user(user_config.raw);
        }

        // Set current OS indicator to Windows
        SEQ_ONE_KEY(KC_W) {
            user_config.osIsWindows = true;
            eeconfig_update_user(user_config.raw);
        }

        // Screenshot
        SEQ_ONE_KEY(KC_S) {
            if (user_config.osIsWindows == 1) {
                tap_code16(S(G(KC_S)));
            } else if (user_config.osIsWindows == 0) {
                tap_code16(S(G(KC_4)));
            }
        }

        // Video
        SEQ_ONE_KEY(KC_V) {
            if (user_config.osIsWindows == 0) {
                tap_code16(S(G(KC_5)));
            }
        }

        // Sleep
        SEQ_ONE_KEY(KC_P) {
            if (user_config.osIsWindows == 1) {
                SEND_STRING(SS_LGUI("x") "u" "h");
            } else if (user_config.osIsWindows == 0) {
                tap_code16(A(G(KC_PWR)));
            }
        }
    }
}


static void render_logo(void) {
  static const char PROGMEM my_logo[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x80, 0xc0, 0x40, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0xf0, 0xe0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0x00, 0x60, 0x40, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xd9, 0xc0, 0x50, 0x13, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc7, 0xfd, 0xe1, 0x71, 0xf1, 0xff, 0xff, 0xff, 0x7f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xe2, 0x00, 0x00, 
0x00, 0x00, 0xff, 0x12, 0x82, 0xb2, 0x86, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x3c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7e, 0xfe, 
0xfd, 0xf9, 0xf3, 0xf7, 0xdf, 0xbf, 0xdf, 0xfb, 0xfd, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0x3c, 0x00, 
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0xe0, 0xe0, 0xe0, 0xf0, 0x70, 0x70, 
0x70, 0x78, 0x78, 0x38, 0x38, 0x3c, 0x3d, 0x3f, 0x7f, 0x7f, 0xcf, 0xcf, 0x18, 0x08, 0x00, 0x00, 
0x21, 0x61, 0x61, 0x65, 0x61, 0x66, 0x61, 0x37, 0xb1, 0xc1, 0x60, 0x66, 0x20, 0x04, 0x04, 0x00, 
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0c, 0x06, 0x07, 0x07, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x24, 0x3c, 0x04, 0x08, 0x01, 0x01, 0x47, 0x0e, 
0xfc, 0xf4, 0xe0, 0xf8, 0x1e, 0x06, 0x01, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xf0, 0x7c, 0xfc, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3b, 0x07, 0x00, 0x80, 0xc0, 0xf4, 
0xff, 0xff, 0xff, 0xff, 0xe4, 0xc8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xf0, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
0xe0, 0x80, 0x01, 0x18, 0x00, 0x00, 0x00, 0x82, 0xc0, 0x81, 0x80, 0x48, 0xff, 0xff, 0xff, 0xff, 
0x6f, 0x6f, 0xef, 0xef, 0x07, 0x07, 0x1b, 0xff, 0xff, 0xfe, 0xfe, 0x7e, 0xfc, 0xf8, 0x30, 0xf0, 
0xe0, 0xe1, 0xc3, 0x83, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

  };

  oled_write_raw_P(my_logo, sizeof(my_logo));
}


#ifdef OLED_DRIVER_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
	return OLED_ROTATION_180;
}

static void render_status(void) {
    // QMK Logo and version information
    oled_write_P(PSTR("       0xDeadbeef\n\n"), false);


    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case COLEMAK:
            oled_write_P(PSTR("Colemak\n"), false);
            break;
        case QWERTY:
            oled_write_P(PSTR("Qwerty\n"), false);
            break;        
        case NUMPAD:
            oled_write_P(PSTR("Numpad\n"), false);
            break;
        case SYMBOLS:
            oled_write_P(PSTR("Symbols\n"), false);
            break;
        case NAV:
            oled_write_P(PSTR("Navigation\n"), false);
            break;
        case MEDIA:
            oled_write_P(PSTR("F Keys & Media\n"), false);
            break;
        case MOUSE:
            oled_write_P(PSTR("Mouse\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    // Host Keyboard LED Status
    uint8_t led_usb_state = host_keyboard_leds();
    oled_write_P(IS_LED_ON(led_usb_state, USB_LED_CAPS_LOCK)   ? PSTR("CAPLCK ") : PSTR("       "), false);
}


void oled_task_user(void) {
    if (is_keyboard_master()) {
        render_status();
     // Renders the current keyboard state (layer, lock, caps, scroll, etc)
    } else {
        render_logo();
    }
}

#endif

#ifdef ENCODER_ENABLE
void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLD);
        } else {
            tap_code(KC_VOLU);
        }
        
    } else if (index == 1) {
        if (clockwise) {
          if (!is_alt_tab_active) {
            is_alt_tab_active = true;
            register_code(KC_LALT);
          }
          alt_tab_timer = timer_read();
          tap_code16(KC_TAB);
        } else {
          alt_tab_timer = timer_read();
          tap_code16(S(KC_TAB));
        }
    }
}

#endif
