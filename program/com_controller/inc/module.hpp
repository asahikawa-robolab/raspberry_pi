#ifndef _MODULE_HPP_
#define _MODULE_HPP_

#include "../../share/inc/_thread.hpp"

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