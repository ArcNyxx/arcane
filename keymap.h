/* arcane - qmk keyboard configuration
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#define MKDANCE(num, key1, key2, key3)                                        \
enum { DANCE ## num = num };                                                  \
                                                                              \
void                                                                          \
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
void                                                                          \
dance ## num ## _set(qk_tap_dance_state_t *state, void *data)                 \
{                                                                             \
	wait_ms(10);                                                          \
	if (state->count == 1)                                                \
		unregister_code16(key1);                                      \
	else if (state->count == 2)                                           \
		unregister_code16(key2);                                      \
	else if (state->count == 3)                                           \
		unregister_code16(key3);                                      \
}
