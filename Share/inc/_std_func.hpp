/*-----------------------------------------------
 *
 * Last updated : 2020/02/05, 21:17
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _STDFUNC_HPP
#define _STDFUNC_HPP
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <sys/wait.h> /* wait */
#include <unordered_map>
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
 * AssocArray
 * 存在しないキーを指定したとき，値を追加しないで例外を投げる
 *
-----------------------------------------------*/
template <typename T>
class AssocArray
{
public:
    AssocArray(std::vector<std::string> keys);
    AssocArray(void) { m_tmp = T(); }
    T &operator[](std::string key);
    size_t size(void) { return m_map.size(); }
    void add(std::string key, T value = T()) { m_map[key] = value; }
    void erase(std::string key) { m_map.erase(key); }

private:
    std::unordered_map<std::string, T> m_map;
    T m_tmp;
};

template <typename T>
AssocArray<T>::AssocArray(std::vector<std::string> keys)
{
    for (auto i : keys)
        m_map[i] = T();
    m_tmp = T();
}

template <typename T>
T &AssocArray<T>::operator[](std::string key)
{
    if (m_map.empty())
        return m_tmp;

    auto itr = m_map.find(key);
    if (itr == m_map.end())
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << std::endl
             << "key [" << key << "] not found." << std::endl;
        throw sstr.str();
    }
    return m_map[key];
}

/*-----------------------------------------------
 *
 * マクロ
 *
-----------------------------------------------*/
/* smaller <= input < bigger を満たすかどうかを返す */
template <typename T>
inline bool BETWEEN(T smaller, T input, T bigger)
{
    return (smaller <= input) && (input < bigger);
}

/* 与えられた２つの時刻の差を返す */
inline double CALC_SEC(struct timespec s, struct timespec e)
{
    return (e.tv_sec - s.tv_sec + (e.tv_nsec - s.tv_nsec) * 1E-9);
}

/* 与えられた２点の距離を返す */
template <typename T>
inline double CALC_DIS(T a, T b)
{
    return (sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

/* start から end への角度を返す */
template <typename T>
inline double CALC_ANGLE(T start, T end)
{
    return atan2(end.y - start.y, end.x - start.x);
}

/* 引数で与えられた角度を -pi ~ pi の範囲に収める */
template <typename T>
inline double LIMIT_ANGLE(T angle)
{
    return atan2(sin(angle), cos(angle));
}

/* 与えられた数値の符号を返す */
template <typename T>
inline T GET_SIGNAL(T a)
{
    return a < 0 ? -1 : 1;
}

/* [deg] を [rad] に変換する */
template <typename T>
inline double DEG_RAD(T x)
{
    return ((x) / 180.0 * M_PI);
}

/* [rad] を [deg] に変換する */
template <typename T>
inline double RAD_DEG(T x)
{
    return ((x) / M_PI * 180.0);
}

/* 上位８bit を取り出す */
inline uint8_t UP(int16_t data)
{
    return (uint8_t)((data >> 8) & 0xFF);
}

/* 下位 8bit を取り出す */
inline uint8_t LOW(int16_t data)
{
    return (uint8_t)(data & 0xFF);
}

/* １バイトの２個のデータを１個の２バイトのデータにする */
inline int16_t ASBL(uint8_t upper, uint8_t lower)
{
    return ((int16_t)((upper) << 8) | (lower));
}

} // namespace jibiki
#endif