//
// Created by 15813 on 2026-10-28.
//

#ifndef TOYPROJECTS_SIMDRPG_SURFACE_H
#define TOYPROJECTS_SIMDRPG_SURFACE_H

#include "core/Types.h"

namespace simdrpg::gfx {
    class Surface {
    public:
        Surface(int w, int h);
        ~Surface();
        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;
        Surface(Surface&&) noexcept;
        Pixel*       data()       { return px_; }

        const Pixel* data() const { return px_; }
        int  width()  const { return w_; }
        int  height() const { return h_; }
        int  pitch()  const { return pitch_; }
        void clear(Pixel color);
        Pixel* row(int y) { return px_ + y * pitch_; }
    private:
        Pixel* px_ = nullptr;
        int w_ = 0, h_ = 0, pitch_ = 0;
    };
}

#endif //TOYPROJECTS_SIMDRPG_SURFACE_H
