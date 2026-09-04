#include "Gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../configs.h"
#include <string>

void Gui::init(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    font = io.Fonts->AddFontFromFileTTF(ASSETS_DIR "/font/font.ttf", 24.0f);
    titleFont = io.Fonts->AddFontFromFileTTF(ASSETS_DIR "/font/font.ttf", 120.0f); // Font grande per il titolo
}

UiCommand Gui::drawMenu(int points, float vpX, float vpY, float vpW, float vpH)
{
    UiCommand command = UiCommand::None;
    ImGui::SetNextWindowPos(ImVec2(vpX, vpY));
    ImGui::SetNextWindowSize(ImVec2(vpW, vpH));

    ImGui::Begin("##MenuOverlay", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize);

    // Controlliamo che entrambi i font siano carichi
    if (font && titleFont)
    {
        // --- 1. RECORD IN ALTO ---
        ImGui::PushFont(font);
        float originalScale = font->Scale; // Salviamo la scala originale

        float hudHeight = vpH / ROWS;
        font->Scale = POINTS_SIZE_SCALE;
        float currentFontSize = ImGui::GetFontSize();
        float marginLeft = vpW * 0.02f;
        float offsetY = (hudHeight - currentFontSize) / 2.0f;

        writePoints("ALL-TIME POINTS RECORD: ", points, marginLeft, offsetY);

        font->Scale = originalScale; // Resettiamo la scala del font piccolo
        ImGui::PopFont();            // Chiudiamo il font piccolo per passare al grande

        // --- 2. TITOLO GRANDE ---
        ImGui::PushFont(titleFont);
        std::string title = "CGTAN";
        ImVec2 titleSize = ImGui::CalcTextSize(title.c_str());
        float titleX = (vpW - titleSize.x) / 2.0f;
        float titleY = vpH * 0.25f;

        ImGui::SetCursorPos(ImVec2(titleX, titleY));

        for (size_t i = 0; i < title.length(); ++i)
        {
            if (i > 0)
                ImGui::SameLine(0, 0);
            glm::vec4 c = pointsColors[i % 8]; // Usiamo l'array globale pointsColors
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c.r, c.g, c.b, c.a));
            ImGui::Text("%c", title[i]);
            ImGui::PopStyleColor();
        }
        ImGui::PopFont(); // Chiudiamo titleFont

        // --- 3. BOTTONI ---
        ImGui::PushFont(font); // Torniamo al font normale per i bottoni
        font->Scale = FONT_SIZE_SCALE;

        float buttonWidth = vpW * 0.5f;
        float buttonHeight = vpH * 0.10f;
        float spacing = vpH * 0.05f;
        float centerX = (vpW - buttonWidth) / 2.0f;
        float centerY = vpH * 0.5f;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 0.05f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.8f, 1.0f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::SetCursorPos(ImVec2(centerX, centerY));
        if (ImGui::Button("START GAME", ImVec2(buttonWidth, buttonHeight)))
            command = UiCommand::StartGame;

        ImGui::SetCursorPos(ImVec2(centerX, centerY + buttonHeight + spacing));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("EXIT", ImVec2(buttonWidth, buttonHeight)))
            command = UiCommand::Exit;

        ImGui::PopStyleColor(); // Bordo rosso
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5); // Colori base bottoni

        font->Scale = originalScale; // Reset finale
        ImGui::PopFont();
    }

    ImGui::End();
    return command;
}
UiCommand Gui::drawHud(const int points, float vpX, float vpY, float vpW, float vpH)
{
    UiCommand command = UiCommand::None;
    float hudHeight = vpH / ROWS;

    ImGui::SetNextWindowPos(ImVec2(vpX, vpY));
    ImGui::SetNextWindowSize(ImVec2(vpW, hudHeight));

    ImGui::Begin("##HudOverlay", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBackground);

    if (font)
    {
        ImGui::PushFont(font);
        font->Scale = POINTS_SIZE_SCALE;
        float currentFontSize = ImGui::GetFontSize();

        // Testo punti a sinistra - CENTRA COME IN drawMenu
        float marginLeft = vpW * 0.02f;
        float offsetY = (hudHeight - currentFontSize) / 2.0f;

        writePoints("POINTS: ", points, marginLeft, offsetY);

        // Bottone X a destra
        font->Scale = FONT_SIZE_SCALE;
        float buttonSize = hudHeight * 0.7f;
        float marginRight = vpW * 0.02f;
        float buttonX = vpW - buttonSize - marginRight;
        float buttonY = (hudHeight - buttonSize) / 2.0f;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 0.05f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::SetCursorPos(ImVec2(buttonX, buttonY));
        if (ImGui::Button("X", ImVec2(buttonSize, buttonSize)))
            command = UiCommand::BackToMenu;

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);
        font->Scale = FONT_SIZE_SCALE;
        ImGui::PopFont();
    }

    ImGui::End();
    return command;
}

void Gui::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::writePoints(const std::string &text, int points, float x, float y)
{
    std::string prefix = text;
    std::string pointsStr = std::to_string(points);

    // Centra verticalmente
    ImGui::SetCursorPos(ImVec2(x, y));

    // Prefisso bianco
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::Text("%s", prefix.c_str());
    ImGui::PopStyleColor();

    // Mantieni la Y mentre scrivi i numeri
    ImGui::SameLine(0, 0);

    for (size_t i = 0; i < pointsStr.length(); ++i)
    {
        ImGui::SameLine(0, 0);
        glm::vec4 color = pointsColors[i % 8];
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
        ImGui::Text("%c", pointsStr[i]);
        ImGui::PopStyleColor();
    }
}