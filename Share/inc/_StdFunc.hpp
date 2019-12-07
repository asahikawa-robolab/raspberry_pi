#ifndef _STDFUNC_HPP
#define _STDFUNC_HPP
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>

/*-----------------------------------------------
 *
 * マクロ
 *
-----------------------------------------------*/
/*-----------------------------------------------
smaller <= input < bigger を満たすかどうかを返す
-----------------------------------------------*/
#define BETWEEN(smaller, input, bigger) ((smaller <= input) && (input < bigger))
/*-----------------------------------------------
与えられた２つの時刻の差を返す
-----------------------------------------------*/
#define CALC_SEC(s, e) (e.tv_sec - s.tv_sec + (e.tv_nsec - s.tv_nsec) * 1E-9)
/*-----------------------------------------------
与えられた２点の距離を返す
-----------------------------------------------*/
#define CALC_DIS(a, b) (sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)))
/*-----------------------------------------------
start から end への角度を返す
-----------------------------------------------*/
inline double CALC_ANGLE(cv::Point2f start, cv::Point2f end)
{
    return atan2(end.y - start.y, end.x - start.x);
}
/*-----------------------------------------------
引数で与えられた角度を -pi ~ pi の範囲に収める
-----------------------------------------------*/
inline double LIMIT_ANGLE(double angle)
{
    return atan2(sin(angle), cos(angle));
}
/*-----------------------------------------------
-pi ~ pi の不連続部に影響されない角度偏差（subed - sub）を返す
-----------------------------------------------*/
typedef enum
{
    MODE_CW,       /* 時計回り */
    MODE_CCW,      /* 反時計回り */
    MODE_SHORTEST, /* 最短方向 */
} CALC_ANGLE_MODE;

inline double CALC_ANGLE_DIF(double subed, double sub, CALC_ANGLE_MODE mode = MODE_SHORTEST)
{
    /* 角度を -pi ~ pi にする */
    double subed_limited = LIMIT_ANGLE(subed);
    double sub_limited = LIMIT_ANGLE(sub);
    double angle_dif = 0;

    switch (mode)
    {
    case MODE_CCW:
        if (subed_limited - sub_limited >= 0)
        {
            angle_dif = subed_limited - sub_limited;
        }
        else
        {
            angle_dif = subed_limited - sub_limited + 2 * M_PI;
        }
        break;
    case MODE_CW:
        if (subed_limited - sub_limited > 0)
        {
            angle_dif = subed_limited - sub_limited - 2 * M_PI;
        }
        else
        {
            angle_dif = subed_limited - sub_limited;
        }
        break;
    case MODE_SHORTEST:
        if (subed_limited - sub_limited > M_PI)
        {
            angle_dif = (subed_limited - sub_limited) - 2 * M_PI;
        }
        else if (subed_limited - sub_limited < -M_PI)
        {
            angle_dif = (subed_limited - sub_limited) + 2 * M_PI;
        }
        else
        {
            angle_dif = subed_limited - sub_limited;
        }
        break;
    }

    // printf("%.0lf, %d\n", angle_dif * 180 / M_PI, mode);
    return angle_dif;
}
/*-----------------------------------------------
与えられた数値の符号を返す
-----------------------------------------------*/
template <typename TYPE>
inline TYPE GET_SIGNAL(TYPE a)
{
    return a < 0 ? -1 : 1;
}
/*-----------------------------------------------
[deg] を [rad] に変換する
-----------------------------------------------*/
#define DEG_RAD(x) ((x) / 180.0 * M_PI)
/*-----------------------------------------------
[rad] を [deg] に変換する
-----------------------------------------------*/
#define RAD_DEG(x) ((x) / M_PI * 180.0)

/*-----------------------------------------------
与えられた１バイトの値から指定されたビットのデータを返す(0bit～7bit)
-----------------------------------------------*/
inline bool GET_BIT(uint8_t data, int bit)
{
    return (data >> bit) & 0b00000001;
}

/*-----------------------------------------------
与えられた１バイトの値から上位４ビットの値を返す
-----------------------------------------------*/
inline uint8_t GET_UP(uint8_t data)
{
    return (data >> 4) & 0x0F;
}

/*-----------------------------------------------
与えられた１バイトの値から下位４ビットの値を返す
-----------------------------------------------*/
inline uint8_t GET_LOW(uint8_t data)
{
    return data & 0x0F;
}
/*-----------------------------------------------
直線補間用の１次関数を返す
-----------------------------------------------*/
inline double LINER(double sx, double sy, double ex, double ey, double x)
{
    return (ey - sy) / (ex - sx) * (x - sx) + sy;
}

/*-----------------------------------------------
 *
 * Define
 *
-----------------------------------------------*/
#define MAX_STRING_LENGTH 100

void PrintMsg(const char *str, bool compact = false);
void PrintError(const char *place, const char *content);
void PrintError(const char *place, const char *content, double num);
void PrintError(const char *place, const char *content, char *str);
void PrintError(const char *place, const char *content, const char *str);
FILE *Myfopen(const char *path, const char *mode);
int kbhit(void);
struct timespec GetTime(void);

#endif