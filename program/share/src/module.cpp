/* Last updated : 2020/10/04, 20:36 */
#define _USE_MATH_DEFINES
#include <sstream>
#include "../inc/module.hpp"
#include "../inc/_std_func.hpp"

/*-----------------------------------------------
*
* SwitchData
*
-----------------------------------------------*/
/* 受信データをメンバにセットする */
void SwitchData::set(jibiki::ParamCom &com)
{
    m_push_l = (com.rx(0) >> 0) & 0b1;
    m_push_r = (com.rx(0) >> 1) & 0b1;
    m_toggle[0] = (com.rx(0) >> 2) & 0b1;
    m_toggle[1] = (com.rx(0) >> 3) & 0b1;
    m_toggle[2] = (com.rx(0) >> 4) & 0b1;
    m_toggle[3] = (com.rx(0) >> 5) & 0b1;
    m_toggle[4] = (com.rx(0) >> 6) & 0b1;
    m_toggle[5] = (com.rx(0) >> 7) & 0b1;
    m_toggle[6] = (com.rx(1) >> 0) & 0b1;
    m_lu = (com.rx(1) >> 1) & 0b1;
    m_ld = (com.rx(1) >> 2) & 0b1;
    m_ru = (com.rx(1) >> 3) & 0b1;
    m_rd = (com.rx(1) >> 4) & 0b1;
    m_slide = com.rx(2);
}

/*-----------------------------------------------
*
* Controller
*
-----------------------------------------------*/
Controller::Controller(std::string json_path)
    : m_speed(0), m_theta{}, m_time(jibiki::get_time()),
      m_l_cross_l(false), m_l_cross_r(false),
      m_l_cross_u(false), m_l_cross_d(false),
      m_r_cross_l(false), m_r_cross_r(false),
      m_r_cross_u(false), m_r_cross_d(false),
      m_l_switch_d(false), m_l_switch_m(false),
      m_r_switch_d(false), m_r_switch_m(false),
      m_l_lever_l(false), m_l_lever_r(false),
      m_r_lever_l(false), m_r_lever_r(false),
      m_l_analog_stick_h(127), m_l_analog_stick_v(127),
      m_r_analog_stick_h(127), m_r_analog_stick_v(127),
      m_l_slide(false), m_r_slide(false),
      m_l_switch_u(false), m_r_switch_u(false),
      m_tact_lu(false), m_tact_mu(false),
      m_tact_ru(false), m_tact_ld(false),
      m_tact_md(false), m_tact_rd(false)
{
    /* JSON の読み込み */
    try
    {
        using picojson::object;
        picojson::value json_val = jibiki::load_json_file(json_path);
        m_calc_period_ms = (size_t)json_val.get<object>()["system"]
                               .get<object>()["calc_period_ms"]
                               .get<double>();
    }
    catch (const std::exception &e)
    {
        std::stringstream sstr;
        sstr << json_path << " 中の system の書式が不適切です．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
    /* エラーチェック */
    if (m_calc_period_ms <= 0)
    {
        std::stringstream sstr;
        sstr << json_path << "中の system/calc_period_ms の値が不適切です．\n"
             << "正の値を指定してください．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
}
/* 受信データをメンバにセットする */
void Controller::set(jibiki::ParamCom &com)
{
    /* 値を読み込む */
    m_l_cross_l = (com.rx(0) >> 0) & 0b1;
    m_l_cross_r = (com.rx(0) >> 1) & 0b1;
    m_l_cross_u = (com.rx(0) >> 2) & 0b1;
    m_l_cross_d = (com.rx(0) >> 3) & 0b1;
    m_r_cross_l = (com.rx(0) >> 4) & 0b1;
    m_r_cross_r = (com.rx(0) >> 5) & 0b1;
    m_r_cross_u = (com.rx(0) >> 6) & 0b1;
    m_r_cross_d = (com.rx(0) >> 7) & 0b1;
    m_l_switch_d = (com.rx(1) >> 0) & 0b1;
    m_l_switch_m = (com.rx(1) >> 1) & 0b1;
    m_r_switch_d = (com.rx(1) >> 2) & 0b1;
    m_r_switch_m = (com.rx(1) >> 3) & 0b1;
    m_l_lever_l = (com.rx(1) >> 4) & 0b1;
    m_l_lever_r = (com.rx(1) >> 5) & 0b1;
    m_r_lever_l = (com.rx(1) >> 6) & 0b1;
    m_r_lever_r = (com.rx(1) >> 7) & 0b1;
    m_l_analog_stick_h = com.rx(2);
    m_l_analog_stick_v = com.rx(3);
    m_r_analog_stick_h = com.rx(4);
    m_r_analog_stick_v = com.rx(5);
    m_l_slide = (com.rx(6) >> 0) & 0b1;
    m_r_slide = (com.rx(6) >> 1) & 0b1;
    m_l_switch_u = (com.rx(6) >> 6) & 0b1;
    m_r_switch_u = (com.rx(6) >> 7) & 0b1;
    m_tact_lu = (com.rx(7) >> 0) & 0b1;
    m_tact_mu = (com.rx(7) >> 1) & 0b1;
    m_tact_ru = (com.rx(7) >> 2) & 0b1;
    m_tact_ld = (com.rx(7) >> 3) & 0b1;
    m_tact_md = (com.rx(7) >> 4) & 0b1;
    m_tact_rd = (com.rx(7) >> 5) & 0b1;
}
/* アナログスティックの値から角度を計算 */
double Controller::my_atan(double y, double x, DirNum dir_num) const
{
    const double M_PI_6 = M_PI / 6;
    const double M_PI_3 = M_PI / 3;
    double theta = std::atan2(y, x);
    if (dir_num == DIR_8)
    {
        if (jibiki::between(-M_PI, theta, -M_PI_6 * 5))
            theta = -M_PI;
        else if (jibiki::between(-M_PI_6 * 5, theta, -M_PI_3 * 2))
            theta = -M_PI_4 * 3;
        else if (jibiki::between(-M_PI_3 * 2, theta, -M_PI_3))
            theta = -M_PI_2;
        else if (jibiki::between(-M_PI_3, theta, -M_PI_6))
            theta = -M_PI_4;
        else if (jibiki::between(-M_PI_6, theta, M_PI_6))
            theta = 0;
        else if (jibiki::between(M_PI_6, theta, M_PI_3))
            theta = M_PI_4;
        else if (jibiki::between(M_PI_3, theta, M_PI_3 * 2))
            theta = M_PI_2;
        else if (jibiki::between(M_PI_3 * 2, theta, M_PI_6 * 5))
            theta = M_PI_4 * 3;
        else if (jibiki::between2(M_PI_6 * 5, theta, M_PI))
            theta = M_PI;
    }
    else if (dir_num == DIR_4)
    {
        if (jibiki::between(-M_PI, theta, -M_PI_4 * 3))
            theta = -M_PI;
        else if (jibiki::between(-M_PI_4 * 3, theta, -M_PI_4))
            theta = -M_PI_2;
        else if (jibiki::between(-M_PI_4, theta, M_PI_4))
            theta = 0;
        else if (jibiki::between(M_PI_4, theta, M_PI_4 * 3))
            theta = M_PI_2;
        else if (jibiki::between2(M_PI_4 * 3, theta, M_PI))
            theta = M_PI;
    }
    return theta;
}

/* アナログスティックのデータを大きさ，向きに変換 */
void Controller::convt(Mode mode, DirNum dir_num)
{
    /*-----------------------------------------------
    前回実行してから m_calc_period_ms 経過していなかったら終了
    -----------------------------------------------*/
    double passed_time = jibiki::calc_sec(m_time.read(), jibiki::get_time());
    if (passed_time < m_calc_period_ms * 1E-3)
        return;
    else
        m_time = jibiki::get_time();

    /*-----------------------------------------------
    データを用意
    -----------------------------------------------*/
    /* mode に対応した生データを読み込む */
    double horizontal = (mode == MODE_L)
                            ? m_l_analog_stick_h.read()
                            : m_r_analog_stick_h.read();
    double vertical = (mode == MODE_L)
                          ? m_l_analog_stick_v.read()
                          : m_r_analog_stick_v.read();
    /* 0～255 を -127.5～127.5 に変更 */
    double x = horizontal - 127.5;
    double y = vertical - 127.5;

    /*-----------------------------------------------
    スピード [0%～100%]
    -----------------------------------------------*/
    const double INSENSIBLE_FIELD_SIZE = 30;                                /* 不感領域の大きさ（原点中心の正方形の辺長 ÷ 2） */
    double speed = sqrt(x * x + y * y);                                     /* 大きさ（原点からの距離）を求める */
    if (fabs(x) < INSENSIBLE_FIELD_SIZE && fabs(y) < INSENSIBLE_FIELD_SIZE) /* 不感領域にいる場合 0 にする */
        speed = 0;
    speed *= 100 / 127.5;                /* 百分率に変換 */
    speed = (speed > 100) ? 100 : speed; /* 100 以下に収める */

    /*-----------------------------------------------
    向き [rad]
    -----------------------------------------------*/
    double theta = my_atan(y, x, dir_num);

    /*-----------------------------------------------
    値を返す
    -----------------------------------------------*/
    m_speed = speed;
    m_theta[mode] = (speed != 0) ? theta : m_theta[mode].read(); /* 不感領域では theta を変更しない */
}

/*-----------------------------------------------
 *
 * Chassis
 *
-----------------------------------------------*/
/* コンストラクタ */
Chassis::Chassis(Imu &imu, std::string json_path)
    : m_imu(&imu),
      m_json_path(json_path),
      m_time(jibiki::get_time()),
      m_speed(0),
      m_theta(jibiki::deg_rad(90)),
      m_spin(0),
      m_turn_mode(TURN_SHORTEST)
{
    try
    {
        /* JSON ファイルから設定を読み込む */
        load_json();
    }
    catch (const std::exception &e)
    {
        jibiki::print_err(__PRETTY_FUNCTION__);
        throw; /* 仲介 */
    }
}

/* JSON ファイルを読み込む */
void Chassis::load_json(void)
{
    using picojson::object;
    double c1, c2, c3, c4; /* channel */

    /*-----------------------------------------------
    ファイルを読み込む
    -----------------------------------------------*/
    try
    {
        /* chassis */
        picojson::value json_value = jibiki::load_json_file(m_json_path);
        object &obj = json_value.get<object>()["chassis"].get<object>();
        /* max_rpm */
        m_max_rpm = obj["max_rpm"].get<double>();
        /* channel */
        c1 = obj["channel"].get<object>()["fr"].get<double>();
        c2 = obj["channel"].get<object>()["fl"].get<double>();
        c3 = obj["channel"].get<object>()["br"].get<double>();
        c4 = obj["channel"].get<object>()["bl"].get<double>();
        m_channel_fr = (size_t)c1;
        m_channel_fl = (size_t)c2;
        m_channel_br = (size_t)c3;
        m_channel_bl = (size_t)c4;
        /* inverse */
        m_inverse_fr = obj["inverse"].get<object>()["fr"].get<bool>();
        m_inverse_fl = obj["inverse"].get<object>()["fl"].get<bool>();
        m_inverse_br = obj["inverse"].get<object>()["br"].get<bool>();
        m_inverse_bl = obj["inverse"].get<object>()["bl"].get<bool>();
        /* rotate */
        m_rotate_min = obj["rotate"].get<object>()["min"].get<double>();
        m_rotate_max = obj["rotate"].get<object>()["max"].get<double>();
        m_rotate_kp = obj["rotate"].get<object>()["kp"].get<double>();
    }
    catch (const std::exception &e)
    {
        std::stringstream sstr;
        sstr << m_json_path << " 中の chassis の書式が不適切です．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
    /*-----------------------------------------------
    エラーチェック
    -----------------------------------------------*/
    try
    {
        bool v_fr = c1 == 0 || c1 == 1 || c1 == 2 || c1 == 3;
        bool v_fl = c2 == 0 || c2 == 1 || c2 == 2 || c2 == 3;
        bool v_br = c3 == 0 || c3 == 1 || c3 == 2 || c3 == 3;
        bool v_bl = c4 == 0 || c4 == 1 || c4 == 2 || c4 == 3;
        if ((v_fr & v_fl & v_br & v_bl) == false)
            throw std::string("chassis/channel の値が不適切です．\n"
                              " 0, 1, 2, 3 のいずれかを指定してください．");
        bool eq1 = c1 == c2 || c1 == c3 || c1 == c4;
        bool eq2 = c2 == c3 || c2 == c4;
        bool eq3 = c3 == c4;
        if (eq1 | eq2 | eq3)
            throw std::string("chassis/channel の値が不適切です．\n"
                              "重複しない相異なる値を指定してください．"); /* エラー発生 */
        if (m_max_rpm <= 0)
            throw std::string("chassis/max_rpm の値が不適切です．\n"
                              "正の値を指定してください．"); /* エラー発生 */
        if (m_rotate_min <= 0)
            throw std::string("chassis/rotate/min の値が不適切です．\n"
                              "正の値を指定してください．"); /* エラー発生 */
        if (m_rotate_max <= 0)
            throw std::string("chassis/rotate/max の値が不適切です．\n"
                              "正の値を指定してください．"); /* エラー発生 */
        if (m_rotate_kp <= 0)
            throw std::string("chassis/rotate/kp の値が不適切です．\n"
                              "正の値を指定してください．"); /* エラー発生 */
    }
    catch (std::string err)
    {
        std::stringstream sstr;
        sstr << m_json_path << " 中の " << err;
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
    /*-----------------------------------------------
    system_calc_period_ms
    -----------------------------------------------*/
    try
    {
        picojson::value json_val = jibiki::load_json_file(m_json_path);
        m_calc_period_ms = (size_t)json_val.get<object>()["system"]
                               .get<object>()["calc_period_ms"]
                               .get<double>();
    }
    catch (const std::exception &e)
    {
        std::stringstream sstr;
        sstr << m_json_path << " 中の system の書式が不適切です．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
    /*-----------------------------------------------
    エラーチェック
    -----------------------------------------------*/
    if (m_calc_period_ms <= 0)
    {
        std::stringstream sstr;
        sstr << m_json_path << " 中の system/calc_period_ms の値が不適切です．\n"
                               "正の値を指定してください．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }
}
/* 各モータの回転数目標値を計算 */
void Chassis::calc(void)
{
    /*-----------------------------------------------
    前回実行してから m_calc_period_ms 経過していなかったら終了
    -----------------------------------------------*/
    double passed_time = jibiki::calc_sec(m_time.read(), jibiki::get_time());
    if (passed_time < 50E-3)
        return;
    else
        m_time = jibiki::get_time();

    /*-----------------------------------------------
    エラーチェック
    -----------------------------------------------*/
    if (!jibiki::between2(0.0, m_speed.read(), m_max_rpm))
    {
        std::stringstream sstr;
        sstr << "Chassis::m_speed の値 (" << m_speed.read() << ") が不適切です．\n "
             << m_json_path << " の chassis/max_rpm の値 (" << max_rpm()
             << ") を超えないようにしてください．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(""); /* エラー発生 */
    }

    double rotate = calc_rotate();       /* 回転量を計算 */
    double current_spin = m_imu->read(); /* 現在の回転角を取得 */

    /*-----------------------------------------------
    目標回転数を計算
    -----------------------------------------------*/
    double val_1 = m_speed.read() * sin(m_theta.read() - current_spin - M_PI_4);
    double val_2 = m_speed.read() * sin(m_theta.read() - current_spin + M_PI_4);
    m_raw_rpm[0] = val_1 + rotate; /* FR */
    m_raw_rpm[1] = val_2 - rotate; /* FL */
    m_raw_rpm[2] = val_2 + rotate; /* BR */
    m_raw_rpm[3] = val_1 - rotate; /* BL */

    /*-----------------------------------------------
    rotate も含めて m_max_rpm を超えないようにする
    -----------------------------------------------*/
    /* m_raw_rpm の中で絶対値が最大のものを求める */
    double max_val = 0;
    for (size_t i = 0; i < 4; ++i)
        if (fabs(m_raw_rpm[i].read()) > max_val)
            max_val = fabs(m_raw_rpm[i].read());

    /* rotate を加減算して m_max_rpm を超える場合 */
    if (max_val > m_max_rpm)
    {
        /* rotate を修正 */
        double diff = m_max_rpm - std::max(fabs(val_1), fabs(val_2));
        rotate = jibiki::get_signal(rotate) * diff;
        /* 目標回転数を再計算 */
        m_raw_rpm[0] = val_1 + rotate; /* FR */
        m_raw_rpm[1] = val_2 - rotate; /* FL */
        m_raw_rpm[2] = val_2 + rotate; /* BR */
        m_raw_rpm[3] = val_1 - rotate; /* BL */
    }

    /*-----------------------------------------------
    m_channel_x に従って値を入れ替える
    -----------------------------------------------*/
    m_fr = m_raw_rpm[m_channel_fr].read();
    m_fl = m_raw_rpm[m_channel_fl].read();
    m_br = m_raw_rpm[m_channel_br].read();
    m_bl = m_raw_rpm[m_channel_bl].read();

    /*-----------------------------------------------
    m_inverse_x に従って極性を反転する
    -----------------------------------------------*/
    if (m_inverse_fr)
        m_fr *= -1;
    if (m_inverse_fl)
        m_fl *= -1;
    if (m_inverse_br)
        m_br *= -1;
    if (m_inverse_bl)
        m_bl *= -1;
}
/* 回転量を計算 */
double Chassis::calc_rotate(void)
{
    /* 角度偏差を計算 */
    double diff =
        calc_angle_diff(m_spin.read(), m_imu->read(), m_turn_mode.read());

    /* 偏差が小さくなったら最短モードに変更 */
    /* （オーバーシュートしたときにもう一回転してしまうから） */
    if (fabs(diff) < jibiki::deg_rad(90))
        m_turn_mode = TURN_SHORTEST;

    /* 偏差に応じて rotate を決定 */
    double rotate;
    if (fabs(diff) < jibiki::deg_rad(2))
    {
        rotate = 0;
    }
    else
    {
        rotate = diff * m_rotate_kp;
        if (fabs(rotate) < m_rotate_min)
            rotate = m_rotate_min * jibiki::get_signal(rotate);
        else if (m_rotate_max < fabs(rotate))
            rotate = m_rotate_max * jibiki::get_signal(rotate);
    }

    return rotate;
}
/* -pi ~ pi の不連続部に影響されない角度偏差（subed - sub）を返す */
double Chassis::calc_angle_diff(double subed, double sub, TurnMode turn_mode)
{
    /* 角度を -pi ~ pi にする */
    double subed_limited = jibiki::limit_angle(subed);
    double sub_limited = jibiki::limit_angle(sub);
    double angle_diff = 0;

    switch (turn_mode)
    {
    case TURN_CCW:
        if (subed_limited - sub_limited >= 0)
            angle_diff = subed_limited - sub_limited;
        else
            angle_diff = subed_limited - sub_limited + 2 * M_PI;
        break;
    case TURN_CW:
        if (subed_limited - sub_limited > 0)
            angle_diff = subed_limited - sub_limited - 2 * M_PI;
        else
            angle_diff = subed_limited - sub_limited;
        break;
    case TURN_SHORTEST:
        if (subed_limited - sub_limited > M_PI)
            angle_diff = (subed_limited - sub_limited) - 2 * M_PI;
        else if (subed_limited - sub_limited < -M_PI)
            angle_diff = (subed_limited - sub_limited) + 2 * M_PI;
        else
            angle_diff = subed_limited - sub_limited;
        break;
    }
    return angle_diff;
}