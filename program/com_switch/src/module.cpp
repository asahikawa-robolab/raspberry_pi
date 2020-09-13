#include "../../share/inc/_std_func.hpp"
#include "../inc/module.hpp"

/*-----------------------------------------------
 *
 * SwitchData
 *
-----------------------------------------------*/
/* 受信データをメンバにセットする */
void SwitchData::set(uint8_t data1, uint8_t data2, uint8_t data3)
{
    m_push_l = (data1 >> 0) & 0b1;
    m_push_r = (data1 >> 1) & 0b1;
    m_toggle[0] = (data1 >> 2) & 0b1;
    m_toggle[1] = (data1 >> 3) & 0b1;
    m_toggle[2] = (data1 >> 4) & 0b1;
    m_toggle[3] = (data1 >> 5) & 0b1;
    m_toggle[4] = (data1 >> 6) & 0b1;
    m_toggle[5] = (data1 >> 7) & 0b1;
    m_toggle[6] = (data2 >> 0) & 0b1;
    m_lu = (data2 >> 1) & 0b1;
    m_ld = (data2 >> 2) & 0b1;
    m_ru = (data2 >> 3) & 0b1;
    m_rd = (data2 >> 4) & 0b1;
    m_slide = data3;
}