#pragma once

#include <dinput.h>
#include <imgui.h>

namespace xr_imgui
{
    inline ImGuiKey xr_key_to_imgui_key(int key)
    {
        switch (key)
        {
        case DIK_UNLABELED:             return ImGuiKey_None;
        case DIK_A:                     return ImGuiKey_A;
        case DIK_B:                     return ImGuiKey_B;
        case DIK_C:                     return ImGuiKey_C;
        case DIK_D:                     return ImGuiKey_D;
        case DIK_E:                     return ImGuiKey_E;
        case DIK_F:                     return ImGuiKey_F;
        case DIK_G:                     return ImGuiKey_G;
        case DIK_H:                     return ImGuiKey_H;
        case DIK_I:                     return ImGuiKey_I;
        case DIK_J:                     return ImGuiKey_J;
        case DIK_K:                     return ImGuiKey_K;
        case DIK_L:                     return ImGuiKey_L;
        case DIK_M:                     return ImGuiKey_M;
        case DIK_N:                     return ImGuiKey_N;
        case DIK_O:                     return ImGuiKey_O;
        case DIK_P:                     return ImGuiKey_P;
        case DIK_Q:                     return ImGuiKey_Q;
        case DIK_R:                     return ImGuiKey_R;
        case DIK_S:                     return ImGuiKey_S;
        case DIK_T:                     return ImGuiKey_T;
        case DIK_U:                     return ImGuiKey_U;
        case DIK_V:                     return ImGuiKey_V;
        case DIK_W:                     return ImGuiKey_W;
        case DIK_X:                     return ImGuiKey_X;
        case DIK_Y:                     return ImGuiKey_Y;
        case DIK_Z:                     return ImGuiKey_Z;
        case DIK_1:                     return ImGuiKey_1;
        case DIK_2:                     return ImGuiKey_2;
        case DIK_3:                     return ImGuiKey_3;
        case DIK_4:                     return ImGuiKey_4;
        case DIK_5:                     return ImGuiKey_5;
        case DIK_6:                     return ImGuiKey_6;
        case DIK_7:                     return ImGuiKey_7;
        case DIK_8:                     return ImGuiKey_8;
        case DIK_9:                     return ImGuiKey_9;
        case DIK_0:                     return ImGuiKey_0;
        case DIK_RETURN:                return ImGuiKey_Enter;
        case DIK_ESCAPE:                return ImGuiKey_Escape;
        case DIK_BACKSPACE:             return ImGuiKey_Backspace;
        case DIK_TAB:                   return ImGuiKey_Tab;
        case DIK_SPACE:                 return ImGuiKey_Space;
        case DIK_MINUS:                 return ImGuiKey_Minus;
        case DIK_EQUALS:                return ImGuiKey_Equal;
        case DIK_LBRACKET:              return ImGuiKey_LeftBracket;
        case DIK_RBRACKET:              return ImGuiKey_RightBracket;
        case DIK_BACKSLASH:             return ImGuiKey_Backslash;
        case DIK_SEMICOLON:             return ImGuiKey_Semicolon;
        case DIK_APOSTROPHE:            return ImGuiKey_Apostrophe;
        case DIK_GRAVE:                 return ImGuiKey_GraveAccent;
        case DIK_COMMA:                 return ImGuiKey_Comma;
        case DIK_PERIOD:                return ImGuiKey_Period;
        case DIK_SLASH:                 return ImGuiKey_Slash;
        case DIK_CAPSLOCK:              return ImGuiKey_CapsLock;
        case DIK_F1:                    return ImGuiKey_F1;
        case DIK_F2:                    return ImGuiKey_F2;
        case DIK_F3:                    return ImGuiKey_F3;
        case DIK_F4:                    return ImGuiKey_F4;
        case DIK_F5:                    return ImGuiKey_F5;
        case DIK_F6:                    return ImGuiKey_F6;
        case DIK_F7:                    return ImGuiKey_F7;
        case DIK_F8:                    return ImGuiKey_F8;
        case DIK_F9:                    return ImGuiKey_F9;
        case DIK_F10:                   return ImGuiKey_F10;
        case DIK_F11:                   return ImGuiKey_F11;
        case DIK_F12:                   return ImGuiKey_F12;
        //case DIK_P:                   return ImGuiKey_PrintScreen;
        case DIK_SCROLL:                return ImGuiKey_ScrollLock;
        case DIK_PAUSE:                 return ImGuiKey_Pause;
        case DIK_INSERT:                return ImGuiKey_Insert;
        case DIK_HOME:                  return ImGuiKey_Home;
        case DIK_PGUP:                  return ImGuiKey_PageUp;
        case DIK_DELETE:                return ImGuiKey_Delete;
        case DIK_END:                   return ImGuiKey_End;
        case DIK_PGDN :                 return ImGuiKey_PageDown;
        case DIK_RIGHT:                 return ImGuiKey_RightArrow;
        case DIK_LEFT:                  return ImGuiKey_LeftArrow;
        case DIK_DOWN:                  return ImGuiKey_DownArrow;
        case DIK_UP:                    return ImGuiKey_UpArrow;
        case DIK_NUMLOCK:               return ImGuiKey_NumLock;
        case DIK_NUMPADSLASH:           return ImGuiKey_KeypadDivide;
        case DIK_NUMPADSTAR:            return ImGuiKey_KeypadMultiply;
        case DIK_NUMPADMINUS:           return ImGuiKey_KeypadSubtract;
        case DIK_NUMPADPLUS:            return ImGuiKey_KeypadAdd;
        case DIK_NUMPADENTER:           return ImGuiKey_KeypadEnter;
        case DIK_NUMPAD1:               return ImGuiKey_Keypad1;
        case DIK_NUMPAD2:               return ImGuiKey_Keypad2;
        case DIK_NUMPAD3:               return ImGuiKey_Keypad3;
        case DIK_NUMPAD4:               return ImGuiKey_Keypad4;
        case DIK_NUMPAD5:               return ImGuiKey_Keypad5;
        case DIK_NUMPAD6:               return ImGuiKey_Keypad6;
        case DIK_NUMPAD7:               return ImGuiKey_Keypad7;
        case DIK_NUMPAD8:               return ImGuiKey_Keypad8;
        case DIK_NUMPAD9:               return ImGuiKey_Keypad9;
        case DIK_NUMPAD0:               return ImGuiKey_Keypad0;
        case DIK_NUMPADPERIOD:          return ImGuiKey_KeypadDecimal;
        case DIK_APPS:                  return ImGuiKey_Menu;
        case DIK_NUMPADEQUALS:          return ImGuiKey_KeypadEqual;
        case DIK_LCONTROL:              return ImGuiKey_LeftCtrl;
        case DIK_LSHIFT:                return ImGuiKey_LeftShift;
        case DIK_LALT:                  return ImGuiKey_LeftAlt;
        case DIK_LWIN:                  return ImGuiKey_LeftSuper;
        case DIK_RCONTROL:              return ImGuiKey_RightCtrl;
        case DIK_RSHIFT:                return ImGuiKey_RightShift;
        case DIK_RALT:                  return ImGuiKey_RightAlt;
        case DIK_RWIN :                 return ImGuiKey_RightSuper;
        } // switch key
        return ImGuiKey_None;
    }

} // namespace xr_imgui