# jibiki::ProcOperateAuto サンプル
[戻る](quick_proc_operate_auto.md/#サンプル)

# 概要
本ページでは [`program/operate_auto`](../../program/operate_auto) のプログラムを解説する

# プログラム上で動くスレッド
* サンプルプログラム中でユーザーが直接関与しているスレッドは以下の３つ（`setting.json` に記載されている）．
    * 実際には `operate_auto` 内で必要に応じてスレッドが作成されるため，`orders` によって作成されるスレッド数が変動する．

|スレッド名|説明|
|:-|:-|
|operate_auto|`jibiki::ProcOperateAuto` によって管理されるスレッド．<br>自動制御に関係する処理を行う．|
|kbhit|`jibiki::kbhit` によるキー入力の受け付けと，キー入力に対する処理を行うスレッド．|
|display|opencv を用いてウィンドウで `executing_order` を表示し，`cv::waitKey` によるキー入力を受け付けるスレッド．|


# operate_auto
実装されている `order` は `test` のみ（`mode_func.cpp` で定義されている）．プログラムを以下に示す．
```C++
void test(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ読み込み */
    size_t wait_time = std::stoi(param[0]);

    printf("[ %d, %d, %d ] start %d[s]\n",
           seq[0], seq[1], seq[2], wait_time);

    /* 指定時間待機 */
    for (size_t i = 0; i < wait_time; ++i)
    {
        /* スレッドの管理 */
        if (!control->manage_thread_int())
            break;

        /* executing_order に書き込む */
        std::stringstream sstr;
        sstr << wait_time - i << " [s]";
        control->set_executing_order(seq, sstr.str());

        /* 待機 */
        usleep(1E6);
    }

    printf("\t\t\t[ %d, %d, %d ] finish\n", seq[0], seq[1], seq[2]);

    /* executing_order をクリア */
    control->clear_executing_order(seq);
}
```
* `test` パラメータで指定された時間だけ待機する．
* `usleep` で長時間待機するのは好ましくないため１
秒待機をループしている．
* `jibiki::ProcOperateAuto` 内のループでは `manage_thread_int` を呼び出す必要がある．
* `set_executing_order`，`clear_executing_order` を呼び出すことで `executing_order` を操作できる．
* `executing_order` は `jibiki::ProcOperateAuto` のオブジェクト作成時に渡す引数から取り出せる．

# display
```C++
const cv::Size g_window_size(450, 420);

void process_display(jibiki::ShareVal<bool> &exit_flag,
                     jibiki::ShareVal<int> &pushed_key,
                     jibiki::ShareValVec<std::string> &executing_order)
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
```
* `cv::Mat` のオブジェクトにデータを準備して，`cv::imshow` でウィンドウに描画する．
    * `cv::imshow` の実行時に指定するウィンドウは `cv::namedWindow`  で作成する．
* `cv::Mat` オブジェクトに文字列を描画するのには `cv::putText` を使用する．
* 描画内容を更新するために `cv::imshow` でウィンドウに描画した後，以下の処理で `cv::Mat` オブジェクトを初期化する．
```C++
img = cv::Mat::zeros(g_window_size, CV_8UC3);
```
* opencv でウィンドウを使って描画処理を行う際は必ず `cv::waitKey` を呼ぶ必要がある．
    * `cv::waitKey` で実行される処理の一つにキー入力を受け付ける処理がある．
    * キー入力に対する処理は別のスレッド（`kbhit`）で行うため，ここでは `pushed_key` に入力されたキーの値を渡すだけになっている．
# kbhit
```C++
void thread_kbhit(jibiki::ShareVal<bool> &exit_flag,
                  jibiki::ShareVal<bool> &start_flag,
                  jibiki::ShareVal<bool> &reset_flag,
                  jibiki::ShareVal<int> &pushed_key,
                  jibiki::ShareValVec<std::string> &executing_order)
{
    if (!jibiki::thread::enable("kbhit"))
        return;

    while (jibiki::thread::manage(exit_flag))
    {
        if (jibiki::kbhit())
            pushed_key = getchar();

        switch (pushed_key.read())
        {
        case 's':
            start_flag = start_flag.read() ^ 1;
            // std::cout << "start : " << start_flag.read() << std::endl;
            break;
        case 'r':
            reset_flag = reset_flag.read() ^ 1;
            // std::cout << "reset : " << reset_flag.read() << std::endl;
            break;
        case 'p':
            for (size_t i = 0; i < executing_order.size(); ++i)
                std::cout << "\t\t" << executing_order.read(i) << std::endl;
            break;
        case 'q':
            exit_flag = true;
            break;
        }

        if (pushed_key.read() != 0)
            pushed_key = 0;
    }
}
```
* `jibiki::kbhit` はキー入力があったときに `true` を返すため， if 文中で `getchar()` を呼び出して入力データを読み出す．
* `pushed_key` の値に関する switch-case 文で入力されたキーに対する処理を行う．
    * `pushed_key` は別のスレッド（`display`）とも共有されているため，上記の switch-case 文では `cv::waitKey` によるキー入力に対する処理も含まれる．
* キー入力に対する処理が重複して実行されないように，処理後に `pushed_key` を 0 で初期化している．

# `jibiki::kbhit` と `cv::waitKey` の違い
* `jibiki::kbhit` と `cv::waitKey` はいずれもキー入力を受け付ける関数であるが，その違いは入力を受け付ける対象にある．
* 前者はターミナルのウィンドウ，後者は opencv のウィンドウ（`cv::namedWindow` によって生成される）を対象としている．
* プログラムの実行状態によっては opecv のウィンドウが新たに生成されたり消えたりすることがあるため，キー入力の対象をどちらか片方に制限せずに両方の入力を受け付けられるようにしている．