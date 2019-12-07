#ifndef _CONFIG_HPP
#define _CONFIG_HPP
/*-----------------------------------------------
Aチーム，Bチームどちらかを選択
-----------------------------------------------*/
#define TEAM_B

/*-----------------------------------------------
COM
-----------------------------------------------*/
/* ENABLE / DISABLE */
#define ENABLE_DISPLAY true
#define ENABLE_COM_SWITCH true
#define ENABLE_COM_CHASSIS true
#define ENABLE_COM_ANGLE true
#define ENABLE_COM_GNDENCODER true
#define ENABLE_COM_IMU true
#define ENABLE_COM_LINETRACER true
#define ENABLE_COM_LIMIT true
#define ENABLE_COM_EMERGENCY false
#define ENABLE_COM_SERVO true
#define ENABLE_COM_MOTOR true

/*-----------------------------------------------
ORDER
-----------------------------------------------*/
#define ENABLE_ORDER_DEBUG true

#define INTERPOLATION_ACC_NO_ERROR /* liner_inter の acc のエラーを無視する */

#endif