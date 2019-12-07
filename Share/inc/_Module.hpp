#ifndef _MODULE_HPP
#define _MODULE_HPP

#include <stdlib.h>
#include "_SharedMemory.hpp"
#include "_StdFunc.hpp"
#include "_Module.hpp"
#include "_Flags.hpp"

int my_stoi(const char src[][MAX_STRING_LENGTH], int src_num, const char *name); /* 文字列を要素数に変換する */

/*-----------------------------------------------
 *
 * MOTION
 *
-----------------------------------------------*/
class MOTION
{
public:
    MOTION(double speed0, double theta0)
    {
        speed = speed0;
        theta = theta0;
    }
    MOTION(void){};

    double speed, /* 速度 [rpm] */
        theta;    /* 移動方向 [rad] */
};

/*-----------------------------------------------
*
* ACTUATOR
*
-----------------------------------------------*/
#define CONVERGE_ZERO_POINT_ADJUSTING 0
#define CONVERGE_FALSE 1
#define CONVERGE_TRUE 2

#ifdef TEAM_A
/*-----------------------------------------------
A チーム
-----------------------------------------------*/
const char _Actuators[NUM_ACTUATOR][MAX_STRING_LENGTH] =
    {
        "FR",
        "FL",
        "BR",
        "BL",
        "elevator_low",
        "elevator_up",
        "t_shirt",
        "bath",
        "sheet_root",
        "sheet_tip",
};
const int _InitActuatorValue[NUM_ACTUATOR] =
    {
        0,     /* FR */
        0,     /* FL */
        0,     /* BR */
        0,     /* BL */
        -1000, /* elevator_low */
        -1000, /* elevator_up */
        0,     /* t_shirt */
        120,   /* bath */
        180,   /* sheet_root */
        79,    /* sheet_tip */
};
#endif
#ifdef TEAM_B
/*-----------------------------------------------
B チーム
-----------------------------------------------*/
const char _Actuators[NUM_ACTUATOR][MAX_STRING_LENGTH] =
    {
        "FR",
        "FL",
        "BR",
        "BL",
        "R_bath_up",
        "R_bath_low",
        "R_sheet",
        "R_clip",
        "L_bath_up",
        "L_bath_low",
        "L_sheet",
        "L_clip",
        "elevator_low",
        "motor0",
        "motor1",
};
const int _InitActuatorValue[NUM_ACTUATOR] =
    {
        0,   /* FR */
        0,   /* FL */
        0,   /* BR */
        0,   /* BL */
        180, /* R_bath_up */
        180,/* R_bath_low */
        0,   /* R_sheet */
        180, /* R_clip */
        0,   /* L_bath_up */
        0,   /* L_bath_low */
        0,   /* L_sheet */
        0,   /* L_clip */
        0,   /* elevator_low */
        0,   /* motor0 */
        0,   /* motor1 */

};
#endif

class ACTUATOR
{
public:
    ACTUATOR(void);
    ~ACTUATOR(void);
    void init(void);
    void set(const char *name0, int value0);
    int read(const char *name0);
    void print(const char *name0);
    int get_converge(const char *name0);
    void set_converge(const char *name0, int value);

private:
    int *value;    /* 目標値 */
    int *converge; /* 目標値に収束したかどうか */

    /* private 隠蔽 */
    ACTUATOR(const ACTUATOR &obj);
    const ACTUATOR &operator=(const ACTUATOR &obj);
};

/* 目標値を設定する */
inline void ACTUATOR::set(const char *name0, int value0)
{
    value[my_stoi(_Actuators, NUM_ACTUATOR, name0)] = value0;
}
/* 目標値を読み出す */
inline int ACTUATOR::read(const char *name0)
{
    return value[my_stoi(_Actuators, NUM_ACTUATOR, name0)];
}
/* 目標値を表示する */
inline void ACTUATOR::print(const char *name0)
{
    printf("%s(%d) : %d\n",
           name0,
           my_stoi(_Actuators, NUM_ACTUATOR, name0),
           value[my_stoi(_Actuators, NUM_ACTUATOR, name0)]);
}
/* 収束しているか返す */
inline int ACTUATOR::get_converge(const char *name0)
{
    return converge[my_stoi(_Actuators, NUM_ACTUATOR, name0)];
}

/* converge をセットする */
inline void ACTUATOR::set_converge(const char *name0, int value)
{
    converge[my_stoi(_Actuators, NUM_ACTUATOR, name0)] = value;
}

/*-----------------------------------------------
*
* LINE
*
-----------------------------------------------*/
const char _Lines[NUM_LINE][MAX_STRING_LENGTH] =
    {
        "front",
        "right",
        "left",
};

class LINE_BASIS
{
public:
    LINE_BASIS(const char *line_name0, bool inverse0 = false);

    char line_name[MAX_STRING_LENGTH];
    bool inverse;
};

inline LINE_BASIS::LINE_BASIS(const char *line_name0, bool inverse0)
{
    strcpy(line_name, line_name0);
    inverse = inverse0;
}

class LINE
{
public:
    LINE(void);
    ~LINE(void);
    void init(void);
    uint8_t read(const char *line_name, bool inverse, bool previous);
    uint8_t read(double theta0, bool previous = false);
    bool read(const char *line_name, int bit);
    bool read(double theta0, int bit);
    void write(const char *line_name, uint8_t value);
    void calc_corr(double theta, MOTION &motion, double gain); /* 補正値を計算（correction） */

private:
    uint8_t *v;
    uint8_t *pre_data; /* calc_corr */
    uint8_t flip(uint8_t in);
    std::vector<LINE_BASIS> bases;

    /* private 隠蔽 */
    LINE(const LINE &obj);
    const LINE &operator=(const LINE &obj);
};

inline void LINE::init(void)
{
    for (int i = 0; i < NUM_LINE; i++)
    {
        v[i] = 0;
        pre_data[i] = 0;
    }
}

inline uint8_t LINE::read(const char *line_name, bool inverse, bool previous)
{
    uint8_t condition = (inverse << 1) | previous;
    size_t index = my_stoi(_Lines, NUM_LINE, line_name);
    uint8_t data;

    switch (condition)
    {
    case 0b00:
        data = v[index];
        break;
    case 0b01:
        data = pre_data[index];
        break;
    case 0b10:
        data = flip(v[index]);
        break;
    case 0b11:
        data = flip(pre_data[index]);
        break;
    }

    return data;
}

inline bool LINE::read(double theta0, int bit)
{
    uint8_t data = read(theta0);

    if (BETWEEN(0, bit, 8) == true)
    {
        return GET_BIT(data, bit);
    }
    else
    {
        PrintError("LINE::read()", "invalid bit", bit);
        _Flags.e(EXIT_FAILURE);
    }
}

inline void LINE::write(const char *line_name, uint8_t value)
{
    size_t index = my_stoi(_Lines, NUM_LINE, line_name);

    /* pre_data を更新 */
    if (v[index] != 0)
    {
        pre_data[index] = v[index];
    }

    v[index] = flip(value);
}

inline uint8_t LINE::flip(uint8_t in)
{
    /* 左右反転 */
    uint8_t out = 0;
    for (size_t i = 0; i < 8; i++)
    {
        out += (in >> i) % 2 * pow(2, 7 - i);
    }
    return out;
}

/*-----------------------------------------------
*
* LIMIT
*
-----------------------------------------------*/
#ifdef TEAM_A
const char _Limits[NUM_LIMIT][MAX_STRING_LENGTH] =
    {
        "front",
        "right",
        "left",
        "bath1",
        "bath2",
};
#endif
#ifdef TEAM_B
const char _Limits[NUM_LIMIT][MAX_STRING_LENGTH] =
    {
        "lim_R_S",
        "lim_L_S",
        "lim_R_B_low",
        "lim_right",
        "lim_R_B_up",
        "lim_left",
        "lim_front",
};
#endif

class LIMIT
{
public:
    LIMIT(void);
    ~LIMIT(void);
    void init(void);
    bool read(const char *limit_name);
    void write(const char *limit_name, uint8_t data, size_t bit);
    void print(void);

private:
    bool *v;

    /* private 隠蔽 */
    LIMIT(const LIMIT &obj);
    const LIMIT &operator=(const LIMIT &obj);
};

inline void LIMIT::init(void)
{
    for (int i = 0; i < NUM_LIMIT; i++)
    {
        v[i] = false;
    }
}

inline bool LIMIT::read(const char *limit_name)
{
    return v[my_stoi(_Limits, NUM_LIMIT, limit_name)];
}

inline void LIMIT::write(const char *limit_name, uint8_t data, size_t bit)
{
    v[my_stoi(_Limits, NUM_LIMIT, limit_name)] = GET_BIT(data, bit);
}

inline void LIMIT::print(void)
{
    printf("Limit ");
    for (int i = 0; i < 8; i++)
    {
        printf("%d, ", v[i]);
    }
    printf("\n");
}

/*-----------------------------------------------
*
* ODOMETRY
*
-----------------------------------------------*/
class ODOMETRY
{
public:
    ODOMETRY(void);
    ~ODOMETRY(void);
    void init(void);
    cv::Point2f *v;

private:
    /* private 隠蔽 */
    ODOMETRY(const ODOMETRY &obj);
    const ODOMETRY &operator=(const ODOMETRY &obj);
};

inline void ODOMETRY::init(void)
{
    *v = cv::Point2f(0, 0);
}

/*-----------------------------------------------
 *
 * CHASSIS
 *
-----------------------------------------------*/
class CHASSIS
{
public:
    CHASSIS(void);
    ~CHASSIS(void);
    void init(void);
    void set(MOTION motion0); /* 引数の MOTION から rpm を決定 */
    void set(void);
    void stop(void);                             /* モータを停止 */
    double read_spin(void);                      /* spin を読み出す */
    void write_spin(double spin0);               /* spin を書き込む */
    double read_spin_offset(void);               /* spin_offset を読み出す */
    void write_spin_offset(double spin_offset0); /* spin_offset を書き込む */
    MOTION read_motion(void);                    /* motion を読み出す */
    double get_spin_dif(void);                   /* spin の偏差を返す */
    double get_target_spin(void);                /* target_spin を取得する */
    void set_target_spin(double target_spin0,
                         CALC_ANGLE_MODE spin_mode0 = MODE_SHORTEST); /* spin の目標値を設定する */

private:
    /* 共有メモリ */
    double *spin;        /* IMU から得られた回転角 [rad] */
    double *target_spin; /* spin の目標値 */
    double *spin_offset; /* spin のオフセット */
    MOTION *motion;      /* ProcessDisplay() で表示するため */

    CALC_ANGLE_MODE spin_mode; /* 旋回方向 */

    int calc_rotate(void); /* 回転制御のための rotate を計算する */

    /* private 隠蔽 */
    const CHASSIS &operator=(const CHASSIS &obj);
    CHASSIS(const CHASSIS &obj);
};

inline void CHASSIS::init(void)
{
    *spin = 0;
    *target_spin = 0;
    *spin_offset = 0;
    *motion = MOTION(0, 0);
}
inline double CHASSIS::read_spin(void)
{
    return *spin;
}
inline void CHASSIS::write_spin(double spin0)
{
    *spin = *spin_offset + spin0;
}
inline double CHASSIS::read_spin_offset(void)
{
    return *spin_offset;
}
inline void CHASSIS::write_spin_offset(double spin_offset0)
{
    *spin_offset = spin_offset0;
}
inline MOTION CHASSIS::read_motion(void)
{
    return *motion;
}
inline double CHASSIS::get_spin_dif(void)
{
    return CALC_ANGLE_DIF(*target_spin, *spin, spin_mode);
}
inline double CHASSIS::get_target_spin(void)
{
    return *target_spin;
}
inline void CHASSIS::set_target_spin(double target_spin0, CALC_ANGLE_MODE spin_mode0)
{
    *target_spin = target_spin0;
    spin_mode = spin_mode0;
    printf("%d\n", spin_mode);
}

/*-----------------------------------------------
 *
 * SWITCH
 *
-----------------------------------------------*/
#define FIELD_BLUE 0
#define FIELD_RED 1
/* SWITCH::once() 用 */
typedef enum
{
    ONCE_START,
    ONCE_RESET,
    ONCE_LU,
    ONCE_LD,
    ONCE_RU,
    ONCE_RD,
} SWITCH_ONCE;

/* 通信データ */
typedef struct
{
    union {
        struct
        {
            unsigned int start : 1;
            unsigned int reset : 1;
            unsigned int field : 1;
            unsigned int isfinal : 1;
            unsigned int collect : 1;
            unsigned int bath1 : 1;
            unsigned int bath2 : 1;
            unsigned int sheet : 1;
        };
        uint8_t d1;
    };
    union {
        struct
        {
            unsigned int program : 1;
            unsigned int lu : 1;
            unsigned int ld : 1;
            unsigned int ru : 1;
            unsigned int rd : 1;
        };
        uint8_t d2;
    };
    uint8_t d3; /* 可変抵抗 */
} SWITCH_DATA;

class SWITCH
{
public:
    SWITCH_DATA *d;

    SWITCH(void);
    ~SWITCH(void);
    void init(void);
    bool once(SWITCH_ONCE name, bool data); /* ボタンが押された瞬間だけ true を返す */
    void print(void);                       /* 表示 */

private:
    int button_cnt[6]; /* once() 用 */

    /* private 隠蔽 */
    SWITCH(const SWITCH &obj);
    const SWITCH &operator=(const SWITCH &obj);
};

inline SWITCH::SWITCH(void)
{
    /* 共有メモリ */
    d = (SWITCH_DATA *)shmOpen(SHM_SWITCH,
                               sizeof(SWITCH_DATA),
                               SHM_NAME_SWITCH);
}

inline SWITCH::~SWITCH(void)
{
    shmClose(SHM_SWITCH);
}

inline void SWITCH::init(void)
{
    d->d1 = 0;
    d->d2 = 0;
    d->d3 = 0;
}

inline void SWITCH::print(void)
{
    printf("st %d, re %d, f %d, isf %d, c %d, b1 %d, b2 %d, sh %d, p %d, lu %d, ld %d, ru %d, rd %d\n",
           d->start, d->reset, d->field, d->isfinal, d->collect, d->bath1, d->bath2, d->sheet,
           d->program, d->lu, d->ld, d->ru, d->rd);
}

#endif