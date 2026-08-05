//
// Created by YangKai.Lin on 2026/8/5.
//

#include "PictureWall.h"
#include "PictureWall_Item.h"
#include "ImFileDialog.h"
#include <SDL_opengl.h>
#include <utility>
#include <vector>

#include "ImageLoader.h"

namespace PictureWall {

    struct ItemInfo {
        ItemInfo(std::filesystem::path path_, std::string name_) : name(std::move(name_)) {
            int h, w;
            ImTextureID tex = HImage::HTextureLoader(path_.string().c_str(), &w, &h);
            info = PictureInfo(tex, h, w);
        }
        PictureInfo info;
        std::string name;
    };

    std::vector<ItemInfo> Items;

    void begin() {
        ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
            GLuint tex;

            glGenTextures(1, &tex); // 创建贴图
            glBindTexture(GL_TEXTURE_2D, tex); // 绑定到Texture 2D上下文
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);

            return (void*)tex;
        };
        ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
            GLuint texID = (GLuint)tex;
            glDeleteTextures(1, &texID);
        };
    }

    void update() {
        if (ImGui::Begin("Picture Wall", 0, ImGuiWindowFlags_MenuBar)) {
            if (ImGui::BeginMenuBar()) {
                if (ImGui::Button("Import Picture")) {
                    ifd::FileDialog::Instance().Open("Import Image to Wall", "Import Pictures", "Image file {.png,.jpg},.*", true);
                }
                ImGui::EndMenuBar();
            }

            if (ifd::FileDialog::Instance().IsDone("Import Image to Wall")) {
                if (ifd::FileDialog::Instance().HasResult()) {
                    const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                    for (const auto& r : res) {
                        Items.emplace_back(ItemInfo(r, r.filename().string()));
                    }
                }
                ifd::FileDialog::Instance().Close();
            }

            static float item_size = 150;
            ImGui::SliderFloat("Image Size", &item_size, 10, 500);

            ImGuiStyle& style = ImGui::GetStyle();
            int buttons_count = Items.size();
            float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            for (int n = 0; n < buttons_count; ++n) {
                ImGui::PushID(n);
                PictureItem(Items[n].info, Items[n].name.c_str(), item_size);
                float last_button_x2 = ImGui::GetItemRectMax().x;
                float next_button_x2 = last_button_x2 + style.ItemInnerSpacing.x + item_size;
                if (n + 1 < buttons_count && next_button_x2 < window_visible_x2) {
                    ImGui::SameLine();
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }
}
