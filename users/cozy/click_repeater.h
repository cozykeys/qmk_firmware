#ifndef __CLICK_REPEATER__
#define __CLICK_REPEATER__

enum click_repeater_state {
    CR_DISABLED,
    CR_WAITING,
    CR_BUTTON_DOWN,

    CR_STATE_COUNT
};

struct click_repeater_t {
    enum click_repeater_state state;
    uint32_t previous_button_down;
    uint32_t previous_button_up;

    uint32_t click_duration;
    const uint32_t click_duration_min;
    const uint32_t click_duration_max;

    uint32_t wait_duration;
    const uint32_t wait_duration_min;
    const uint32_t wait_duration_max;
};

void cr_enable(struct click_repeater_t *cr);
void cr_disable(struct click_repeater_t *cr);
void cr_poll(struct click_repeater_t *cr);

#endif // __CLICK_REPEATER__
