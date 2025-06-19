#include "imgui_impl.hpp"
#include <iostream>
#include <ctime>

ImGuiManager g_imguiManager;

ImGuiManager::ImGuiManager() 
    : m_initialized(false), m_showLogs(true), m_autoScroll(true) {
}

ImGuiManager::~ImGuiManager() {
    if (m_initialized) {
        Shutdown();
    }
}

bool ImGuiManager::Init(GLFWwindow* window) {
    if (m_initialized) return true;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        return false;
    }
    
    m_initialized = true;
    AddLog("ImGui initialized successfully", ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    
    return true;
}

void ImGuiManager::Shutdown() {
    if (!m_initialized) return;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    m_initialized = false;
}

void ImGuiManager::NewFrame() {
    if (!m_initialized) return;
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render() {
    if (!m_initialized) return;
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::ShowInterface(int& currentEffect, float& planetRotationSpeed) {
    if (!m_initialized) return;
    
    ImGui::Begin("Solar System Controls");
    
    ShowEffectButtons(currentEffect);
    ImGui::Separator();
    ShowPlanetControls(planetRotationSpeed);
    ImGui::Separator();
    
    if (ImGui::Button("Show/Hide Logs")) {
        m_showLogs = !m_showLogs;
    }
    
    ImGui::End();
    
    if (m_showLogs) {
        ShowLogWindow();
    }
}

void ImGuiManager::ShowEffectButtons(int& currentEffect) {
    ImGui::Text("Post-Processing Effects:");
    
    const char* effectNames[] = {
        "No Effect",
        "Blur", 
        "Edge Detection",
        "Color Inversion",
        "Grayscale",
        "High Contrast",
        "Vignette"
    };
    
    ImGui::Text("Current Effect: %s", effectNames[currentEffect]);
    ImGui::Spacing();
    
    for (int i = 0; i < 7; i++) {
        if (i > 0 && i % 3 != 0) {
            ImGui::SameLine();
        }
        
        ImGui::PushID(i);
        
        if (currentEffect == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
            
            ImGui::Button(effectNames[i], ImVec2(130, 30));
            
            ImGui::PopStyleColor(3);
        } else {
            if (ImGui::Button(effectNames[i], ImVec2(130, 30))) {
                currentEffect = i;
                AddLog("Effect changed to: " + std::string(effectNames[i]), 
                       ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
            }
        }
        
        ImGui::PopID();
    }
}

void ImGuiManager::ShowPlanetControls(float& planetRotationSpeed) {
    ImGui::Text("Planet Animation:");
    
    float oldSpeed = planetRotationSpeed;
    ImGui::SliderFloat("Rotation Speed", &planetRotationSpeed, 0.0f, 5.0f, "%.2f");
    
    if (planetRotationSpeed != oldSpeed) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Planet rotation speed changed to: %.2f", planetRotationSpeed);
        AddLog(buffer, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
    }
    
    ImGui::Text("Presets:");
    if (ImGui::Button("Stop")) {
        planetRotationSpeed = 0.0f;
        AddLog("Planets stopped", ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    }
    ImGui::SameLine();
    if (ImGui::Button("Normal")) {
        planetRotationSpeed = 1.0f;
        AddLog("Normal speed restored", ImVec4(0.0f, 1.0f, 0.8f, 1.0f));
    }
    ImGui::SameLine();
    if (ImGui::Button("Fast")) {
        planetRotationSpeed = 3.0f;
        AddLog("Fast rotation enabled", ImVec4(1.0f, 0.0f, 0.8f, 1.0f));
    }
}

void ImGuiManager::ShowLogWindow() {
    ImGui::Begin("Logs", &m_showLogs);
    
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    ImGui::SameLine();
    if (ImGui::Button("Clear Logs")) {
        m_logs.clear();
        AddLog("Logs cleared", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    }
    
    ImGui::Separator();
    
    ImGui::BeginChild("LogRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& log : m_logs) {
        ImGui::PushStyleColor(ImGuiCol_Text, log.color);
        ImGui::Text("[%.2f] %s", log.timestamp, log.message.c_str());
        ImGui::PopStyleColor();
    }
    
    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::End();
}

void ImGuiManager::AddLog(const std::string& message, ImVec4 color) {
    LogEntry entry;
    entry.message = message;
    entry.timestamp = static_cast<float>(glfwGetTime());
    entry.color = color;
    
    m_logs.push_back(entry);
    
    while (m_logs.size() > MAX_LOGS) {
        m_logs.pop_front();
    }
    
    std::cout << "[" << entry.timestamp << "] " << message << std::endl;
}
