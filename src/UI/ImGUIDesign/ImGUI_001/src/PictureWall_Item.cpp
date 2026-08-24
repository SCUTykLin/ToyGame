//
// Created by 15813 on 2026/8/5.
//

#include "PictureWall_Item.h"
#include <imgui_internal.h>
#include <string>

/*
 * Copy from imgui.py and modify
 */
bool PictureWall::PictureItem(const PictureInfo& info, const char* label, float& size_x) {
    ImGuiWindow* window = ImGui::GetCurrentWindow(); // 获得当前窗口
    if (window->SkipItems) // 是否跳过当前控件
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style; // 上下文样式
    const ImGuiID id = window->GetID(info.texture); // 生成id
    float label_size = ImGui::CalcTextSize(label, nullptr, true).x; // 计算渲染尺寸

    std::string render_label = label;
    bool is_short = false;
    while (label_size > size_x) {
        is_short = true;
        render_label.pop_back();
        label_size = ImGui::CalcTextSize(render_label.c_str(), nullptr, true).x;
    }
    if (is_short && render_label.size() >= 3) {
        render_label.replace(render_label.size() - 3, 3, "...");
    }

    ImVec2 pos = window->DC.CursorPos; // 控件位置
    ImVec2 size = ImVec2(size_x + style.FramePadding.x * 2, size_x + g.FontSize + style.FramePadding.y * 3); // 控件大小

    const ImRect bb(pos, pos + size); // 区域大小
    ImGui::ItemSize(size, style.FramePadding.y); // FramePadding是控件内部的留白
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // Render
    const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderNavHighlight(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding + 5);

    window->DrawList->AddRectFilled(ImVec2(bb.Min + style.FramePadding), ImVec2(bb.Min + style.FramePadding + ImVec2(size_x, size_x)), ImColor(0,0,0, 255), style.FrameRounding + 5);

    ImRect Image_Rect;
    if (info.Proportion < 0) {
        Image_Rect.Min = ImVec2(bb.Min + style.FramePadding);
        Image_Rect.Max = ImVec2(size_x, size_x) + Image_Rect.Min;
    }else {
        if (info.Direction) {
            Image_Rect.Min = ImVec2(bb.Min + style.FramePadding + ImVec2((size_x - size_x * info.Proportion) / 2, 0));
            Image_Rect.Max = ImVec2(size_x * info.Proportion, size_x) + Image_Rect.Min;
        }else {
            Image_Rect.Min = ImVec2(bb.Min + style.FramePadding + ImVec2(0, (size_x - size_x * info.Proportion) / 2));
            Image_Rect.Max = ImVec2(size_x, size_x * info.Proportion) + Image_Rect.Min;
        }
    }

    window->DrawList->AddImageRounded(info.texture, Image_Rect.Min, Image_Rect.Max, ImVec2(), ImVec2(1, 1), ImColor(255, 255, 255, 255), style.FrameRounding + 5);

    window->DrawList->AddText(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + size_x + style.FramePadding.y * 2), ImGui::GetColorU32(ImGuiCol_Text), render_label.c_str());

    return pressed;
}

PictureWall::PictureInfo::PictureInfo(ImTextureID tex, int h_, int w_) : texture(tex), h(h_), w(w_) {
    if (h != w) {
        Proportion = (h > w) ? w / static_cast<float>(h) : h / static_cast<float>(w);
        Direction = (h > w) ? true : false;
    }
}
