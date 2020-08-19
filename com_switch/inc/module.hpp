#ifndef _MODULE_HPP_
#define _MODULE_HPP_

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

#endif