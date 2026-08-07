//
// Created by YangKai.Lin on 2026/8/6.
//

#include "FPS.h"
#include "SDL_FPS.h"
#include <chrono>

namespace ConFPS {
    constexpr int  nScreenWidth = 120;
    constexpr int  nScreenHeight = 40;

    wchar_t *screen = nullptr;
    HANDLE hConsole = nullptr;
    Player_t player;
    Map_t map;

    void createWindowBuffer() {
        screen = new wchar_t[nScreenWidth * nScreenHeight];
        hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
        SetConsoleActiveScreenBuffer(hConsole);
        DWORD dwBytesWritten = 0;

        map.map += L"#########.......";
        map.map += L"#...............";
        map.map += L"#.......########";
        map.map += L"#..............#";
        map.map += L"#......##......#";
        map.map += L"#......##......#";
        map.map += L"#..............#";
        map.map += L"###....#######.#";
        map.map += L"##.............#";
        map.map += L"#......####..###";
        map.map += L"#......#.......#";
        map.map += L"#......#.......#";
        map.map += L"#..............#";
        map.map += L"#......#########";
        map.map += L"#..............#";
        map.map += L"################";

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
    }

    void renderConsole() {
        DWORD dwBytesWritten = 0;
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
    }

    void empty_screen() {
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
    }

    void control(bool* mKey, bool* rKey) {
        static constexpr BYTE vkTable[] = {VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN};
        static constexpr BYTE vkWASD[] = { 'A', 'D', 'W', 'S' };
        for (int k = 0; k < 4; k++) {
            SHORT st = GetAsyncKeyState(vkTable[k]);
            SHORT st_wasd = GetAsyncKeyState(vkWASD[k]);
            mKey[k] = (st & 0x8000) != 0;
            rKey[k] = (st_wasd & 0x8000) != 0;
        }
    }

    void move_player(bool* mKey, bool* rKey, float dt) {
        if (mKey[0]) {
            player.fPlayerA -= (0.8f) * dt;
        } // move left
        if (mKey[1]) {
            player.fPlayerA += (0.8f) * dt;
        } // move right
        if (mKey[2]) {
            player.fPlayerX += sin(player.fPlayerA) * 5.0f * dt;
            player.fPlayerY += cos(player.fPlayerA) * 5.0f * dt;

            if (map.map[(int)player.fPlayerY * map.nMapWidth + (int)player.fPlayerX] == '#') {
                player.fPlayerX -= sin(player.fPlayerA) * 5.0f * dt;
                player.fPlayerY -= cos(player.fPlayerA) * 5.0f * dt;
            }
        } // move up
        if (mKey[3]) {
            player.fPlayerX -= sin(player.fPlayerA) * 5.0f * dt;
            player.fPlayerY -= cos(player.fPlayerA) * 5.0f * dt;
            if (map.map[(int)player.fPlayerY * map.nMapWidth + (int)player.fPlayerX] == '#') {
                player.fPlayerX += sin(player.fPlayerA) * 5.0f * dt;
                player.fPlayerY += cos(player.fPlayerA) * 5.0f * dt;
            }
        } // move down

        if (rKey[0]) {} // turn left
        if (rKey[1]) {} // turn right
    }

    void mainLoop() {
        bool mKey[4], rKey[4];
        FPS_Window fps_window_instance;
        fps_window_instance.init_window(nScreenWidth, nScreenHeight);

        createWindowBuffer();

        auto tp1 = std::chrono::system_clock::now();
        auto tp2 = tp1;

        bool is_running = true;
        float scale = 5.0f;
        while (is_running) {

            if (fps_window_instance.run()) {
                fps_window_instance.ResetBoundary();
                fps_window_instance.ResetDistance();
                fps_window_instance.ResetCategory();
            }

            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsed = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsed.count();

            control(mKey, rKey);
            fps_window_instance.control(is_running, scale);
            move_player(mKey, rKey, fElapsedTime);

            for (int x = 0; x < nScreenWidth; x++) {
                float fRayAngle = (player.fPlayerA - player.fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * player.fFOV;

                float fDistanceToWall = 0.0f;
                bool bHitWall = false;
                bool bBoundary = false;

                float fEyeX = sinf(fRayAngle);
                float fEyeY = cosf(fRayAngle);

                while (!bHitWall && fDistanceToWall < player.fDepth) {
                    fDistanceToWall += 0.1f;

                    int nTestX = (int)(player.fPlayerX + fEyeX * fDistanceToWall);
                    int nTestY = (int)(player.fPlayerY + fEyeY * fDistanceToWall);

                    if (nTestX < 0 || nTestX >= map.nMapWidth || nTestY < 0 || nTestY >= map.nMapHeight) {
                        bHitWall = true;
                        fDistanceToWall = player.fDepth;
                    }else {
                        if (map.map[nTestY * map.nMapWidth + nTestX] == '#') {
                            bHitWall = true;

                            std::vector<std::pair<float, float>> p;

                            for (int tx = 0; tx < 2; tx++) {
                                for (int ty = 0; ty < 2; ty++) { // 墙壁的四个角
                                    float vy = (float)nTestY + ty - player.fPlayerY;
                                    float vx = (float)nTestX + tx - player.fPlayerX;
                                    float d = sqrt(vx * vx + vy * vy);
                                    float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                    p.push_back(std::make_pair(d, dot));
                                }
                            }

                            sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

                            float fBound = 0.01;
                            if (acos(p.at(0).second) < fBound) bBoundary = true;
                            if (acos(p.at(1).second) < fBound) bBoundary = true;
                            if (acos(p.at(2).second) < fBound) bBoundary = true;
                        }
                    }
                }

                int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
                int nFloor = nScreenHeight - nCeiling; // Ceilling和Floor是上下对称的

                short nShade = ' ';
                if (fDistanceToWall <= player.fDepth / 4.0f)  nShade = 0x2588;
                else if (fDistanceToWall < player.fDepth / 3.0f) nShade = 0x2593;
                else if (fDistanceToWall < player.fDepth / 2.0f) nShade = 0x2592;
                else if (fDistanceToWall < player.fDepth) nShade = 0x2591;
                else nShade = ' ';

                if (bBoundary) {
                    nShade = ' ';
                    if (fps_window_instance.run()){
                        for (int y = 0; y < nScreenHeight; y++) fps_window_instance.SetBoundary(x, y, true);
                    }
                }

                for (int y = 0; y < nScreenHeight; y++) {
                    if (y <= nCeiling) {
                        screen[y * nScreenWidth + x] = ' ';
                        if (fps_window_instance.run()) {
                            fps_window_instance.SetCategory(x, y, FPS_Window::CEILING);
                            fps_window_instance.SetDistance(x, y, fDistanceToWall);
                        }
                    }else if (y > nCeiling && y <= nFloor) {
                        screen[y * nScreenWidth + x] = nShade;
                        if (fps_window_instance.run() && nShade != ' ') {
                            fps_window_instance.SetCategory(x, y, FPS_Window::WALL);
                            fps_window_instance.SetDistance(x, y, fDistanceToWall);
                        }
                    }else {
                        float b = 1.0f - (((float)y - nScreenHeight/2.0f) / ((float)nScreenHeight / 2.0f));
                        if (b < 0.25)		nShade = '#';
                        else if (b < 0.5)	nShade = 'x';
                        else if (b < 0.75)	nShade = '.';
                        else if (b < 0.9)	nShade = '-';
                        else				nShade = ' ';
                        screen[y*nScreenWidth + x] = nShade;
                        if (fps_window_instance.run()) {
                            fps_window_instance.SetCategory(x, y, FPS_Window::FLOOR);
                            fps_window_instance.SetDistance(x, y, fDistanceToWall);
                        }
                    }
                }
            }

            int len = swprintf_s(screen, 40, L"X=%.2f, Y=%.2f, A=%.2f FPS=%.2f",
                player.fPlayerX, player.fPlayerY, player.fPlayerA, 1.0f / fElapsedTime);

            if (len >= 0){
                std::fill(screen + len, screen + nScreenWidth, L' ');
            }

            for (int nx = 0; nx < map.nMapWidth; nx++)
                for (int ny = 0; ny < map.nMapWidth; ny++){
                    screen[(ny + 1) * nScreenWidth + nx] = map.map[ny * map.nMapWidth + nx];
                }
            screen[(int)floor(player.fPlayerY + 1) * nScreenWidth + (int)floor(player.fPlayerX)] = 'P';

            empty_screen();
            renderConsole();

            fps_window_instance.render(&player, &map, scale);
        }

        delete[] screen;
    }
}