#include <imgui.h>
#include <imgui_internal.h>

// mouse lock for drag scalars
// taken from https://github.com/Coollab-Art/imgui/

static int _FindPlatformMonitorForPos(const ImVec2& pos)
{
    ImGuiContext& g = *GImGui;
    for (int monitor_n = 0; monitor_n < g.PlatformIO.Monitors.Size; monitor_n++)
    {
        const ImGuiPlatformMonitor& monitor = g.PlatformIO.Monitors[monitor_n];
        if (ImRect(monitor.MainPos, monitor.MainPos + monitor.MainSize).Contains(pos))
            return monitor_n;
    }
    return -1;
}

static void WrapMousePosEx(const ImRect& wrap_rect)
{
    ImGuiContext& g = *GImGui;
    ImVec2 p_mouse = g.IO.MousePos;
    for (int axis = 0; axis < 2; axis++)
    {
        if (p_mouse[axis] >= wrap_rect.Max[axis])
            p_mouse[axis] = wrap_rect.Min[axis] + 1.0f;
        else if (p_mouse[axis] <= wrap_rect.Min[axis])
            p_mouse[axis] = wrap_rect.Max[axis] - 1.0f;
    }
    if (p_mouse.x != g.IO.MousePos.x || p_mouse.y != g.IO.MousePos.y)
    {
        ImGui::TeleportMousePos(p_mouse);
    }
}

namespace ImGui
{
    void WrapMousePos()
    {
        ImGuiContext& g = *GImGui;
#ifdef IMGUI_HAS_DOCK
        if (g.IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            const int monitor_index = _FindPlatformMonitorForPos(g.IO.MousePosPrev);
            if (monitor_index == -1)
                return;
            const ImGuiPlatformMonitor& monitor = g.PlatformIO.Monitors[monitor_index];
            WrapMousePosEx(ImRect(monitor.MainPos, monitor.MainPos + monitor.MainSize - ImVec2(1.0f, 1.0f)));
        }
        else
#endif
        {
            ImGuiViewport* viewport = GetMainViewport();
            WrapMousePosEx(ImRect(viewport->Pos, viewport->Pos + viewport->Size - ImVec2(1.0f, 1.0f)));
        }
    }

    void LockMousePos()
    {
        static ImVec2 mouse_pos_when_activated{}; // It's ok to use a static variable here, because only one widget will ever be active at the same time.
        if (IsItemActivated())
            mouse_pos_when_activated = GetMousePos();
        if (IsItemActive())
        {
            SetMouseCursor(ImGuiMouseCursor_None);
            WrapMousePos();
        }
        if (IsItemDeactivated())
        {
            GetIO().MousePos = mouse_pos_when_activated;
            GetIO().WantSetMousePos = true;
        }
    }
}
