#include "click_repeater.h"
#include <stdlib.h>
#include QMK_KEYBOARD_H

// State handler functions
void cr_waiting(struct click_repeater_t *cr);
void cr_button_down(struct click_repeater_t *cr);

// Utility functions
uint32_t get_rand(uint32_t min, uint32_t max);

typedef void (*cr_state_handler)(struct click_repeater_t *cr);
static cr_state_handler cr_state_handlers[] = {
    [CR_DISABLED] = NULL,
    [CR_WAITING] = cr_waiting,
    [CR_BUTTON_DOWN] = cr_button_down,
};

void cr_enable(struct click_repeater_t *cr) {
    if (cr->state != CR_DISABLED)
        return;
    cr->state = CR_WAITING;
    cr->previous_button_down = 0;
    cr->previous_button_up = 0;
    cr->click_duration = 0;
    cr->wait_duration = 0;
}

void cr_disable(struct click_repeater_t *cr) {
    if (cr->state == CR_BUTTON_DOWN)
        unregister_code(KC_BTN1);
    cr->state = CR_DISABLED;
}

void cr_poll(struct click_repeater_t *cr) {
    cr_state_handler handler = cr_state_handlers[cr->state];
    if (handler)
        (handler)(cr);
}

void cr_waiting(struct click_repeater_t *cr) {
    uint32_t now = timer_read32();
    if (now > (cr->previous_button_up + cr->wait_duration)) {
        cr->state = CR_BUTTON_DOWN;
        cr->previous_button_down = now;
        cr->click_duration = get_rand(cr->click_duration_min, cr->click_duration_max);
        register_code(KC_BTN1);
    }
}

void cr_button_down(struct click_repeater_t *cr) {
    uint32_t now = timer_read32();
    if (now > (cr->previous_button_down + cr->click_duration)) {
        cr->state = CR_WAITING;
        cr->previous_button_up = now;
        cr->wait_duration = get_rand(cr->wait_duration_min, cr->wait_duration_max);
        unregister_code(KC_BTN1);
    }
}

// Return a random number between min and max; assumes that the random number
// generator has already been seeded
uint32_t get_rand(uint32_t min, uint32_t max) {
    return (rand() % (max - min)) + min;
}
