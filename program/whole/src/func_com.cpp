#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_picojson.hpp"
#include "../../share/inc/module.hpp"
#include "../inc/ext_var.hpp"

void com_rot_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_angle.read());
    com.tx(2) = jibiki::low(g_angle.read());
    com.send();

    /* 受信 */
    if (com.receive())
    {
        // printf("target %d, curr %d, pwm %d\n",
        //        g_angle.read(),
        //        jibiki::asbl(com.rx(0), com.rx(1)),
        //        (int8_t)com.rx(4));
    }
}

/* 足回りモータ（右前，左前） */
void com_chassis_f(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);
    com.tx(1) = jibiki::up(g_chassis.fr());
    com.tx(2) = jibiki::low(g_chassis.fr());
    com.tx(3) = jibiki::up(g_chassis.fl());
    com.tx(4) = jibiki::low(g_chassis.fl());
    com.send();
}

/* 足回りモータ（右後，左後） */
void com_chassis_b(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);
    com.tx(1) = jibiki::up(g_chassis.br());
    com.tx(2) = jibiki::low(g_chassis.br());
    com.tx(3) = jibiki::up(g_chassis.bl());
    com.tx(4) = jibiki::low(g_chassis.bl());
    com.send();
}

void com_switch(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 1, 3, B57600, name, false);

    if (com.receive())
    {
        /* データを送り返してスイッチスレーブの LED を点灯させる */
        com.tx(0) = com.rx(0);
        com.send();

        /* 受信データを読み込ませる */
        g_switch_data.set(com);
    }
}

void com_imu(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 1, 4, B57600, name, false);

    /* 受信 */
    if (com.receive())
    {
        double angle =
            jibiki::deg_rad(jibiki::asbl(com.rx(0), com.rx(1)) / 10.0);
        g_imu.write_raw_data(angle);
    }
}

void com_controller(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 0, 8, B57600, name, false);

    /* 受信データを読み込ませる */
    if (com.receive())
        g_controller.set(com);
}