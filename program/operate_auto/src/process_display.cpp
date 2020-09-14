#include <opencv2/opencv.hpp>
#include "../../share/inc/_thread.hpp"

const cv::Size g_window_size(450, 420);

void process_display(jibiki::ShareVar<bool> &exit_flag,
                     jibiki::ShareVar<int> &pushed_key,
                     jibiki::ShareVarVec<std::string> &executing_order)
{
    if (!jibiki::thread::enable("display"))
        return;

    cv::Mat img = cv::Mat::zeros(g_window_size, CV_8UC3); /* 画像を作成 */
    cv::namedWindow("window");                            /* ウィンドウを作成 */

    while (jibiki::thread::manage(exit_flag))
    {
        /* 画像に文字列を描画 */
        cv::putText(img,
                    "executing_order",
                    cv::Point(7, 25),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.6,
                    cv::Scalar(255, 255, 255));
        for (size_t i = 0; i < executing_order.size(); ++i)
            cv::putText(img,
                        executing_order.read(i).c_str(),
                        cv::Point(7, 25 * i + 50),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.6,
                        cv::Scalar(255, 255, 255));

        cv::imshow("window", img);                    /* ウィンドウに描画 */
        img = cv::Mat::zeros(g_window_size, CV_8UC3); /* 画像をクリア */
        pushed_key = cv::waitKey(1);                  /* キー入力 */
    }
}