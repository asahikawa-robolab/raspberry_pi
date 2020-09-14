#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_picojson.hpp"
#include "../../share/inc/module.hpp"

/* 外部変数 */
extern jibiki::ShareVar<short> g_angle;
extern SwitchData g_switch_data;
extern Imu g_imu;
extern Controller g_controller;

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

    if (com.receive())
        g_controller.set(com); /* 受信データを読み込ませる */
}