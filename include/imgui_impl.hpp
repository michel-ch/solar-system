#ifndef IMGUI_IMPL_HPP
#define IMGUI_IMPL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>
#include <deque>

class ImGuiManager {
public:
    struct LogEntry {
        std::string message;
        float timestamp;
        ImVec4 color;
    };

    ImGuiManager();
    ~ImGuiManager();
    
    bool Init(GLFWwindow* window);
    void Shutdown();
    void NewFrame();
    void Render();
    
    void ShowInterface(int& currentEffect, float& planetRotationSpeed);
    void AddLog(const std::string& message, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    
private:
    bool m_initialized;
    std::deque<LogEntry> m_logs;
    const size_t MAX_LOGS = 100;
    bool m_showLogs;
    bool m_autoScroll;
    
    void ShowEffectButtons(int& currentEffect);
    void ShowPlanetControls(float& planetRotationSpeed);
    void ShowLogWindow();
};

extern ImGuiManager g_imguiManager;

#endif
