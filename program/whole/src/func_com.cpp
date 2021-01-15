#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_picojson.hpp"
#include "../../share/inc/module.hpp"
#include "../inc/ext_var.hpp"

void com_pwm_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 1, B57600, name, true);
    
    /* 送信 */
    com.tx(1) = g_pwm[0].read();
    com.tx(2) = g_pwm[1].read();
    com.send();

}

void com_rev_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_rev[0].read());
    com.tx(2) = jibiki::low(g_rev[0].read());
    com.tx(3) = jibiki::up(g_rev[1].read());
    com.tx(4) = jibiki::low(g_rev[1].read());
    com.send();

    /* 受信 */
    if (com.receive())
    {     
        g_rev_diff[0] = jibiki::asbl(com.rx(0), com.rx(1));/* 偏差じゃないから */
        g_rev_diff[1] = jibiki::asbl(com.rx(2), com.rx(3));/* 偏差じゃないから */

        printf("motor_1 target %d, curr %d, pwm %d\nmotor_2 target %d, curr %d, pwm %d\n", 
                g_rev[0].read(), jibiki::asbl(com.rx(0), com.rx(1)),
                (int8_t)com.rx(4),
                g_rev[1].read(), jibiki::asbl(com.rx(2), com.rx(3)),
                (int8_t)com.rx(5));
    }
}

void com_rot_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_rot[0].read());
    com.tx(2) = jibiki::low(g_rot[0].read());
    com.tx(3) = jibiki::up(g_rot[1].read());
    com.tx(4) = jibiki::low(g_rot[1].read());
    com.send();

    /* 受信 */
    if (com.receive())
    {
        g_rot_diff[0] = jibiki::asbl(com.rx(0), com.rx(1));/* 偏差じゃないから */
        g_rot_diff[1] = jibiki::asbl(com.rx(2), com.rx(3));/* 偏差じゃないから */
        printf("motor_1 target %d, curr %d, pwm %d\nmotor_2 target %d, curr %d, pwm %d\n", 
                g_rev[0].read(), jibiki::asbl(com.rx(0), com.rx(1)),
                (int8_t)com.rx(4),
                g_rev[1].read(), jibiki::asbl(com.rx(2), com.rx(3)),
                (int8_t)com.rx(5));
    }
}

void com_odmetry_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 8, B57600, name, true);

    /* 送信 */
    com.tx(1) = g_odometry_flag[0].read();
    com.tx(2) = g_odometry_flag[1].read();
    com.send();

    /* 受信 */
    if(com.receive())
    {
        g_dist[0] = com.rx(0);
        g_dist[0] = g_dist[0].read() | (com.rx(1) << 8);
        g_dist[0] = g_dist[0].read() | (com.rx(2) << 16);
        g_dist[0] = g_dist[0].read() | (com.rx(3) << 24);
        g_dist[1] = com.rx(4);
        g_dist[1] = g_dist[1].read() | (com.rx(5) << 8);
        g_dist[1] = g_dist[1].read() | (com.rx(6) << 16);
        g_dist[1] = g_dist[1].read() | (com.rx(7) << 24);
        // printf("%d, %d\n", g_dist[0].read(), g_dist[1].read());
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