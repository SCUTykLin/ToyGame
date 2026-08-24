//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_LAYER_H
#define TOYPROJECTS_Madsycode_LAYER_H

namespace Madsycode::Map {
    class Layer {
    public:
        virtual void Render() = 0;
        virtual void Update() = 0;
    };
}

#endif //TOYPROJECTS_Madsycode_LAYER_H