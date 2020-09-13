#ifndef _MODULE_HPP_
#define _MODULE_HPP_

#include "../../share/inc/_thread.hpp"

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

#endif