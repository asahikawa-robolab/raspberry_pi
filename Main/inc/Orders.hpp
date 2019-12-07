#ifndef ORDERS_HPP
#define ORDERS_HPP
#include <vector>
#include <string>
#include <cstdint>
#include <time.h>
#include "../../Share/inc/_StdFunc.hpp"
#include "../../Share/inc/_Module.hpp"

/*-----------------------------------------------
 *
 * ORDER_VAL
 *
-----------------------------------------------*/
class ORDER_VAL
{
public:
    std::string name;
    double value;
};

/*-----------------------------------------------
 *
 * ORDER
 *
-----------------------------------------------*/
#define NUM_ORDER_VAL 2 /* Order 内で使用する変数の数 */
#define NUM_MODE 13     /* モードの種類 */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * MODE_KIND と _Modes の並びは同じにすること
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
typedef enum
{
    MODE_TEST,             /* テスト用のモード．指定時間経過したら終了． */
    MODE_WAIT,             /* スタートボタンが押されるまで待機 */
    MODE_ENCODER,          /* 接地エンコータを使って移動 */
    MODE_TURN,             /* 旋回処理 */
    MODE_LIMIT_CHASSIS,    /* リミットスイッチが押されるまで移動 */
    MODE_ANGLE,            /* 回転角制御 */
    MODE_SERVO,            /* サーボ */
    MODE_MOTOR,            /* モータスレーブを動かすモード */
    MODE_SET_ODOMETRY,     /* 指定された値でオドメトリを上書きする */
    MODE_SET_SPIN,         /* 指定した値でロボットの回転角（spin）を上書きする */
    MODE_ALONG_LINE,       /* ラインに沿って移動する */
    MODE_DETECT_LINE,      /* ラインを検出するまで指定の MOTION をする */
    MODE_POSITIONING_LINE, /* ラインで位置合わせをして，オドメトリを上書き */
} MODE_KIND;

const char _Modes[NUM_MODE][2][MAX_STRING_LENGTH] =
    {
        {"test", 1},
        {"wait", 0},
        {"encoder", 7},
        {"turn", 2},
        {"limit_chassis", 3},
        {"angle", 2},
        {"servo", 3},
        {"motor", 3},
        {"set_odometry", 2},
        {"set_spin", 1},
        {"along_line", 7},
        {"detect_line", 3},
        {"positioning_line", 3},
};

extern ORDER_VAL _OrderVal[NUM_ORDER_VAL];

#define STATE_YET 0   /* 未実行 */
#define STATE_DOING 1 /* 実行中 */
#define STATE_WAIT 2  /* 同一ブロック処理の終了待ち */
#define STATE_DONE 3  /* 終了 */

/* 補間に使用するパラメータ */
class INTER_PARAM
{
public:
    double min, max, acc, dif_init, dif, thres;
    std::string edge;
    INTER_PARAM(double min0,
                double max0,
                double acc0,
                double dif_init0,
                double dif0,
                std::string edge0)
    {
        min = min0;
        max = max0;
        acc = acc0;
        dif_init = dif_init0;
        dif = dif0;
        thres = 0;
        edge = edge0;
    }
};

class ORDER
{
public:
    uint16_t seq[3];
    int state; /* 未実行，実行中，完了 */
    char name[MAX_STRING_LENGTH];
    int mode;

    std::vector<std::string> par; /* パラメータ */
    ORDER(uint16_t seq1, uint16_t seq2, uint16_t seq3, const char *name0);
    void load_par(FILE *fp);
    std::string appval(std::string str);
    int judge_mode(char *mode_str);
    double liner_inter(INTER_PARAM &param);
    /* mode */
    void mode_test(void);
    void mode_wait(void);
    void mode_encoder(char *str);
    void mode_limit_chassis(void);
    void mode_turn(void);
    void mode_angle(char *str);
    void mode_servo(void);
    void mode_set_odometry(void);
    void mode_along_line(char *str);
    void mode_detect_line(void);
    void mode_positioning_line(void);
    void mode_set_spin(void);
    void mode_motor(void);

private:
    /* メンバ関数用の変数 */
    bool isFirst;
    bool isFirst_acc_err;                 /* liner_inter */
    int pre_value;                        /* angle */
    double dif_init;                      /* encoder, along_line */
    struct timespec time_s;               /* test, servo, set_odometry */
    int complete_cnt;                     /* detect_line_center */
    cv::Point2f odometry_init;            /* encoder, limit_chassis */
    double theta_control;                 /* encoder, limit_chassis */
    std::vector<cv::Point2f> check_point; /* encoder */

    void finish(char *str0);
    void calc_theta_control(double target_theta, double kp);
    int mode_positioning_line_decide_direction(uint8_t data);
    /* liner_inter */
    double liner_inter_rise(INTER_PARAM param);
    double liner_inter_fall(INTER_PARAM param);
    double liner_inter_both(INTER_PARAM param);
};

/*-----------------------------------------------
 *
 * ORDERS
 *
-----------------------------------------------*/
class ORDERS
{
public:
    std::vector<ORDER> q;
    ORDERS(void);
    ~ORDERS(void);
    void print(void);
    void print_state(void);
    void manage(void);
    void execute(void);
    void load(void);
    void write_exe(char *str);
    void read_exe(char *str);
    void read_order_path(char *str);
    void set_order_path(void);

private:
    char *exe, *order_path;
    size_t cnt; /* write_exe() */

    int find(uint16_t seq1, uint16_t seq2, uint16_t seq3);
    int find(uint16_t seq1, uint16_t seq2);
    int find(uint16_t seq1);
    void set_done(void);
    int judge_mode(char *mode_str);

    /* private 隠蔽 */
    ORDERS(const ORDERS &obj);
    const ORDERS &operator=(const ORDERS &obj);
};

inline void ORDERS::write_exe(char *str)
{
    /* 指定回数毎に Executing を更新 */
    if (cnt > 1000)
    {
        cnt = 0;
        exe[0] = '\0';
        strcpy(exe, str);
    }
    cnt++;
}

inline void ORDERS::read_exe(char *str)
{
    if (strlen(exe) > 0)
    {
        strcpy(str, exe);
    }
}

inline void ORDERS::read_order_path(char *str)
{
    strcpy(str, order_path);
}

#endif