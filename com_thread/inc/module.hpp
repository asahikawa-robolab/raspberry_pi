#ifndef _MODULE_HPP_
#define _MODULE_HPP_

#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_thread.hpp"

/*-----------------------------------------------
 *
 * SwitchData
 *
-----------------------------------------------*/
class SwitchData
{
public:
    jibiki::ShareVal<uint8_t> m_push_l;
    jibiki::ShareVal<uint8_t> m_push_r;
    jibiki::ShareVal<uint8_t> m_toggle[7];
    jibiki::ShareVal<uint8_t> m_lu;
    jibiki::ShareVal<uint8_t> m_ld;
    jibiki::ShareVal<uint8_t> m_ru;
    jibiki::ShareVal<uint8_t> m_rd;
    jibiki::ShareVal<uint8_t> m_slide; /* スライドポテンショメータ */

public:
    /* 受信データをメンバにセットする */
    void set(uint8_t data1, uint8_t data2, uint8_t data3);
};

/*-----------------------------------------------
 *
 * ImuData
 *
-----------------------------------------------*/
class ImuData
{
public:
    void write_offset(double angle);      /* read の結果が angle になるようにオフセットを設定する */
    void write_raw_data(double raw_data); /* 生データを書き込む */
    double read(void);                    /* 角度データを読み出す */

public:
    ImuData(void)
    {
        m_raw_data = 0;
        m_offset = 0;
    }

private:
    jibiki::ShareVal<double> m_raw_data;
    jibiki::ShareVal<double> m_offset;
};
/* オフセットを設定する */
inline void ImuData::write_offset(double angle) { m_offset = angle - m_raw_data.read(); }
/* 生データを書き込む */
inline void ImuData::write_raw_data(double raw_data) { m_raw_data = raw_data; }
/* 角度データを読み出す */
inline double ImuData::read(void) { return m_raw_data.read() + m_offset.read(); }

/*-----------------------------------------------
 *
 * ControllerData
 *
-----------------------------------------------*/
class ControllerData
{
public:
    jibiki::ShareVal<bool> m_l_cross_l;
    jibiki::ShareVal<bool> m_l_cross_r;
    jibiki::ShareVal<bool> m_l_cross_u;
    jibiki::ShareVal<bool> m_l_cross_d;
    jibiki::ShareVal<bool> m_r_cross_l;
    jibiki::ShareVal<bool> m_r_cross_r;
    jibiki::ShareVal<bool> m_r_cross_u;
    jibiki::ShareVal<bool> m_r_cross_d;
    jibiki::ShareVal<bool> m_l_switch_d;
    jibiki::ShareVal<bool> m_l_switch_m;
    jibiki::ShareVal<bool> m_r_switch_d;
    jibiki::ShareVal<bool> m_r_switch_m;
    jibiki::ShareVal<bool> m_l_lever_l;
    jibiki::ShareVal<bool> m_l_lever_r;
    jibiki::ShareVal<bool> m_r_lever_l;
    jibiki::ShareVal<bool> m_r_lever_r;
    jibiki::ShareVal<uint8_t> m_l_analog_stick_h;
    jibiki::ShareVal<uint8_t> m_l_analog_stick_v;
    jibiki::ShareVal<uint8_t> m_r_analog_stick_h;
    jibiki::ShareVal<uint8_t> m_r_analog_stick_v;
    jibiki::ShareVal<bool> m_l_slide;
    jibiki::ShareVal<bool> m_r_slide;
    jibiki::ShareVal<bool> m_l_switch_u;
    jibiki::ShareVal<bool> m_r_switch_u;
    jibiki::ShareVal<bool> m_tact_lu;
    jibiki::ShareVal<bool> m_tact_mu;
    jibiki::ShareVal<bool> m_tact_ru;
    jibiki::ShareVal<bool> m_tact_ld;
    jibiki::ShareVal<bool> m_tact_md;
    jibiki::ShareVal<bool> m_tact_rd;

    void set(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4,
             uint8_t d5, uint8_t d6, uint8_t d7, uint8_t d8);
};
#endif