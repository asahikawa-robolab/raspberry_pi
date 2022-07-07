#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_picojson.hpp"
#include "../../share/inc/module.hpp"
#include "../inc/ext_var.hpp"

/*-----------------------------------------------
与えられた１バイトの値から指定されたビットのデータを返す(0bit～7bit)
-----------------------------------------------*/
bool GET_BIT(uint8_t data, int bit)
{
    return (data >> bit) & 0b00000001;
}

void com_pwm_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 1, B57600, name, true);
    
    /* 送信 */
    com.tx(1) = g_pwm_tgt[0].read();
    com.tx(2) = g_pwm_tgt[1].read();
    com.send();

}

void com_rev_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_rev_tgt[0].read());
    com.tx(2) = jibiki::low(g_rev_tgt[0].read());
    com.tx(3) = jibiki::up(g_rev_tgt[1].read());
    com.tx(4) = jibiki::low(g_rev_tgt[1].read());
    com.send();

    /* 受信 */
    if (com.receive())
    {     
        g_rev_curr[0] = jibiki::asbl(com.rx(0), com.rx(1));
        g_rev_curr[1] = jibiki::asbl(com.rx(2), com.rx(3));
        
        printf("motor_1 target %d, curr %d, pwm %d\nmotor_2 target %d, curr %d, pwm %d\n", 
                g_rev_curr[0].read(), jibiki::asbl(com.rx(0), com.rx(1)),
                (int8_t)com.rx(4),
                g_rev_curr[1].read(), jibiki::asbl(com.rx(2), com.rx(3)),
                (int8_t)com.rx(5));
    }
}

void com_rot_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_rot_tgt[0].read());
    com.tx(2) = jibiki::low(g_rot_tgt[0].read());
    com.tx(3) = jibiki::up(g_rot_tgt[1].read());
    com.tx(4) = jibiki::low(g_rot_tgt[1].read());
    com.send();

    /* 受信 */
    if (com.receive())
    {
        g_rot_curr[0] = jibiki::asbl(com.rx(0), com.rx(1));
        g_rot_curr[1] = jibiki::asbl(com.rx(2), com.rx(3));
        printf("motor_1 target %d, curr %d, pwm %d\t motor_2 target %d, curr %d, pwm %d\n", 
                g_rot_tgt[0].read(), jibiki::asbl(com.rx(0), com.rx(1)),
                (int8_t)com.rx(4),
                g_rot_tgt[1].read(), jibiki::asbl(com.rx(2), com.rx(3)),
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
        g_dist_curr[0] = com.rx(0);
        g_dist_curr[0] = g_dist_curr[0].read() | (com.rx(1) << 8);
        g_dist_curr[0] = g_dist_curr[0].read() | (com.rx(2) << 16);
        g_dist_curr[0] = g_dist_curr[0].read() | (com.rx(3) << 24);
        g_dist_curr[1] = com.rx(4);
        g_dist_curr[1] = g_dist_curr[1].read() | (com.rx(5) << 8);
        g_dist_curr[1] = g_dist_curr[1].read() | (com.rx(6) << 16);
        g_dist_curr[1] = g_dist_curr[1].read() | (com.rx(7) << 24);

        // printf("%d\t%d\n%d\t%d\n", g_dist_tgt[0].read(), g_dist_tgt[1].read(),
        //     g_dist_curr[0].read(), g_dist_curr[1].read());
    }
}

void com_limit(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 1, 1, B57600, name, false);

    if(com.receive())
    {
        g_limit[0] = GET_BIT(com.rx(0), 0);
        g_limit[1] = GET_BIT(com.rx(0), 1);
        g_limit[2] = GET_BIT(com.rx(0), 2);
        g_limit[3] = GET_BIT(com.rx(0), 3);
        g_limit[4] = GET_BIT(com.rx(0), 4);
        g_limit[5] = GET_BIT(com.rx(0), 5);
        g_limit[6] = GET_BIT(com.rx(0), 6);
        g_limit[7] = GET_BIT(com.rx(0), 7);
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

    if(com.receive())
    {   
        g_chassis_rev[0] = jibiki::asbl(com.rx(0), com.rx(1));
        g_chassis_rev[1] = jibiki::asbl(com.rx(2), com.rx(3));
    //     std::cout << "rpm1 : " << jibiki::asbl(com.rx(0), com.rx(1)) << "\t";
    //     std::cout << "pwm1 : " << (int8_t)com.rx(4) << "\t";
    //     std::cout << "rpm2 : " << jibiki::asbl(com.rx(2), com.rx(3)) << "\t";
    //     std::cout << "pwm2 : " << (int8_t)com.rx(5) << std::endl;
    }
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

    if(com.receive())
    {   
        g_chassis_rev[2] = jibiki::asbl(com.rx(0), com.rx(1));
        g_chassis_rev[3] = jibiki::asbl(com.rx(2), com.rx(3));
    // // printf("\t\t\t\t%.0lf\t%.0lf\n", g_chassis.br(), g_chassis.bl());
    //         printf("curr %d, pwm %d\n", 
    //          jibiki::asbl(com.rx(0), com.rx(1)),(int8_t)com.rx(4));
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
	static jibiki::ParamCom com(path, 4, 8, B57600, name, false);

	g_controller.lcd_sprintf1("123ﾃｽﾄ");
	g_controller.lcd_sprintf2("456ﾃｽﾄ");

	/* データを送信する */
	g_controller.send(com);

	/* 受信データを読み込ませる */
	if (com.receive())
		g_controller.set(com);
}

void com_emergency(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 1, 1, B57600, name, false);

        if (com.receive())
        {
            emergency_sw = com.rx(0);
            // if(emergency_sw.read() == 1)
                //printf("on\n");
                
            // else

                //printf("off\n");
        }
}