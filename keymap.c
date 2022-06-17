#include QMK_KEYBOARD_H
#include "version.h"

enum {
	RGB_SLD = EZ_SAFE_RANGE,
	MACRO_1,
	DISCO_TIME
};

enum {
	TAP_1, TAP_2, TAP_3, TAP_4
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_ergodox_pretty(
		KC_TRNS,   KC_1, KC_2, KC_3, KC_4, KC_5, KC_TRNS,
		KC_TRNS,   KC_6, KC_7, KC_8, KC_9, KC_0, KC_TRNS,

		KC_GRAVE,  KC_Q, KC_W, KC_E, KC_R, KC_T, KC_EQUAL,
		KC_MINUS,  KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLASH,

		KC_TAB,    KC_A, KC_S, KC_D, KC_F, KC_G,
		           KC_H, KC_J, KC_K, KC_L, KC_SCOLON, KC_QUOTE,

		KC_LSHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_LBRC,
		KC_RBRC,   KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, KC_RSHIFT,

		KC_LCTRL, KC_RALT, KC_TRNS, KC_CAPS, KC_LGUI,
		KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, KC_RCTRL,

		TD(TAP_1), TD(TAP_2), TD(TAP_3), TD(TAP_4),
		KC_TRNS, KC_TRNS,
		KC_SPACE, KC_ESCAPE, KC_UNDS,
		MACRO_1, KC_BSPACE, KC_ENTER
	)
};

rgblight_config_t rgbset;
int disco_enabled = 1;

bool
process_record_user(uint16_t keycode, keyrecord_t *record)
{
	switch (keycode) {
	case RGB_SLD:
		if (record->event.pressed)
			rgblight_mode(1);
		return false;
	case MACRO_1:
		if (record->event.pressed)
			SEND_STRING(SS_TAP(X_MINUS) SS_DELAY(100)
					SS_LSFT(SS_TAP(X_DOT)));
		break;
	case DISCO_TIME:
		if (record->event.pressed) {
			disco_enabled ^= 1;

                	rgblight_enable_noeeprom();
                	rgblight_mode_noeeprom(1);
                	rgblight_sethsv_noeeprom(0, 0, 0);
		}

		return false;
	default:
		break;
	}
	return true;
}

void
post_process_record_user(uint16_t keycode, keyrecord_t *record)
{
	if (disco_enabled && record->event.pressed) {
		if (record->event.key.row < 7) {
			rgblight_sethsv_range(rand() % 255, 255, 255,
					(uint8_t)RGBLED_NUM / 2,
					(uint8_t)RGBLED_NUM);
		} else {
			rgblight_sethsv_range(rand() % 255, 255, 255, 0,
					(uint8_t)RGBLED_NUM / 2);
		}
	}
}

void
keyboard_post_init_user(void)
{
	layer_state_set_user(layer_state);
}

#define CONCT(x, y) x ## y
#define CONCT3(x, y, z) x ## y ## z
#define DANCE(num, key1, key2, key3)                                          \
void CONCT3(dance, num, _fin)(qk_tap_dance_state_t *state, void *data);       \
void CONCT3(dance, num, _set)(qk_tap_dance_state_t *state, void *data);       \
                                                                              \
void                                                                          \
CONCT3(dance, num, _fin)(qk_tap_dance_state_t *state, void *data)             \
{                                                                             \
	if (state->count == 1)                                                \
		register_code16(key1);                                        \
	else if (state->count == 2)                                           \
		register_code16(key2);                                        \
	else if (state->count == 3)                                           \
		register_code16(key3);                                        \
	else                                                                  \
		rgblight_mode(4);                                             \
}                                                                             \
                                                                              \
void                                                                          \
CONCT3(dance, num, _set)(qk_tap_dance_state_t *state, void *data)             \
{                                                                             \
	wait_ms(10);                                                          \
	if (state->count == 1)                                                \
		unregister_code16(key1);                                      \
	else if (state->count == 2)                                           \
		unregister_code16(key2);                                      \
	else if (state->count == 3)                                           \
		unregister_code16(key3);                                      \
}
DANCE(1, KC_F1,  KC_F2,  KC_F3)
DANCE(2, KC_F4,  KC_F5,  KC_F6)
DANCE(3, KC_F7,  KC_F8,  KC_F9)
DANCE(4, KC_F10, KC_F11, KC_F12)

#define DANCE_ADD(num)                                                        \
[CONCT(TAP_, num)] = ACTION_TAP_DANCE_FN_ADVANCED(NULL,                       \
		CONCT3(dance, num, _fin), CONCT3(dance, num, _set))
qk_tap_dance_action_t tap_dance_actions[] = {
	DANCE_ADD(1), DANCE_ADD(2), DANCE_ADD(3), DANCE_ADD(4)
};
