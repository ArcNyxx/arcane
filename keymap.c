/* arcane - qmk keyboard configuration
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include QMK_KEYBOARD_H
#include "version.h"

#define BASE(val, ind, mid) ((uint32_t)(val) * (val) * (val) / 255 / 255) *   \
	5 / (((ind) > (mid) ? (ind) - (mid) : (mid) - (ind)) + 5)
#define SWITCH(num, val, ...)                                                 \
if (__VA_ARGS__) {                                                            \
	ergodox_right_led_ ## num ## _on();                                   \
	ergodox_right_led_ ## num ## _set(val);                               \
} else {                                                                      \
	ergodox_right_led_ ## num ## _off();                                  \
}
#define MKDANCE(num, key1, key2, key3)                                        \
enum { DANCE ## num = num };                                                  \
static void dance ## num ## _fin(qk_tap_dance_state_t *state, void *data);    \
static void dance ## num ## _set(qk_tap_dance_state_t *state, void *data);    \
                                                                              \
static void                                                                   \
dance ## num ## _fin(qk_tap_dance_state_t *state, void *data)                 \
{                                                                             \
	if (state->count == 1)                                                \
		register_code16(key1);                                        \
	else if (state->count == 2)                                           \
		register_code16(key2);                                        \
	else if (state->count == 3)                                           \
		register_code16(key3);                                        \
}                                                                             \
                                                                              \
static void                                                                   \
dance ## num ## _set(qk_tap_dance_state_t *state, void *data)                 \
{                                                                             \
	if (state->count == 1)                                                \
		unregister_code16(key1);                                      \
	else if (state->count == 2)                                           \
		unregister_code16(key2);                                      \
	else if (state->count == 3)                                           \
		unregister_code16(key3);                                      \
}

MKDANCE(1, KC_F1,  KC_F2,  KC_F3)
MKDANCE(2, KC_F4,  KC_F5,  KC_F6)
MKDANCE(3, KC_F7,  KC_F8,  KC_F9)
MKDANCE(4, KC_F10, KC_F11, KC_F12)

static void set_solid(void);

qk_tap_dance_action_t tap_dance_actions[] = {
	[DANCE1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance1_fin, dance1_set),
	[DANCE2] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance2_fin, dance2_set),
	[DANCE3] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance3_fin, dance3_set),
	[DANCE4] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance4_fin, dance4_set)
};

enum { DISCO = EZ_SAFE_RANGE, LOCK, MACRO };
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_ergodox_pretty(
		KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TRNS,
		KC_TRNS, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    LOCK,

		KC_GRV,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_EQL,
		KC_MINS, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,

		KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
		         KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,

		KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LBRC,
		KC_RBRC, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,

		KC_LCTL, KC_RALT, DISCO,   KC_CAPS, KC_LGUI,
		KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_RCTL,

		TD(DANCE1), TD(DANCE2), TD(DANCE3), TD(DANCE4),
		KC_TRNS, KC_TRNS,
		KC_SPC,  KC_ESC,  KC_UNDS, MACRO,   KC_BSPC, KC_ENT
	)
};

uint8_t lhue, lval = 0, lkeys = 0, lmid;
uint8_t rhue, rval = 0, rkeys = 0, rmid;

uint8_t or = 0x5B, og = 0xCE, ob = 0xFA;
uint8_t ir = 0xF5, ig = 0xA9, ib = 0xB8;

bool solid = true, disco = false;
uint8_t backsp[10] = { 1 };
uint16_t timer;

static void
set_solid(void)
{
	for (uint8_t i = 0; i < RGBLED_NUM; ++i) {
		if (i > 7 && i < 22)
			setrgb(ir, ig, ib, &led[i]);
		else
			setrgb(or, og, ob, &led[i]);
	}
	rgblight_set(); rgblight_set();
}

void
keyboard_post_init_user(void)
{
	rgblight_enable_noeeprom();
	set_solid();
}

bool
process_record_user(uint16_t keycode, keyrecord_t *record)
{
	static uint8_t ctrl;
	static bool caps, lock;

	if (keycode == KC_LCTL || keycode == KC_RCTL)
		ctrl += record->event.pressed ? 1 : -1;
	if (keycode == LOCK && record->event.pressed) {
		SWITCH (1, 255, (lock = !lock))
	}
	if (lock)
		return false;
	if (!record->event.pressed)
		return true;

	uint8_t addb = 1;
	switch (keycode) {
	case KC_CAPS:
		SWITCH(3, 150, (caps = !caps))
		return true;
	case MACRO:
		SEND_STRING(SS_TAP(X_MINUS)
				SS_DELAY(100) SS_LSFT(SS_TAP(X_DOT)));
		addb = 2;
		break;
	case DISCO:
		if (ctrl == 0) {
			disco = !disco, lkeys = rkeys = 0;
		} else if ((solid = !solid)) {
			set_solid();
		} else {
			rgblight_setrgb(0, 0, 0);
			lval = rval = 0;
			return false;
		}
		return true;
	case KC_BSPC:
		for (uint8_t i = 1; i < backsp[9]; ++i) {
			unregister_code(KC_BSPC);
			register_code(KC_BSPC);
		}

		memmove(&backsp[1], &backsp[0], 9 * sizeof(uint8_t));
		backsp[0] = 1;
		return true;
	}
	memmove(&backsp[0], &backsp[1], 9 * sizeof(uint8_t));
	backsp[9] = addb;
	return true;
}

void
post_process_record_user(uint16_t keycode, keyrecord_t *record)
{
	if (!disco || solid)
		return;

	if (!record->event.pressed) {
		if (record->event.key.row < 7 && lkeys > 0)
			--lkeys;
		else if (record->event.key.row >= 7 && rkeys > 0)
			--rkeys;
		return;
	}

	if (record->event.key.row < 7) {
		lhue = rand() % 255, lval = 255, ++lkeys, lmid = 15;
		if (record->event.key.col != 5)
			lmid = 28 - 2 * record->event.key.row;
		for (uint8_t i = RGBLED_NUM / 2; i < RGBLED_NUM; ++i)
			sethsv(lhue, 255, BASE(lval, i, lmid), &led[i]);
	} else {
		rhue = rand() % 255, rval = 255, ++rkeys, rmid = 14;
		if (record->event.key.col != 5)
			rmid = 27 - 2 * record->event.key.row;
		for (uint8_t i = 0; i < RGBLED_NUM / 2; ++i)
			sethsv(rhue, 255, BASE(rval, i, rmid), &led[i]);
	}
	rgblight_set();
}

void
matrix_init_user(void)
{
	timer = timer_read();
}

void
matrix_scan_user(void)
{
	if (solid || (lval == 0 && rval == 0) || timer_elapsed(timer) < 20)
		return;
	timer = timer_read();

	if (lkeys == 0 && lval > 0) {
		lval -= 5;
		for (uint8_t i = RGBLED_NUM / 2; i < RGBLED_NUM; ++i)
			sethsv(lhue, 255, BASE(lval, i, lmid), &led[i]);
	}
	if (rkeys == 0 && rval > 0) {
		rval -= 5;
		for (uint8_t i = 0; i < RGBLED_NUM / 2; ++i)
			sethsv(rhue, 255, BASE(rval, i, rmid), &led[i]);
	}
	rgblight_set();
}
