/* Last updated : 2020/10/06, 00:38 */
#ifndef _STDFUNC_HPP
#define _STDFUNC_HPP
#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include "_picojson.hpp"

namespace jibiki
{
    int kbhit(void) noexcept;                                            /* キー入力 */
    std::vector<std::string> split(const std::string &str, char sep);    /* 文字列を sep で分割 */
    picojson::value load_json_file(std::string path);                    /* JSON ファイルを読み込む */
    void print_err(const char *func, std::string err_msg = "") noexcept; /* エラーメッセージを表示 */
    using time_point = std::chrono::system_clock::time_point;            /* get_time で使用する型 */

    /* 時間を取得 */
    inline time_point get_time(void) noexcept
    {
        return std::chrono::system_clock::now();
    }

    /* 時間差を計算 */
    double calc_sec(time_point start, time_point end = get_time());

    /* スリープ */
    inline void usleep(size_t microseconds)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    }

    /* smaller <= input < bigger を満たすかどうかを返す */
    template <typename T>
    inline bool between(T smaller, T input, T bigger) noexcept
    {
        return (smaller <= input) && (input < bigger);
    }

    /* smaller <= input <= bigger を満たすかどうかを返す */
    template <typename T>
    inline bool between2(T smaller, T input, T bigger) noexcept
    {
        return (smaller <= input) && (input <= bigger);
    }

    /* 与えられた２点の距離を返す */
    template <typename T>
    inline double calc_dist(T a, T b)
    {
        return (std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2)));
    }

    /* start から end への角度を返す */
    template <typename T>
    inline double calc_angle(T start, T end)
    {
        return std::atan2(end.y - start.y, end.x - start.x);
    }

    /* 引数で与えられた角度を -pi ~ pi の範囲に収める */
    template <typename T>
    inline double limit_angle(T angle)
    {
        return std::atan2(sin(angle), cos(angle));
    }

    /* 与えられた数値の符号を返す */
    template <typename T>
    inline T get_signal(T a) noexcept
    {
        if (a == 0)
            return 0;
        else if (a > 0)
            return 1;
        else
            return -1;
    }

    /* [deg] を [rad] に変換する */
    template <typename T>
    inline double deg_rad(T x) noexcept
    {
        return (x / 180.0 * M_PI);
    }

    /* [rad] を [deg] に変換する */
    template <typename T>
    inline double rad_deg(T x) noexcept
    {
        return (x / M_PI * 180.0);
    }

    /* 上位８bit を取り出す */
    inline uint8_t up(int16_t data) noexcept
    {
        return (uint8_t)((data >> 8) & 0xFF);
    }

    /* 下位 8bit を取り出す */
    inline uint8_t low(int16_t data) noexcept
    {
        return (uint8_t)(data & 0xFF);
    }

    /* １バイトの２個のデータを１個の２バイトのデータにする */
    inline int16_t asbl(uint8_t upper, uint8_t lower) noexcept
    {
        return ((int16_t)((upper) << 8) | (lower));
    }

} // namespace jibiki
#endif