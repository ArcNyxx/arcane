#include QMK_KEYBOARD_H
#include "version.h"
#include "keymap.h"

MKDANCE(1, KC_F1,  KC_F2,  KC_F3)
MKDANCE(2, KC_F4,  KC_F5,  KC_F6)
MKDANCE(3, KC_F7,  KC_F8,  KC_F9)
MKDANCE(4, KC_F10, KC_F11, KC_F12)

qk_tap_dance_action_t tap_dance_actions[] = {
	[DANCE1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance1_fin, dance1_set),
	[DANCE2] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance2_fin, dance2_set),
	[DANCE3] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance3_fin, dance3_set),
	[DANCE4] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance4_fin, dance4_set)
};

enum { DISCO = EZ_SAFE_RANGE, MACRO };
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_ergodox_pretty(
		KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TRNS,
		KC_TRNS, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_TRNS,

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
		KC_SPC,  KC_ESC, KC_UNDS,  MACRO,   KC_BSPC, KC_ENT
	)
};

#define LEDS (uint8_t)RGBLED_NUM
#define DIST(x, y) ((x) > (y) ? (x) - (y) : (y) - (x))
#define EASE(num) ((uint32_t)(num) * (num) * (num) / 255 / 255)
#define VALD(prog, i, centre) (EASE(prog) * 5 / (5 + DIST(i, centre)))

static bool disco = true;
static uint8_t lhue, lprog, lcentre, lkeys;
static uint8_t rhue, rprog, rcentre, rkeys;
static uint16_t timer;

void
keyboard_post_init_user(void)
{
	rgblight_enable_noeeprom();
	rgblight_mode_noeeprom(1);
	rgblight_sethsv_noeeprom(0, 0, 0);
}

bool
process_record_user(uint16_t keycode, keyrecord_t *record)
{
	switch (keycode) {
	case MACRO:
		if (record->event.pressed)
			SEND_STRING(SS_TAP(X_MINUS) SS_DELAY(100)
					SS_LSFT(SS_TAP(X_DOT)));
		return false;
	case DISCO:
		if (record->event.pressed) {
			disco = !disco;
			lprog = lkeys = rprog = rkeys = 0;
		}
		return false;
	default:
		return true;
	}
}

void
post_process_record_user(uint16_t keycode, keyrecord_t *record)
{
	if (!disco)
		return;

	if (!record->event.pressed) {
		if (record->event.key.row < 7 && lkeys > 0)
			--lkeys;
		else if (record->event.key.row >= 7 && rkeys > 0)
			--rkeys;
		return;
	}
	
	if (record->event.key.row < 7) {
		lhue = rand() % 255, lprog = 255, ++lkeys;
		lcentre = 30 - (record->event.key.row * 2 + 2);
		for (int i = LEDS / 2; i < LEDS; ++i)
			sethsv(lhue, 255, VALD(lprog, i, lcentre), &led[i]);
	} else {
		rhue = rand() % 255, rprog = 255, ++rkeys;
		rcentre = 30 - (record->event.key.row * 2 + 2);
		for (int i = LEDS; i < LEDS / 2; ++i)
			sethsv(rhue, 255, VALD(rprog, i, rcentre), &led[i]);
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
	if (!(disco && (lprog > 0 || rprog > 0) && timer_elapsed(timer) > 20))
		return;
	timer = timer_read();

	if (lkeys == 0 && lprog > 0) {
		lprog -= 5;
		for (int i = LEDS / 2; i < LEDS; ++i)
			sethsv(lhue, 255, VALD(lprog, i, lcentre), &led[i]);
	}
	if (rkeys == 0 && rprog > 0) {
		rprog -= 5;
		for (int i = 0; i < LEDS / 2; ++i)
			sethsv(rhue, 255, VALD(rprog, i, rcentre), &led[i]);
	}
	rgblight_set();
}
