#include "UI.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "internal/imgui_impl_sdl_gl3.h"

namespace library
{
UI::UI()
{
    //imguiInit();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
}

UI::~UI()
{
    //imguiShutdown();
}

void UI::set_events(const ui_events &events)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float) events.window_size.x, (float) events.window_size.y);
    io.DeltaTime = events.delta_time;
    io.KeyShift = events.is_key_shift_down;
    io.KeyCtrl = events.is_key_ctrl_down;
    io.KeyAlt = events.is_key_alt_down;
    io.KeySuper = events.is_key_super_down;
    io.MousePos = ImVec2((float) events.mouse_position.x, (float) events.mouse_position.y);
    io.MouseWheel = (float) events.mouse_wheel;
    io.MouseDown[0] = events.is_mouse_button_left_down;
    io.MouseDown[1] = events.is_mouse_button_right_down;
    io.MouseDown[2] = events.is_mouse_button_middle_down;
    memcpy(io.KeysDown, events.keys_down, 512 * sizeof(bool));
    io.AddInputCharactersUTF8(events.text_input);
}

void UI::frame_start()
{
    ImGui::NewFrame();
}

void UI::frame_end()
{
    ImGui::Render();
}

}