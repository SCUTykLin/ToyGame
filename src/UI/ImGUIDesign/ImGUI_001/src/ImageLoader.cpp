//
// Created by YangKai.Lin on 2026/8/5.
//

#include "ImageLoader.h"
#include <SDL_opengl.h>
#include <stb_image.h>
#include <imgui.h>

namespace HImage {

    ImTextureID HTextureLoader(const char* filename, int* out_width, int* out_height) {
        // Load from file
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
        if (image_data == nullptr)
            return nullptr;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *out_width = image_width;
        *out_height = image_height;

        return reinterpret_cast<void*>(image_texture);
    }

    void HTextureUnload(ImTextureID tex) {
        GLuint texID = (GLuint)tex;
        glDeleteTextures(1, &texID);
    }
}