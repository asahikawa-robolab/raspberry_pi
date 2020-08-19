#include "../inc/module.hpp"

/*-----------------------------------------------
 *
 * ControllerData
 *
-----------------------------------------------*/
void ControllerData::set(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4,
                         uint8_t d5, uint8_t d6, uint8_t d7, uint8_t d8)
{
    m_l_cross_l = (d1 >> 0) & 0b1;
    m_l_cross_r = (d1 >> 1) & 0b1;
    m_l_cross_u = (d1 >> 2) & 0b1;
    m_l_cross_d = (d1 >> 3) & 0b1;
    m_r_cross_l = (d1 >> 4) & 0b1;
    m_r_cross_r = (d1 >> 5) & 0b1;
    m_r_cross_u = (d1 >> 6) & 0b1;
    m_r_cross_d = (d1 >> 7) & 0b1;
    m_l_switch_d = (d2 >> 0) & 0b1;
    m_l_switch_m = (d2 >> 1) & 0b1;
    m_r_switch_d = (d2 >> 2) & 0b1;
    m_r_switch_m = (d2 >> 3) & 0b1;
    m_l_lever_l = (d2 >> 4) & 0b1;
    m_l_lever_r = (d2 >> 5) & 0b1;
    m_r_lever_l = (d2 >> 6) & 0b1;
    m_r_lever_r = (d2 >> 7) & 0b1;
    m_l_analog_stick_h = d3;
    m_l_analog_stick_v = d4;
    m_r_analog_stick_h = d5;
    m_r_analog_stick_v = d6;
    m_l_slide = (d7 >> 0) & 0b1;
    m_r_slide = (d7 >> 1) & 0b1;
    m_l_switch_u = (d7 >> 6) & 0b1;
    m_r_switch_u = (d7 >> 7) & 0b1;
    m_tact_lu = (d8 >> 0) & 0b1;
    m_tact_mu = (d8 >> 1) & 0b1;
    m_tact_ru = (d8 >> 2) & 0b1;
    m_tact_ld = (d8 >> 3) & 0b1;
    m_tact_md = (d8 >> 4) & 0b1;
    m_tact_rd = (d8 >> 5) & 0b1;
}