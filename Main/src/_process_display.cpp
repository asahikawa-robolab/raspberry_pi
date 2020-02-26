#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "../../Share/inc/_std_func.hpp"
#include "../inc/external_variable.hpp"
#include "../inc/_utility.hpp"
#include "../inc/config.hpp"

/* using 宣言 */
using picojson::array;
using picojson::object;

/* exctern 宣言 */
extern void func_draw_main(cv::Mat &img);

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void process_display(void)
{
    printf("[start display]\n");
    atexit([]() { printf("[end display]\n"); });

    try
    {
        /*-----------------------------------------------
        設定ファイル読み込み
        -----------------------------------------------*/
        picojson::value json_value;
        json_value =
            jibiki::load_json_file("setting.json");
        /* 終了 */
        if (json_value
                .get<object>()["enable"]
                .get<object>()["process_display"]
                .get<bool>() == false)
            return;

        /*-----------------------------------------------
        画像作成
        -----------------------------------------------*/
        cv::Mat img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3);

        /*-----------------------------------------------
        ウィンドウ作成
        -----------------------------------------------*/
        cv::namedWindow("info");
        cv::moveWindow("info", WINDOW_POS.x, WINDOW_POS.y);

        while (manage_process(OPERATE_NONE))
        {
            /* 表示する画像を用意 */
            func_draw_main(img);

            /* ディスプレイに描画 */
            cv::imshow("info", img);

            /* クリア */
            img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3);

            /* キー入力 */
            int key = cv::waitKey(1);
            if (key > 0)
                g_flags[FLAG_KEY] = (char)key;
        }
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }
}