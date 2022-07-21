#include "imgui_code.h"
#include "imgui.h"
#include <sstream>
#include <iostream>
#include <vector>
#include "imgui_stdlib.h"
#include "boost/algorithm/string.hpp"
#include "RMIO.h"
namespace RMImGui {
    /*ImVec4 GetColor(std::string name) {
        if (name == "local") {
            return { 0.2, 0.6, 0.2, 1.0 };
        }else if (name == "local") {
            return { 0.2, 0.6, 0.2, 0.4 };
        }

        return { 1.0, 1.0, 1.0, 1.0 };
    }

    char delimiters[] = { '/', '=', ' ', '+', '-', '*', '\"', '\'', '(', '[', '{', '}', ']', ')', '?', '$', '%', '!' };

    bool Delimited(char element) {
        return std::find(std::begin(delimiters), std::end(delimiters), element) != std::end(delimiters);
    }*/

    bool DisplayCode(RMImGui::AnimationWindow* w, RMImGui::ImGuiData& d) {
        /*std::stringstream ss_line(code);
        std::string to_line;*/

        ////"/", "=", " ", "+", "-", "*", "\"", "'", "(", "[", "{", "}", "]", ")", "?", "$", "%", "!"
        //if (code != NULL)
        //{
        //    while (std::getline(ss_line, to_line, '\n')) {
        //        /*std::vector<std::string> parts;
        //        boost::split(parts, to_line, boost::is_any_of(("/= +-*\"'([{}])?$%!")));
        //        for (int i = 0; i < parts.size(); i++) {
        //            ImGui::TextColored(GetColor(parts[i]), parts[i].c_str());
        //            if (i != parts.size() - 1) {
        //                ImGui::SameLine();
        //            }
        //        }*/

        //        std::string element = "";
        //        for (int i = 0; i < to_line.size(); i++) {
        //            bool is_delimiter = Delimited(to_line[i]);
        //            if (is_delimiter) { 
        //                ImGui::TextColored(GetColor(element), element.c_str());
        //                ImGui::SameLine();

        //                
        //                if (to_line[i] != ' ') {
        //                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 9);
        //                    ImGui::TextColored(GetColor("" + to_line[i]), std::string(1, to_line[i]).c_str());
        //                    ImGui::SameLine();
        //                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 9);
        //                }
        //                element = "";
        //            }
        //            else {
        //                element += to_line[i];
        //            }
        //        }
        //        if (element != "") {
        //            ImGui::TextColored(GetColor(element), element.c_str());
        //            ImGui::SameLine();
        //        }
        //        ImGui::NewLine();
        //    }
        //}
        /*static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        HelpMarker("You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example. (This is not demonstrated in imgui_demo.cpp because we don't want to include <string> in here)");
        ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
        ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags, ImGuiInputTextFlags_AllowTabInput);
        ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", &flags, ImGuiInputTextFlags_CtrlEnterForNewLine);*/


        bool open = true;
        ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(80000, 80000));
        ImGui::Begin(w->name.c_str(), &open);

        std::string name = "Select Script";

        if (w->f->script != -1) {
            name = d.scripts[w->f->script].name;
        }
        auto script_names = d.getScriptNames();
        if (ImGui::BeginCombo("##script_select", name.c_str())) {
            for (int i = 0; i < script_names.size(); i++) {
                if (ImGui::Selectable(script_names[i], true)) {
                    w->f->script = i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("New Script")) {
            ImGui::OpenPopup(("enter_script_name_" + w->name).c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            w->f->script = -1;
        }

        if (ImGui::BeginPopupContextItem(("enter_script_name_" + w->name).c_str()))
        {
            ImGui::InputText("Enter Script Name", &w->temp_name);
            ImGui::SameLine();
            if (ImGui::Button("Enter") || ImGui::IsKeyReleased(ImGuiKey_Enter)) {
                if (boost::trim_copy(w->temp_name) == "" || w->temp_name.size() >= FILENAME_MAX) {
                    RMIO::PlayErrorSound();
                }
                else {
                    bool found = false;
                    for (auto s : script_names) {
                        if (s == w->temp_name) {
                            found = true;
                            RMIO::PlayErrorSound();
                        }
                    }
                    if (!found) {
                        w->f->script = d.addScript(w->f, w->temp_name);
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }

        if (w->f->script != -1) {
            ImGui::InputTextMultiline("##source", d.scripts[w->f->script].script.data(), SCRIPT_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
            if (ImGui::Button("Compile")) {
                d.scripts[w->f->script].compile();
            }
            ImGui::SameLine();
            if (ImGui::Button("Run")) {
                d.scripts[w->f->script].evaluate(d.timeline.frame / d.timeline.fps);
            }
        }
        ImGui::End();
        return open;
    }
}