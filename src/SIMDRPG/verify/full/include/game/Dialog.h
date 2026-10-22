#pragma once
#include <cstring>
#include <string>
#include <vector>

namespace game {

struct Line {
    const char* who;        // 说话人
    const char* text;       // 内容
    const char* faceset;    // 头像（可空）
};

struct DialogState {
    const std::vector<Line>* lines = nullptr;
    int   index = 0;
    float chars_shown = 0.f;
    float speed = 40.f;      // 每秒显示多少字
    bool  active = false;

    void start(const std::vector<Line>& l) {
        lines = &l;
        index = 0;
        chars_shown = 0.f;
        active = true;
    }

    const Line& current() const { return (*lines)[size_t(index)]; }

    int text_len() const { return int(std::strlen(current().text)); }

    bool line_done() const { return chars_shown >= float(text_len()); }

    // 返回 true 表示"整段对话结束"
    bool advance() {
        if (!line_done()) {                 // 还没打完：先一次性显示完
            chars_shown = float(text_len());
            return false;
        }
        ++index;
        chars_shown = 0.f;
        if (index >= int(lines->size())) {
            active = false;
            return true;
        }
        return false;
    }

    void update(float dt) {
        if (!active) return;
        chars_shown += speed * dt;
        const float n = float(text_len());
        if (chars_shown > n) chars_shown = n;
    }
};

// ---------------------------------------------------------------
// 对话表
//
// ⚠ 必须是英文。
// gfx::draw_text 只认 ASCII（idx = 字节 - 32，超出 0..94 就跳过），
// 写中文进去不会报错，但游戏里是**一片空白** —— 只看到对话框没有人话。
// 这个坑我踩过：台词全是中文，跑起来一句都看不见，还以为是对话没触发。
//
// 要中文得自己做一张汉字点阵图集 + UTF-8 解码，见 15.6。
// ---------------------------------------------------------------
inline const std::vector<Line> kElderIntro = {
    {"Maelor", "You slept three days.", nullptr},
    {"Maelor", "The thing below is not hungry. It is spreading.", nullptr},
    {"Aric",   "Is the Tree still alive?", nullptr},
    {"Maelor", "Alive. But it needs four relics to seal the crack.", nullptr},
    {"Maelor", "The Heartseed is with me. The rest are in the mountains,", nullptr},
    {"Maelor", "in the crypt, and further out than that.", nullptr},
    {"Aric",   "Then we start with the mountains.", nullptr},
};

inline const std::vector<Line> kSmithSword = {
    {"Brann", "A sword? Take it. Dont thank me.", nullptr},
    {"Brann", "Come back when it dulls. Dont die out there.", nullptr},
};

inline const std::vector<Line> kMerchantShop = {
    {"Sela", "Life or coin? Dont answer. I know.", nullptr},
};

inline const std::vector<Line> kBossWarning = {
    {"Aric", "This thing... is bigger than the stories.", nullptr},
};

// ---------------------------------------------------------------
// 路牌：按地图返回不同碑文。
// 这类东西花不了几行代码，却能让玩家知道"我在哪、该往哪走"。
// ---------------------------------------------------------------
inline const std::vector<Line>& sign_lines(const std::string& map) {
    static const std::vector<Line> kVillage = {
        {"Signpost", "EAST  >  Whispering Wood", nullptr},
        {"Signpost", "The wood takes what it likes.", nullptr},
    };
    static const std::vector<Line> kForest = {
        {"Signpost", "<  WEST   Dawnhollow", nullptr},
        {"Signpost", "EAST   Frostbite Pass  >", nullptr},
        {"Signpost", "Someone scratched: DONT SLEEP HERE", nullptr},
    };
    static const std::vector<Line> kPass = {
        {"Signpost", "<  WEST   Whispering Wood", nullptr},
        {"Signpost", "EAST   The Ashen Crypt  >", nullptr},
        {"Signpost", "The ice is thinner than it looks.", nullptr},
    };
    static const std::vector<Line> kCrypt = {
        {"Carving", "HERE LIE THE ONES WHO STAYED", nullptr},
        {"Carving", "THE ROOTS REMEMBER THEIR NAMES", nullptr},
    };
    static const std::vector<Line> kNest = {
        {"Carving", "Something old sleeps under this soil.", nullptr},
        {"Carving", "It has been fed. Do not wake it.", nullptr},
    };
    static const std::vector<Line> kRift = {
        {"Echo", "This is not your sky.", nullptr},
        {"Echo", "Go back while the door is still a door.", nullptr},
    };

    if (map == "village") return kVillage;
    if (map == "forest")  return kForest;
    if (map == "pass")    return kPass;
    if (map == "crypt")   return kCrypt;
    if (map == "nest")    return kNest;
    return kRift;
}

} // namespace game
