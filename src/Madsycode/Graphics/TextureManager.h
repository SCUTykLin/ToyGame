//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_Madsycode_TEXTUREMANAGER_H
#define TOYPROJECTS_Madsycode_TEXTUREMANAGER_H

#include <string>

class TextureManager {
public:
    TextureManager();
    static TextureManager* GetInstance(){return s_Instance = (s_Instance != nullptr) ? s_Instance : new TextureManager();}
    bool Load(std::string id, std::string filename);
private:
    TextureManager(){};
    static TextureManager* s_Instance;
};


#endif //TOYPROJECTS_Madsycode_TEXTUREMANAGER_H