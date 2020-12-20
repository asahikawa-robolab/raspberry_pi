/* Last updated : 2020/10/05, 19:36 */
#ifndef MODULE_HPP
#define MODULE_HPP
#include "_thread.hpp"
#include "_serial_communication.hpp"

/*-----------------------------------------------
 *
 * SwitchData
 *
-----------------------------------------------*/
class SwitchData
{
private:
    static const std::size_t m_TOGGLE_NUM = 7;
    jibiki::ShareVar<uint8_t> m_push_l;
    jibiki::ShareVar<uint8_t> m_push_r;
    jibiki::ShareVar<uint8_t> m_toggle[m_TOGGLE_NUM];
    jibiki::ShareVar<uint8_t> m_lu;
    jibiki::ShareVar<uint8_t> m_ld;
    jibiki::ShareVar<uint8_t> m_ru;
    jibiki::ShareVar<uint8_t> m_rd;
    jibiki::ShareVar<uint8_t> m_slide; /* スライドポテンショメータ */

public:
    void set(jibiki::ParamCom &com); /* 受信データをメンバにセットする */
    uint8_t push_l(void) { return m_push_l.read(); }
    uint8_t push_r(void) { return m_push_r.read(); }
    uint8_t toggle(std::size_t index);
    uint8_t lu(void) { return m_lu.read(); }
    uint8_t ld(void) { return m_ld.read(); }
    uint8_t ru(void) { return m_ru.read(); }
    uint8_t rd(void) { return m_rd.read(); }
    uint8_t slide(void) { return m_slide.read(); }
};

inline uint8_t SwitchData::toggle(std::size_t index)
{
    /* 範囲チェック */
    if (m_TOGGLE_NUM <= index)
    {
        jibiki::print_err(__PRETTY_FUNCTION__, "out_of_range");
        throw std::out_of_range(""); /* エラー発生 */
    }
    return m_toggle[index].read();
}

/*-----------------------------------------------
 *
 * Imu
 *
-----------------------------------------------*/
class Imu
{
private:
    jibiki::ShareVar<double> m_raw_data;
    jibiki::ShareVar<double> m_offset;

public:
    Imu(void) noexcept : m_raw_data(0), m_offset(0) {}
    void write_offset(double angle);      /* read の結果が angle になるようにオフセットを設定する */
    void write_raw_data(double raw_data); /* 生データを書き込む */
    double read(void);                    /* 角度データを読み出す */
};
inline void Imu::write_offset(double angle) { m_offset = angle - m_raw_data.read(); }
inline void Imu::write_raw_data(double raw_data) { m_raw_data = raw_data; }
inline double Imu::read(void) { return m_raw_data.read() + m_offset.read(); }

/*-----------------------------------------------
 *
 * Controller
 *
-----------------------------------------------*/
class Controller
{
public:
    typedef enum /* 左スティックか右スティックか */
    {
        MODE_L, /* 左スティック */
        MODE_R, /* 右スティック */
    } Mode;
    typedef enum /* 方向の分割数 */
    {
        DIR_INF, /* 全方向 */
        DIR_8,   /* ８方向 */
        DIR_4,   /* ４方向 */
    } DirNum;

private: /* コンストラクタでしか変更操作が行われないため排他制御不要 */
    size_t m_calc_period_ms;

private:
    jibiki::ShareVar<double> m_speed;            /* スティックの倒し具合 */
    jibiki::ShareVar<double> m_theta[2];         /* スティックを倒している向き */
    jibiki::ShareVar<jibiki::time_point> m_time; /* 実行周期の管理 */

    jibiki::ShareVar<bool> m_l_cross_l;
    jibiki::ShareVar<bool> m_l_cross_r;
    jibiki::ShareVar<bool> m_l_cross_u;
    jibiki::ShareVar<bool> m_l_cross_d;
    jibiki::ShareVar<bool> m_r_cross_l;
    jibiki::ShareVar<bool> m_r_cross_r;
    jibiki::ShareVar<bool> m_r_cross_u;
    jibiki::ShareVar<bool> m_r_cross_d;
    jibiki::ShareVar<bool> m_l_switch_d;
    jibiki::ShareVar<bool> m_l_switch_m;
    jibiki::ShareVar<bool> m_r_switch_d;
    jibiki::ShareVar<bool> m_r_switch_m;
    jibiki::ShareVar<bool> m_l_lever_l;
    jibiki::ShareVar<bool> m_l_lever_r;
    jibiki::ShareVar<bool> m_r_lever_l;
    jibiki::ShareVar<bool> m_r_lever_r;
    jibiki::ShareVar<uint8_t> m_l_analog_stick_h;
    jibiki::ShareVar<uint8_t> m_l_analog_stick_v;
    jibiki::ShareVar<uint8_t> m_r_analog_stick_h;
    jibiki::ShareVar<uint8_t> m_r_analog_stick_v;
    jibiki::ShareVar<bool> m_l_slide;
    jibiki::ShareVar<bool> m_r_slide;
    jibiki::ShareVar<bool> m_l_switch_u;
    jibiki::ShareVar<bool> m_r_switch_u;
    jibiki::ShareVar<bool> m_tact_lu;
    jibiki::ShareVar<bool> m_tact_mu;
    jibiki::ShareVar<bool> m_tact_ru;
    jibiki::ShareVar<bool> m_tact_ld;
    jibiki::ShareVar<bool> m_tact_md;
    jibiki::ShareVar<bool> m_tact_rd;

private:
    double my_atan(double y, double x, DirNum dir_num) const;
    void convt(Mode mode, DirNum dir_num); /* アナログスティックのデータを大きさ，向きに変換 */

public:
    Controller(std::string json_path = "setting.json");
    void set(jibiki::ParamCom &com);
    bool l_cross_l(void) { return m_l_cross_l.read(); }
    bool l_cross_r(void) { return m_l_cross_r.read(); }
    bool l_cross_u(void) { return m_l_cross_u.read(); }
    bool l_cross_d(void) { return m_l_cross_d.read(); }
    bool r_cross_l(void) { return m_r_cross_l.read(); }
    bool r_cross_r(void) { return m_r_cross_r.read(); }
    bool r_cross_u(void) { return m_r_cross_u.read(); }
    bool r_cross_d(void) { return m_r_cross_d.read(); }
    bool l_switch_d(void) { return m_l_switch_d.read(); }
    bool l_switch_m(void) { return m_l_switch_m.read(); }
    bool r_switch_d(void) { return m_r_switch_d.read(); }
    bool r_switch_m(void) { return m_r_switch_m.read(); }
    bool l_lever_l(void) { return m_l_lever_l.read(); }
    bool l_lever_r(void) { return m_l_lever_r.read(); }
    bool r_lever_l(void) { return m_r_lever_l.read(); }
    bool r_lever_r(void) { return m_r_lever_r.read(); }
    uint8_t l_analog_stick_h(void) { return m_l_analog_stick_h.read(); }
    uint8_t l_analog_stick_v(void) { return m_l_analog_stick_v.read(); }
    uint8_t r_analog_stick_h(void) { return m_r_analog_stick_h.read(); }
    uint8_t r_analog_stick_v(void) { return m_r_analog_stick_v.read(); }
    bool l_slide(void) { return m_l_slide.read(); }
    bool r_slide(void) { return m_r_slide.read(); }
    bool l_switch_u(void) { return m_l_switch_u.read(); }
    bool r_switch_u(void) { return m_r_switch_u.read(); }
    bool tact_lu(void) { return m_tact_lu.read(); }
    bool tact_mu(void) { return m_tact_mu.read(); }
    bool tact_ru(void) { return m_tact_ru.read(); }
    bool tact_ld(void) { return m_tact_ld.read(); }
    bool tact_md(void) { return m_tact_md.read(); }
    bool tact_rd(void) { return m_tact_rd.read(); }
    /* アナログスティック */
    double speed(Mode mode, DirNum dir_num);
    double theta(Mode mode, DirNum dir_num);
};
inline double Controller::speed(Mode mode, DirNum dir_num)
{
    convt(mode, dir_num);
    return m_speed.read();
}
inline double Controller::theta(Mode mode, DirNum dir_num)
{
    convt(mode, dir_num);
    return m_theta[mode].read();
}

/*-----------------------------------------------
 *
 * Chassis（動作未確認）
 *
-----------------------------------------------*/
class Chassis
{
public:
    typedef enum
    {
        TURN_CW,       /* 時計回り */
        TURN_CCW,      /* 反時計回り */
        TURN_SHORTEST, /* 最短方向 */
    } TurnMode;

private: /* コンストラクタでしか変更操作が行われないため排他制御不要 */
    size_t m_channel_fr, m_channel_fl, m_channel_br, m_channel_bl;
    bool m_inverse_fr, m_inverse_fl, m_inverse_br, m_inverse_bl;
    double m_max_rpm;
    double m_rotate_max, m_rotate_min, m_rotate_kp;
    Imu *m_imu;
    std::string m_json_path;
    size_t m_calc_period_ms;

private:
    jibiki::ShareVar<double> m_fr, m_fl, m_br, m_bl; /* 回転数目標値 */
    jibiki::ShareVar<double> m_raw_rpm[4];           /* 回転数目標値（入れ替え，反転なし） */
    jibiki::ShareVar<jibiki::time_point> m_time;     /* calc 用 */

private:
    void load_json(void);
    void calc(void);
    double calc_rotate(void);
    double calc_angle_diff(double subed, double sub, TurnMode turn_mode);

public:
    jibiki::ShareVar<double> m_speed;
    jibiki::ShareVar<double> m_theta;
    jibiki::ShareVar<double> m_spin;
    jibiki::ShareVar<TurnMode> m_turn_mode;

public:
    Chassis(Imu &imu, std::string json_path = "setting.json");
    void stop(void);
    double fr(void);
    double fl(void);
    double br(void);
    double bl(void);
    double raw_fr(void);
    double raw_fl(void);
    double raw_br(void);
    double raw_bl(void);
    double max_rpm(void) const noexcept; /* JSON ファイルから読み込んだ max_rpm を返す */
};
/* 足回りを停止させる */
inline void Chassis::stop(void)
{
    m_speed = 0;
    m_spin = m_imu->read();
}
/* 回転数目標値を返す（FR） */
inline double Chassis::fr(void)
{
    calc();
    return m_fr.read();
}
/* 回転数目標値を返す（FL） */
inline double Chassis::fl(void)
{
    calc();
    return m_fl.read();
}
/* 回転数目標値を返す（BR） */
inline double Chassis::br(void)
{
    calc();
    return m_br.read();
}
/* 回転数目標値を返す（BL） */
inline double Chassis::bl(void)
{
    calc();
    return m_bl.read();
}
/* 生の回転数目標値を返す（FR） */
inline double Chassis::raw_fr(void)
{
    calc();
    return m_raw_rpm[0].read();
}
/* 生の回転数目標値を返す（FL） */
inline double Chassis::raw_fl(void)
{
    calc();
    return m_raw_rpm[1].read();
}
/* 生の回転数目標値を返す（BR） */
inline double Chassis::raw_br(void)
{
    calc();
    return m_raw_rpm[2].read();
}
/* 生の回転数目標値を返す（BL） */
inline double Chassis::raw_bl(void)
{
    calc();
    return m_raw_rpm[3].read();
}
/* JSON ファイルから読み込んだ max_rpm を返す */
inline double Chassis::max_rpm(void) const noexcept { return m_max_rpm; }

#endif