
#ifndef _DEFINES_HPP
#define _DEFINES_HPP
#include "_Config.hpp"
#include "_StdFunc.hpp"

/*-----------------------------------------------
MODE
-----------------------------------------------*/
#define MODE_ANGLE_TEST 0
#define MODE_ANGLE_POWER_WINDOW 1
#define MODE_ANGLE_ELEVATOR_A 2
#define MODE_ANGLE_ELEVATOR_B 4
#define MODE_GNDENCODER_A 0
#define MODE_GNDENCODER_B 1
#define MODE_REV_CHASSIS_A 0
#define MODE_REV_CHASSIS_B 1

#ifdef TEAM_A
#define MODE_REV_CHASSIS MODE_REV_CHASSIS_A
#endif
#ifdef TEAM_B
#define MODE_REV_CHASSIS MODE_REV_CHASSIS_B
#endif

/*-----------------------------------------------
CHASSIS
-----------------------------------------------*/
#ifdef TEAM_A
#define FR 2
#define FL 3
#define BR 0
#define BL 1
#define FR_INVERSE false
#define FL_INVERSE true
#define BR_INVERSE false
#define BL_INVERSE true
#define SPEED_MAX 150
#define GAIN_REV 10
#endif
#ifdef TEAM_B
#define FR 0
#define FL 1
#define BR 2
#define BL 3
#define FR_INVERSE true
#define FL_INVERSE false
#define BR_INVERSE true
#define BL_INVERSE false
#define SPEED_MAX 200
#define GAIN_REV 10
#endif

/*-----------------------------------------------
GNDENCODER
-----------------------------------------------*/
#ifdef TEAM_A
#define MODE_GNDENCODER MODE_GNDENCODER_A
#endif
#ifdef TEAM_B
#define MODE_GNDENCODER MODE_GNDENCODER_B
#endif

/*-----------------------------------------------
ADDRESS
-----------------------------------------------*/
#define ADDRESS 0x00

/*-----------------------------------------------
PATH
-----------------------------------------------*/
#ifdef TEAM_A
#define PATH_CHASSIS_R "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579ZM-if00-port0"
#define PATH_CHASSIS_L "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI057A81-if00-port0"
#define PATH_IMU "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579AL-if00-port0"
#define PATH_ENCODER "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579FP-if00-port0"
#define PATH_SWITCH "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH03IBXC-if00-port0"
#define PATH_LIMIT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A505YVUD-if00-port0"
#define PATH_LINE_FRONT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH01J8LU-if00-port0"
#define PATH_LINE_RIGHT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579ZK-if00-port0"
#define PATH_LINE_LEFT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579O5-if00-port0"
#define PATH_EMERGENCY "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579OE-if00-port0"
#define PATH_SERVO "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI057CF8-if00-port0"
#define PATH_ELEVATOR "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579ZU-if00-port0"
#define PATH_POWER_WINDOW "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579ZQ-if00-port0"
#endif
#ifdef TEAM_B
#define PATH_LINE_FRONT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A505SIYB-if00-port0"
//#define PATH_LINE_RIGHT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AL032KBO-if00-port0"
//#define PATH_LINE_LEFT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI057C80-if00-port0"
#define PATH_CHASSIS_R "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0"
#define PATH_CHASSIS_L "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LP-if00-port0"
#define PATH_IMU "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH03I9Z6-if00-port0"
#define PATH_SERVO_R "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579S8-if00-port0"  /* 4 */
#define PATH_SERVO_L "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A505YVUI-if00-port0"  /* 3 */
#define PATH_ELEVATOR "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579ZA-if00-port0" /* 1 */
#define PATH_MOTOR "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH03IC1H-if00-port0"    /* 2 */
#define PATH_ENCODER "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579K2-if00-port0"  /* 5 */
#define PATH_SWITCH "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V9YT-if00-port0"
#define PATH_LIMIT "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A505RXCU-if00-port0"
#define PATH_EMERGENCY "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V9VG-if00-port0"
#endif

#define PATH_MINIA "/dev/serial/by-id/usb-FTDI_FT230X_Basic_UART_DJ00M39V-if00-port0"
#define PATH_TEST "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH01J8LU-if00-port0"

/*-----------------------------------------------
KEY
-----------------------------------------------*/
#define KEY_LEFT 81
#define KEY_LEFT_KBHIT 68
#define KEY_UP 82
#define KEY_UP_KBHIT 65
#define KEY_RIGHT 83
#define KEY_RIGHT_KBHIT 67
#define KEY_DOWN 84
#define KEY_DOWN_KBHIT 66
#define KEY_PLUS 59
#define KEY_MINUS 45

#endif