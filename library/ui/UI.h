#pragma once

#include "math/Math.h"
#include <imgui.h>

namespace library
{
class UI
{
public:
    UI();
    virtual ~UI();

    struct ui_events
    {
        ivec2 window_size;
        float delta_time;
        bool is_key_shift_down;
        bool is_key_ctrl_down;
        bool is_key_alt_down;
        bool is_key_super_down;
        ivec2 mouse_position;
        int mouse_wheel;
        bool is_mouse_button_left_down;
        bool is_mouse_button_right_down;
        bool is_mouse_button_middle_down;
        const bool *keys_down;
        const char *text_input;
    };

    void set_events(const ui_events &events);
    void frame_start();
    void frame_end();

    ImFont* font;
};
}