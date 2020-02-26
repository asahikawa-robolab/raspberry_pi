#ifndef MODULE_HPP
#define MODULE_HPP

#include <stdint.h>
#include "../../Share/inc/_std_func.hpp"
#include "_utility.hpp"

/*-----------------------------------------------
 *
 * Switch
 *
-----------------------------------------------*/
typedef enum
{
    FIELD_BLUE,
    FIELD_RED,
} Field;

/* 通信データ */
typedef struct
{
    union {
        struct
        {
            unsigned int start : 1;
            unsigned int reset : 1;
            unsigned int field : 1;
            unsigned int is_final : 1;
            unsigned int toggle1 : 1;
            unsigned int toggle2 : 1;
            unsigned int toggle3 : 1;
            unsigned int toggle4 : 1;
        };
        uint8_t d1;
    };
    union {
        struct
        {
            unsigned int toggle5 : 1;
            unsigned int lu : 1;
            unsigned int ld : 1;
            unsigned int ru : 1;
            unsigned int rd : 1;
        };
        uint8_t d2;
    };
    uint8_t d3; /* 可変抵抗 */
} SwitchData;

/*-----------------------------------------------
 *
 * Flags
 *
-----------------------------------------------*/
#define START true
#define STOP false
#define KILL_NONE 0
#define KILL_OK 1
#define NUM_FLAGS 5

typedef enum
{
    FLAG_EXIT,
    FLAG_START,
    FLAG_KEY,
    FLAG_OPERATE,
    FLAG_RESET,
} FLAG_ENUM;

/*-----------------------------------------------
 *
 * IMU
 *
-----------------------------------------------*/
class IMU
{
public:
    void write_offset(double offset);     /* オフセットを設定する */
    void write_raw_data(double raw_data); /* 生データを書き込む */
    double read_spin(void);               /* 角度データを読み出す */

public:
    IMU(void)
    {
        m_raw_data = 0;
        m_offset = 0;
    }

private:
    double m_raw_data;
    double m_offset;
};

inline void IMU::write_offset(double offset) { m_offset = offset; }         /* オフセットを設定する */
inline void IMU::write_raw_data(double raw_data) { m_raw_data = raw_data; } /* 生データを書き込む */
inline double IMU::read_spin(void) { return m_raw_data + m_offset; }        /* 角度データを読み出す */

/*-----------------------------------------------
 *
 * Chassis
 *
-----------------------------------------------*/
class Chassis
{
public:
    double m_speed;
    double m_theta;
    double m_spin;
    TurnMode m_turn_mode;

public:
    void stop(void);
    void calc(double target_rpm[]);

public:
    Chassis(void);

private:
    double calc_rotate(void);
};

/*-----------------------------------------------
 *
 * Controller
 *
-----------------------------------------------*/
typedef struct
{
    union {
        uint8_t d1;
        struct
        {
            unsigned int l_cross_l : 1;
            unsigned int l_cross_r : 1;
            unsigned int l_cross_u : 1;
            unsigned int l_cross_d : 1;
            unsigned int r_cross_l : 1;
            unsigned int r_cross_r : 1;
            unsigned int r_cross_u : 1;
            unsigned int r_cross_d : 1;
        };
    };
    union {
        uint8_t d2;
        struct
        {
            unsigned int l_switch_d : 1;
            unsigned int l_switch_m : 1;
            unsigned int r_switch_d : 1;
            unsigned int r_switch_m : 1;
            unsigned int l_lever_l : 1;
            unsigned int l_lever_r : 1;
            unsigned int r_lever_l : 1;
            unsigned int r_lever_r : 1;
        };
    };
    union {
        uint8_t d3;
        uint8_t l_analog_stick_h;
    };
    union {
        uint8_t d4;
        uint8_t l_analog_stick_v;
    };
    union {
        uint8_t d5;
        uint8_t r_analog_stick_h;
    };
    union {
        uint8_t d6;
        uint8_t r_analog_stick_v;
    };
    union {
        uint8_t d7;
        struct
        {
            unsigned int l_slide : 1;
            unsigned int r_slide : 5;
            unsigned int l_switch_u : 1;
            unsigned int r_switch_u : 1;
        };
    };
    union {
        uint8_t d8;
        struct
        {
            unsigned int tact_lu : 1;
            unsigned int tact_mu : 1;
            unsigned int tact_ru : 1;
            unsigned int tact_ld : 1;
            unsigned int tact_md : 1;
            unsigned int tact_rd : 1;
        };
    };
} Controller;

#endif