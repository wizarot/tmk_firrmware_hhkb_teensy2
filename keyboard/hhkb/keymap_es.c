/*
 * SpaceFN
 * http://geekhack.org/index.php?topic=51069.0
 */
#include "keymap_common.h"


const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default Layer
     * ,----------------------------------------------------------------------------.
     * |Esc/~|   1|   2|   3|   4|   5|   6|   7|   8|   9|   0|   -|   =|VolDn|VolUp|
     * |----------------------------------------------------------------------------|
     * |Tab     |   Q|   W|   E|   R|   T|   Y|   U|   I|   O|   P|   [|   ]|      \|
     * |----------------------------------------------------------------------------|
     * |Control   |   A|   S|   D|   F|   G|   H|   J|   K|   L|   ;|   '|Enter     |
     * |----------------------------------------------------------------------------|
     * |Shift      |   Z|   X|   C|   V|   B|   N|   M|   ,|   .|   /|Shift    | Fn0|
     * |----------------------------------------------------------------------------|
     *       |Gui  |Alt    |         SpaceFN         |Alt    |Gui  |
     *       `-----------------------------------------------------'
     */
    KEYMAP(FN3, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, _VOLDOWN, _VOLUP,  \
           TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSPC,              \
           LCTL,A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,ENT,                    \
           LSFT,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,RSFT,FN0,                    \
                LGUI,LALT,          FN1,                RALT,RGUI),

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
     *       |LANG4|       |                         |Space  |LANG5|
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
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) dprint("P"); else dprint("R");
    if (record->tap.interrupted) dprint("i");

#   define MODS_SHIFT_MASK   (MOD_BIT(KC_LSHIFT)|MOD_BIT(KC_RSHIFT))
    static uint8_t shift_esc_prev_shift;

    switch (id) {
        // Shift + Esc -> ~
        case SHIFT_ESC:
            shift_esc_prev_shift = get_mods()&MODS_SHIFT_MASK;
            if (record->event.pressed) {
                if (shift_esc_prev_shift) {
                    add_key(KC_GRV);
                    send_keyboard_report(); // send grave with shift for tilde result
                } else {
                    del_mods(shift_esc_prev_shift);   // remove shift
                    add_key(KC_ESC);
                    send_keyboard_report();
                    add_mods(shift_esc_prev_shift);   // return shift but not sent
                }
            } else {
                if (shift_esc_prev_shift) {
                    del_key(KC_GRV);
                    send_keyboard_report();
                } else {
                    del_key(KC_ESC);
                    send_keyboard_report();
                }
            }
            break;
    }
}

/*
    switch (id) {
        case SHIFT_ESC:
            if ( keyboard_report->mods & (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) ) {
                add_key(KC_GRV);
                send_keyboard_report(); // send PgUp without Ctrl
            } else {
                add_key(KC_ESC);
                send_keyboard_report();
            }
            break;
    }
    */

const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(2, KC_SPACE),
    [2] = ACTION_MODS_KEY(MOD_LSFT, KC_GRV),    // tilde
    [3] = ACTION_FUNCTION(SHIFT_ESC),
};
