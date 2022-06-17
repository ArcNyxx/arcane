/* arcane - qmk keyboard configuration
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

/* decrease size */
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT
#define NO_ACTION_TAPPING

#undef  TAPPING_TERM
#define TAPPING_TERM 270 /* increase buffer time after tap key */

#define ERGODOX_LED_30   /* force non-mirroring behaviour */
