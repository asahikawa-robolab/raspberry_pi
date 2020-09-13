/*-----------------------------------------------
 *
 * Last updated : 2020/08/23, 16:03
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _STDFUNC_HPP
#define _STDFUNC_HPP
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <sys/wait.h> /* wait */
#include "_picojson.hpp"

namespace jibiki
{
    /*-----------------------------------------------
     *
     * プロトタイプ宣言
     *
    -----------------------------------------------*/
    int kbhit(void);
    struct timespec get_time(void);
    std::vector<std::string> split(const std::string &str, char sep);
    picojson::value load_json_file(std::string path);

    /*-----------------------------------------------
     *
     * マクロ
     *
    -----------------------------------------------*/
    /* smaller <= input < bigger を満たすかどうかを返す */
    template <typename T>
    inline bool between(T smaller, T input, T bigger)
    {
        return (smaller <= input) && (input < bigger);
    }

    /* 与えられた２つの時刻の差を返す */
    inline double calc_sec(struct timespec s, struct timespec e)
    {
        return (e.tv_sec - s.tv_sec + (e.tv_nsec - s.tv_nsec) * 1E-9);
    }

    /* 与えられた２点の距離を返す */
    template <typename T>
    inline double calc_dis(T a, T b)
    {
        return (sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
    }

    /* start から end への角度を返す */
    template <typename T>
    inline double calc_angle(T start, T end)
    {
        return atan2(end.y - start.y, end.x - start.x);
    }

    /* 引数で与えられた角度を -pi ~ pi の範囲に収める */
    template <typename T>
    inline double limit_angle(T angle)
    {
        return atan2(sin(angle), cos(angle));
    }

    /* 与えられた数値の符号を返す */
    template <typename T>
    inline T get_signal(T a)
    {
        return a < 0 ? -1 : 1;
    }

    /* [deg] を [rad] に変換する */
    template <typename T>
    inline double deg_rad(T x)
    {
        return ((x) / 180.0 * M_PI);
    }

    /* [rad] を [deg] に変換する */
    template <typename T>
    inline double rad_deg(T x)
    {
        return ((x) / M_PI * 180.0);
    }

    /* 上位８bit を取り出す */
    inline uint8_t up(int16_t data)
    {
        return (uint8_t)((data >> 8) & 0xFF);
    }

    /* 下位 8bit を取り出す */
    inline uint8_t low(int16_t data)
    {
        return (uint8_t)(data & 0xFF);
    }

    /* １バイトの２個のデータを１個の２バイトのデータにする */
    inline int16_t asbl(uint8_t upper, uint8_t lower)
    {
        return ((int16_t)((upper) << 8) | (lower));
    }

} // namespace jibiki
#endif