//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_CHARACTERS_H
#define TOYPROJECTS_Madsycode_CHARACTERS_H

#include "../Object/GameObject.h"
#include <string>

namespace Madsycode::Character {
    class Character : public Object::GameObject{
    public:
        Character(Object::Properties* props) : Object::GameObject(props) {};
    protected:
        std::string m_Name;
    };
}

#endif //TOYPROJECTS_Madsycode_CHARACTERS_H