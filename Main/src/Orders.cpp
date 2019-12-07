#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <climits>
#include <unistd.h>
#include <string>
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"
#include "../inc/Orders.hpp"

/*-----------------------------------------------
 *
 * ORDER
 *
-----------------------------------------------*/
ORDER_VAL _OrderVal[NUM_ORDER_VAL] = {
    {"$speed_max", SPEED_MAX},
    {"$collect_pos", 50},
};

ORDER::ORDER(uint16_t seq1, uint16_t seq2, uint16_t seq3, const char *name0)
{
    if (seq1 < 1 || seq2 < 1 || seq3 < 1)
    {
        PrintError("ORDER::ORDER()", "invalid seq", (double)seq1);
        _Flags.e(EXIT_FAILURE);
    }
    state = STATE_YET;
    seq[0] = seq1;
    seq[1] = seq2;
    seq[2] = seq3;
    snprintf(name, MAX_STRING_LENGTH, "%s", name0);
    isFirst = true;
    isFirst_acc_err = true;
}

void ORDER::load_par(FILE *fp)
{
    /* エラーチェック */
    if (BETWEEN(0, mode, NUM_MODE) == false)
    {
        PrintError("ORDER::load_par", "invalid mode", (double)mode);
        _Flags.e(EXIT_FAILURE);
    }

    /* パラメータを読み込む */
    for (int i = 0; i < _Modes[mode][1][0]; i++)
    {
        char str[MAX_STRING_LENGTH];
        fscanf(fp, "%s", str);
        std::string tmp = appval(str);
        par.emplace_back(tmp);
    }
}

std::string ORDER::appval(std::string str)
{
    std::string out;

    /* 変数のシンボルがあった場合 */
    if (str[0] == '$')
    {
        bool isFound = false;
        /* _OrderVal から一致するものを探す */
        for (size_t i = 0; i < NUM_ORDER_VAL; i++)
        {
            if (str == _OrderVal[i].name)
            {
                isFound = true;
                out = std::to_string(_OrderVal[i].value);
                break;
            }
        }
        /* エラーチェック */
        if (isFound == false)
        {
            PrintError("ORDER::appval()",
                       "invalid name",
                       str.c_str());
            _Flags.e(EXIT_FAILURE);
        }
    }
    /* 変数のシンボルがなかった場合（ただの定数） */
    else
    {
        out = str;
    }
    return out;
}

/* ORDER のモード判定 */
int ORDER::judge_mode(char *mode_str)
{
    int mode_num = 0;
    bool isFound = false;

    /* _Modes の文字列と比較する */
    for (int i = 0; i < NUM_MODE; i++)
    {
        if (strcmp(mode_str, _Modes[i][0]) == 0)
        {
            mode = i;
            isFound = true;
            break;
        }
    }

    /* 一つも一致しなかったらエラー */
    if (isFound == false)
    {
        char tmp[MAX_STRING_LENGTH];
        snprintf(tmp, MAX_STRING_LENGTH, "invalid mode (%s)", mode_str);
        PrintError("ORDERS::judge_mode()", tmp);
        _Flags.e(EXIT_FAILURE);
    }

    return mode_num;
}

void ORDER::finish(char *str0)
{
    state = STATE_WAIT;
    printf("complete (%s) [%s]\n", str0, name);
}

/*-----------------------------------------------
 *
 * 線形補間（RISE, FALL, BOTH, NONE）
 *
-----------------------------------------------*/
double ORDER::liner_inter(INTER_PARAM &param)
{
    /* エラーチェック */
    if (param.acc < 0)
    {
        _Chassis.stop();
        PrintError("ORDER::liner_inter()", "invalid acc", param.acc);
        _Flags.e(EXIT_FAILURE);
    }

    /* acc から thres を計算 */
    param.thres = (param.max - param.min) / param.acc;

    /* 加速しきらない場合 thres を変更 */
    bool err_both = (param.edge == "BOTH" && param.dif_init < param.thres * 2);
    bool err_rise_fall = ((param.edge == "RISE" || param.edge == "FALL") && param.dif_init < param.thres);
    if (err_both || err_rise_fall)
    {
        double pre_thres = param.thres;
        if (err_both)
            param.thres = param.dif_init / 2;
        else
            param.thres = param.dif_init;

        if (isFirst_acc_err == true)
        {
            isFirst_acc_err = false;
            printf("ORDER::liner_inter(), thres was changed. (%.3lf -> %.3lf)\n",
                   pre_thres, param.thres);
        }
    }

    /* edge に応じた処理 */
    if (param.edge == "RISE")
    {
        return liner_inter_rise(param);
    }
    else if (param.edge == "FALL")
    {
        return liner_inter_fall(param);
    }
    else if (param.edge == "BOTH")
    {
        return liner_inter_both(param);
    }
    else if (param.edge == "NONE")
    {
        return param.max;
    }
    else
    {
        PrintError("ORDER::liner_inter", "invalid edge", param.edge.c_str());
        _Flags.e(EXIT_FAILURE);
        return 1;
    }
}

double ORDER::liner_inter_rise(INTER_PARAM param)
{
    if (BETWEEN(0, param.dif, param.dif_init - param.thres))
    {
        return param.max;
    }
    else if (BETWEEN(param.dif_init - param.thres, param.dif, param.dif_init))
    {
        return LINER(param.dif_init - param.thres, param.max,
                     param.dif_init, param.min,
                     param.dif);
    }
    else if (param.dif_init <= param.dif)
    {
        return param.min;
    }
    else
    {
        PrintError("ORDER::liner_inter_rise()", "invalid dif", param.dif);
        _Flags.e(EXIT_FAILURE);
        return 1;
    }
}

double ORDER::liner_inter_fall(INTER_PARAM param)
{
    if (BETWEEN(0, param.dif, param.thres))
    {
        return LINER(0, param.min,
                     param.thres, param.max,
                     param.dif);
    }
    else if (param.thres <= param.dif)
    {
        return param.max;
    }
    else
    {
        PrintError("ORDER::liner_inter_rise()", "invalid dif", param.dif);
        _Flags.e(EXIT_FAILURE);
        return 1;
    }
}

double ORDER::liner_inter_both(INTER_PARAM param)
{
    if (BETWEEN(0, param.dif, param.thres))
    {
        return LINER(0, param.min,
                     param.thres, param.max,
                     param.dif);
    }
    else if (BETWEEN(param.thres, param.dif, param.dif_init - param.thres))
    {
        return param.max;
    }
    else if (BETWEEN(param.dif_init - param.thres, param.dif, param.dif_init))
    {
        return LINER(param.dif_init - param.thres, param.max,
                     param.dif_init, param.min,
                     param.dif);
    }
    else if (param.dif_init <= param.dif)
    {
        return param.min;
    }
    else
    {
        PrintError("ORDER::liner_inter_both()", "invalid dif", param.dif);
        _Flags.e(EXIT_FAILURE);
        return 1;
    }
}

/*-----------------------------------------------
 *
 * ORDERS
 *
-----------------------------------------------*/
ORDERS::ORDERS(void)
{
    /* 共有メモリ */
    exe = (char *)shmOpen(SHM_EXECUTING,
                          sizeof(char) * NUM_EXECUTING,
                          SHM_NAME_EXECUTING);
    order_path = (char *)shmOpen(SHM_ORDER_PATH,
                                 sizeof(char) * NUM_ORDER_PATH,
                                 SHM_NAME_ORDER_PATH);

    /* mode_wait をセット */
    ORDER tmp(1, 1, 1, "wait");
    tmp.mode = MODE_WAIT;
    tmp.state = STATE_DOING;
    q.emplace_back(tmp);

    /* 変数の初期化 */
    cnt = 0;
}

ORDERS::~ORDERS(void)
{
    shmClose(SHM_EXECUTING);
    shmClose(SHM_ORDER_PATH);
}

void ORDERS::print(void)
{
    printf("-------------------------------\n");
    for (size_t i = 0; i < q.size(); i++)
    {
        printf("%s\n%d, %d, %d : %s, %d\n",
               q[i].name,
               q[i].seq[0],
               q[i].seq[1],
               q[i].seq[2],
               _Modes[q[i].mode][0],
               q[i].state);
        for (size_t j = 0; j < q[i].par.size(); j++)
        {
            printf("%s ", q[i].par[j].c_str());
        }
        printf("\n\n");
    }
    printf("-------------------------------\n");
}

void ORDERS::print_state(void)
{
    for (size_t i = 0; i < q.size(); i++)
    {
        printf("%d, ", q[i].state);
    }
    printf("\n");
}

void ORDERS::manage(void)
{
    // print_state();
    for (size_t i = 0; i < q.size(); i++)
    {
        if (q[i].state == STATE_WAIT)
        {
            /* 直列に接続されている処理を実行 */
            {
                int num_seq1 = q[i].seq[0];
                int num_seq2 = q[i].seq[1];
                int num_seq3 = q[i].seq[2];
                if (num_seq3 < find(num_seq1, num_seq2))
                {
                    int order_num = find(num_seq1, num_seq2, num_seq3 + 1);
                    q[order_num].state = STATE_DOING;
                    q[i].state = STATE_DONE;
                    continue;
                }
            }

            /* 現在のブロックの処理がすべて終了しているか確認 */
            bool isComplete = true;
            {
                int num_seq1 = q[i].seq[0];
                int num_seq2 = find(num_seq1); /* 分岐の数 */
                int final_seq3;                /* 直列接続の数 */
                int order_num_final;           /* 分岐の先端のベクトルの要素番号 */
                for (int j = 0; j < num_seq2; j++)
                {
                    final_seq3 = find(num_seq1, j + 1);
                    order_num_final = find(num_seq1, j + 1, final_seq3);
                    if (q[order_num_final].state != STATE_WAIT)
                    {
                        isComplete = false;
                    }
                }
            }

            /* 全て終了していたら state を STATE_DONE にして，
            次のブロックの state を STATE_DOING にセットする */
            if (isComplete == true)
            {
                // print();
                set_done();

                /* 終了かどうか判断する */
                {
                    int num_seq1 = q[i].seq[0] + 1;
                    int num_seq2 = find(num_seq1);
                    if (num_seq2 == 0)
                    {
                        printf("end.\n");
                        _Flags.f[FLAG_EMERGENCY] = STOP;
                        _Flags.e(EXIT_SUCCESS);
                    }
                }

                /* STATE_DOING をセットする */
                {
                    int num_seq1 = q[i].seq[0] + 1;
                    int num_seq2 = find(num_seq1);
                    int order_num;
                    for (int j = 0; j < num_seq2; j++)
                    {
                        order_num = find(num_seq1, j + 1, 1);
                        q[order_num].state = STATE_DOING;
                    }
                }
            }
        }
    }
}

/* 引数に指定した seq と一致する ORDER の番号を返す */
int ORDERS::find(uint16_t seq1, uint16_t seq2, uint16_t seq3)
{
    for (size_t count = 0; count < q.size(); count++)
    {
        if ((q[count].seq[0] == seq1) && (q[count].seq[1] == seq2) && (q[count].seq[2] == seq3))
        {
            return count;
        }
    }

    /* 指定された seq が見つからなかったら -1 を返す */
    return -1;
}

/* 引数で指定した seq において，seq3 の最大値を返す */
int ORDERS::find(uint16_t seq1, uint16_t seq2)
{
    int max = 0;
    for (size_t i = 0; i < q.size(); i++)
    {
        if ((q[i].seq[0] == seq1) && (q[i].seq[1] == seq2) && (q[i].seq[2] > max))
        {
            max = q[i].seq[2];
        }
    }
    return max;
}

/* 引数で指定した seq において，seq2 の最大値を返す */
int ORDERS::find(uint16_t seq1)
{
    int max = 0;
    for (size_t i = 0; i < q.size(); i++)
    {
        if ((q[i].seq[0] == seq1) && (q[i].seq[1] > max))
        {
            max = q[i].seq[1];
        }
    }
    return max;
}

/* state が STATE_WAIT になっている Order をすべて STATE_DONE にする */
void ORDERS::set_done(void)
{
    for (size_t i = 0; i < q.size(); i++)
    {
        if (q[i].state == STATE_WAIT)
        {
            q[i].state = STATE_DONE;
        }
    }
}

void ORDERS::execute(void)
{
    char exe_buf[MAX_STRING_LENGTH] = {'\0'}; /* ウィンドウ表示用 */
    for (size_t i = 0; i < q.size(); i++)
    {
        if (q[i].state == STATE_DOING)
        {
            strcat(exe_buf, ", ");
            strcat(exe_buf, q[i].name); /* ウィンドウ表示用 */
            switch (q[i].mode)
            {
            case MODE_TEST:
                q[i].mode_test();
                break;
            case MODE_WAIT:
                q[i].mode_wait();
                break;
            case MODE_ENCODER:
                q[i].mode_encoder(exe_buf);
                break;
            case MODE_LIMIT_CHASSIS:
                q[i].mode_limit_chassis();
                break;
            case MODE_ANGLE:
                q[i].mode_angle(exe_buf);
                break;
            case MODE_SERVO:
                q[i].mode_servo();
                break;
            case MODE_SET_ODOMETRY:
                q[i].mode_set_odometry();
                break;
            case MODE_ALONG_LINE:
                q[i].mode_along_line(exe_buf);
                break;
            case MODE_DETECT_LINE:
                q[i].mode_detect_line();
                break;
            case MODE_POSITIONING_LINE:
                q[i].mode_positioning_line();
                break;
            case MODE_SET_SPIN:
                q[i].mode_set_spin();
                break;
            case MODE_TURN:
                q[i].mode_turn();
                break;
            case MODE_MOTOR:
                q[i].mode_motor();
                break;
            default:
                PrintError("ORDERS::execute()", "invalid mode", q[i].mode);
                _Flags.e(EXIT_FAILURE);
            }
        }
    }
    /* 指定回数毎に Executing を更新 */
    write_exe(exe_buf);
}

/*-----------------------------------------------
テキストファイルから ORDER を読み込む
-----------------------------------------------*/
void ORDERS::load(void)
{
    set_order_path();
    FILE *fp = Myfopen(order_path, "r");

    char name0[MAX_STRING_LENGTH], mode0[MAX_STRING_LENGTH];
    int seq0[3];

    printf("-----------------------------\n");

    while (fscanf(fp, "%s\n", name0) != EOF)
    {
#if ENABLE_ORDER_DEBUG
        printf("%s\n", name0);
#endif

        /* seq, mode */
        fscanf(fp, "%d %d %d %s\n", &seq0[0], &seq0[1], &seq0[2], mode0);
        ORDER tmp(seq0[0], seq0[1], seq0[2], name0);
        tmp.judge_mode(mode0); /* モード判定 */
        tmp.load_par(fp);      /* パラメータ読み込み */

        /* コメントになっている処理を除外 */
        if (strlen(name0) >= 2)
        {
            if (name0[0] == '/' && name0[1] == '/')
            {
                continue;
            }
        }

        /* キューに追加 */
        q.emplace_back(tmp);
    }
    fclose(fp);
    printf("-----------------------------\n");
}

void ORDERS::set_order_path(void)
{
/*-----------------------------------------------
チーム
-----------------------------------------------*/
#ifdef TEAM_A
    strcpy(order_path, "Orders_A/");
#endif
#ifdef TEAM_B
    strcpy(order_path, "Orders_B/");
#endif

    /*-----------------------------------------------
    予選，決勝
    -----------------------------------------------*/
    if (_Switch.d->isfinal == true)
    {
        strcat(order_path, "final/");
    }
    else
    {
        strcat(order_path, "pre/");
    }

    /*-----------------------------------------------
    赤ゾーン，青ゾーン
    -----------------------------------------------*/
    if (_Switch.d->field == FIELD_RED)
    {
        strcat(order_path, "R");
    }
    else
    {
        strcat(order_path, "B");
    }

    size_t tmp_len = strlen(order_path);

    /*-----------------------------------------------
    回収
    -----------------------------------------------*/
    if (_Switch.d->collect == true)
    {
        strcat(order_path, "_C.txt");
        return; /* 回収と洗濯物干しを同時に行うことはない */
    }

    /*-----------------------------------------------
    バスタオル
    -----------------------------------------------*/
    if (_Switch.d->bath1 == true)
    {
        strcat(order_path, "_B1");
    }
    if (_Switch.d->bath2 == true)
    {
        strcat(order_path, "_B2");
    }

    /*-----------------------------------------------
    シーツ
    -----------------------------------------------*/
    if (_Switch.d->sheet == true)
    {
        strcat(order_path, "_S");
    }

    /*-----------------------------------------------
    拡張子
    -----------------------------------------------*/
    strcat(order_path, ".txt");

    /*-----------------------------------------------
    テストモード
    -----------------------------------------------*/
    /* C, B1, B2, S がすべて OFF だったらテストモード */
    if (tmp_len == strlen(order_path) - 4)
    {
        if (order_path[7] == 'A')
        {
            strcpy(order_path, "Orders_A/test.txt");
        }
        else if (order_path[7] == 'B')
        {
            strcpy(order_path, "Orders_B/test.txt");
        }
        else
        {
            printf("%s\n", order_path);
            printf("%c\n", order_path[7]);
            _Flags.e(EXIT_FAILURE);
        }
    }
}