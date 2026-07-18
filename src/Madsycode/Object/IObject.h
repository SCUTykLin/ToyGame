//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_IOBJECT_H
#define TOYPROJECTS_Madsycode_IOBJECT_H

namespace Madsycode::Object {
    class IObject {
    public:
        virtual void Draw() = 0;
        virtual void Update(float dt) = 0;
        virtual void Clean() = 0;
    };
}

#endif //TOYPROJECTS_Madsycode_IOBJECT_H