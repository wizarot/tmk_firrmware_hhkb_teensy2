/*
 * SpaceFN
 * http://geekhack.org/index.php?topic=51069.0
 */
#include "keymap_common.h"


const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default Layer
     * ,----------------------------------------------------------------------------.
     * |Esc/~|   1|   2|   3|   4|   5|   6|   7|   8|   9|   0|   -|   =|VolDn|VolUp|
     * |-----------------------------------------------------------------------------|
     * |Tab   |   Q|   W|   E|   R|   T|   Y|   U|   I|   O|   P|   [|   ]|Bkspc/Pipe|
     * |-----------------------------------------------------------------------------|
     * |Control   |   A|   S|   D|   F|   G|   H|   J|   K|   L|   ;|   '|Enter      |
     * |-----------------------------------------------------------------------------|
     * |Shift      |   Z|   X|   C|   V|   B|   N|   M|   ,|   .|   /|Shift    |Fn0  |
     * |-----------------------------------------------------------------------------|
     *       |Gui  |Alt    |         SpaceFN         |Alt    |Gui  |
     *       `-----------------------------------------------------'
     */
    KEYMAP(FN2, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, _VOLDOWN, _VOLUP,  \
           TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC, FN3,              \
           LCTL,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,ENT,                    \
           LSFT,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,RSFT,FN0,                    \
                LGUI,LALT,          FN1,                RALT,PAUS),

    /* Layer 1: HHKB mode (HHKB Fn)
     * ,-----------------------------------------------------------.
     * |Pwr| F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|Ins|Del|
     * |-----------------------------------------------------------|
     * |Caps |   |   |   |   |   |   |   |Psc|Slk|Pus|Up |   |Backs|
     * |-----------------------------------------------------------|
     * |      |VoD|VoU|Mut|   |   |  *|  /|Hom|PgU|Lef|Rig|Enter   |
     * |-----------------------------------------------------------|
     * |        |   |   |   |   |   |  +|  -|End|PgD|Dow|      |   |
     * `-----------------------------------------------------------'
     *       |   |     |                       |     |   |
     *       `-------------------------------------------'
     */ 
    KEYMAP(POWER, SLEP,  WAKE,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10,TRNS, F11, TRNS, TRNS,  \
           CAPS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,PSCR,SLCK,PAUS, UP, TRNS, BSLS,                \
           TRNS,VOLD,VOLU,MUTE,TRNS,TRNS,PAST,PSLS,HOME,PGUP,LEFT,RGHT,PENT,                      \
           TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,PPLS,PMNS,END, PGDN,DOWN,TRNS,TRNS,                      \
                TRNS,TRNS,          SPC,                TRNS,TRNS),

    /* Layer 2: SpaceFN
     * ,----------------------------------------------------------------------------.
     * |`   |  F9| F10| F11| F12|    |    |    |    |    |    |    |    | Mute| OutP|
     * ,----------------------------------------------------------------------------.
     * |LANG3   |  F5|  F6|  F7|  F8|    |    |    |    |    |PScn|    |    |      \|
     * |----------------------------------------------------------------------------|
     * |LALT      |  F1|  F2|  F3|  F4|    |Left|Down|  Up|Righ|Home| End|LANG1     |
     * |----------------------------------------------------------------------------|
     * |           |    |    |    |    |LANG2|   |   ~|   `|    |    |         |    |
     * |----------------------------------------------------------------------------|
     *       |LANG4|       |                         |Space  |PAUS |
     *       `-----------------------------------------------------'
     *
     *       LANG1 (SpaceFN+ENTER) assigned in i3 to spawn terminal
     *       LANG2 (SpaceFN+B)     assigned in i3 to spawn browser
     *       LANG3 (SpaceFN+TAB)   assigned in i3 to kill window
     */
    KEYMAP(GRV, F9, F10, F11, F12,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS, _MUTE,OUT,   \
           LANG3,F5 ,F6  ,F7  ,F8  ,TRNS,TRNS,TRNS,TRNS,TRNS, PSCR,TRNS,TRNS,BSLS,      \
           LALT,F1  ,F2  ,F3  ,F4  ,TRNS,LEFT,DOWN,UP,RGHT,HOME,END ,LANG1,             \
           TRNS,TRNS,TRNS,TRNS,TRNS,LANG2, TRNS,GRV, FN2, TRNS,TRNS,TRNS,NO,            \
                LANG4,TRNS,          TRNS,             SPC ,LANG5),
};

/*
 * user defined action function
 */
enum function_id {
    SHIFT_ESC,       // Shift + Esc -> Tilde
    SHIFT_BSPC,     // Shift + Backspace -> Pipe
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) dprint("P"); else dprint("R");
    if (record->tap.interrupted) dprint("i");

#   define MODS_SHIFT_MASK   (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
    static uint8_t shift_mod;

    switch (id) {
        // Shift + Esc -> ~
        case SHIFT_ESC:
            shift_mod = get_mods()&MODS_SHIFT_MASK;
            if (record->event.pressed) {
                if (shift_mod) {
                    add_key(KC_GRV);
                    send_keyboard_report(); // send grave with shift for tilde result
                } else {
                    del_mods(shift_mod); // remove shift
                    add_key(KC_ESC);
                    send_keyboard_report(); // send escape
                    add_mods(shift_mod); // return shift but not sent
                }
            } else {
                if (shift_mod) {
                    del_key(KC_GRV);
                    send_keyboard_report();
                } else {
                    del_key(KC_ESC);
                    send_keyboard_report();
                }
            }
            break;
        case SHIFT_BSPC:
            shift_mod = get_mods()&MODS_SHIFT_MASK;
            if (record->event.pressed) {
                if (shift_mod) {
                    add_key(KC_BSLS);
                    send_keyboard_report(); // send backslash with shift for pipe result
                } else {
                    del_mods(shift_mod); // remove shift
                    add_key(KC_BSPC);
                    send_keyboard_report(); // send backspace
                    add_mods(shift_mod); // return shift but not sent
                }
            } else {
                if (shift_mod) {
                    del_key(KC_BSLS);
                    send_keyboard_report();
                } else {
                    del_key(KC_BSPC);
                    send_keyboard_report();
                }
            }
            break;
    }
}

const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(2, KC_SPACE),
    [2] = ACTION_FUNCTION(SHIFT_ESC),
    [3] = ACTION_FUNCTION(SHIFT_BSPC),
//  [x] = ACTION_MODS_KEY(MOD_LSFT, KC_GRV),    // tilde
};
