# jibiki::thread ::manage サンプル
[戻る](spec_thread.md/#jibikithread-manage)

# 目次
1. [基本的な使い方](#例１基本的な使い方)
2. [多重ループ](#例２多重ループ)
3. [操作方法を考慮（一時停止）](#例３操作方法を考慮一時停止)

# 例１）基本的な使い方
```C++
#include <thread>
#include <unistd.h>                      /* usleep */
#include "../../share/inc/_std_func.hpp" /* キー入力 */
#include "../../share/inc/_thread.hpp"

void func1(jibiki::ShareVar<bool> &exit_flag)
{
    while (jibiki::thread::manage(exit_flag))
    {
        /* キー入力 */
        if (jibiki::kbhit())
        {
            switch (getchar())
            {
            case 'q':
                exit_flag = true;
                break;
            }
        }
    }
}

void func2(jibiki::ShareVar<bool> &exit_flag)
{
    while (jibiki::thread::manage(exit_flag))
    {
        usleep(1E6);
        std::cout << "hello" << std::endl;
    }
}

int main(void)
{
    jibiki::ShareVar<bool> exit_flag(false); /* 終了フラグ */

    std::thread t1(func1, std::ref(exit_flag));
    std::thread t2(func2, std::ref(exit_flag));

    t1.join();
    t2.join();

    return 0;
}
```
* プログラムを実行すると１秒毎に `hello` が出力される．
* キーボードの `q` を入力するとすべてのスレッドが終了し，プログラムも終了する．
* `jibiki::thread::manage()` を各スレッドのループで呼び出すことによって，`exit_flag` でスレッドを終了させることができるようになる．

# 例２）多重ループ
```C++
#include <thread>
#include <unistd.h>                      /* usleep */
#include "../../share/inc/_std_func.hpp" /* キー入力 */
#include "../../share/inc/_thread.hpp"

void func1(jibiki::ShareVar<bool> &exit_flag)
{
    while (jibiki::thread::manage(exit_flag))
    {
        /* キー入力 */
        if (jibiki::kbhit())
        {
            switch (getchar())
            {
            case 'q':
                exit_flag = true;
                break;
            }
        }
    }
}

void func2(jibiki::ShareVar<bool> &exit_flag)
{
    while (jibiki::thread::manage(exit_flag))
    {
        for (size_t i = 0; i < 5; ++i)
        {
            if (!jibiki::thread::manage(exit_flag))
                break;

            usleep(1E6);
            std::cout << i + 1 << " / 5" << std::endl;
        }
    }
}

int main(void)
{
    jibiki::ShareVar<bool> exit_flag(false); /* 終了フラグ */

    std::thread t1(func1, std::ref(exit_flag));
    std::thread t2(func2, std::ref(exit_flag));

    t1.join();
    t2.join();

    return 0;
}
```
* 例１の `func2()` の `while` 内に `for` によるループが追加されている．
* プログラムを実行すると１秒毎に `x / 5` が出力される（x：1～5）．
* キーボードの `q` を入力するとすべてのスレッドが終了し，プログラムも終了する．
* スレッド内に多重ループがある場合は１つのループにつき `jibiki::thread::manage()` が１個必要．
    * 二重ループだったら `jibiki::thread::manage()` が２個，三重ループだったら３個必要になる．
    * 要するに<u>**`jibki::thread::manage()` の戻り値として `false` が帰ってきたら直ちにループを抜けるようにすればよい．**</u>
    * もしも `func2` の `for` 内部の `jibiki::thread::manage()` が無ければ，キーボードの `q` を入力しても 5 までカウントし終わるまでプログラムは終了しなくなってしまう．

# 例３）操作方法を考慮（一時停止）
```C++
#include <iostream>
#include <thread>
#include <unistd.h>                      /* usleep */
#include "../../share/inc/_std_func.hpp" /* キー入力 */
#include "../../share/inc/_thread.hpp"

void process_operate_manual(
    jibiki::ShareVar<bool> &exit_flag,
    jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method)
{
    while (jibiki::thread::manage(exit_flag,
                                  current_method,
                                  jibiki::thread::OPERATE_MANUAL))
    {
        /* ロボットをコントローラ等で手動制御する処理 */
        std::cout << "operate_manual" << std::endl;
        usleep(1E6);
    }
}

void process_operate_auto(
    jibiki::ShareVar<bool> &exit_flag,
    jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method)
{
    while (jibiki::thread::manage(exit_flag,
                                  current_method,
                                  jibiki::thread::OPERATE_AUTO))
    {
        /* ロボットを自動制御する処理 */
        std::cout << "\t\toperate_auto" << std::endl;
        usleep(1E6);
    }
}

void process_keyboard(
    jibiki::ShareVar<bool> &exit_flag,
    jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method)
{
    while (jibiki::thread::manage(exit_flag))
    {
        /* キー入力 */
        if (jibiki::kbhit())
        {
            switch (getchar())
            {
            case 's':
                /* 操作方法を変更 */
                current_method = current_method.read() == jibiki::thread::OPERATE_MANUAL
                                     ? jibiki::thread::OPERATE_AUTO
                                     : jibiki::thread::OPERATE_MANUAL;
                break;
            case 'q':
                /* 終了 */
                exit_flag = true;
                break;
            }
        }
    }
}

int main(void)
{
    jibiki::ShareVar<bool> exit_flag(false);        /* 終了フラグ */
    jibiki::ShareVar<jibiki::thread::OperateMethod> /* 現在使用している操作方法 */
        current_method(jibiki::thread::OPERATE_MANUAL);

    /* 手動制御の処理を行うスレッド */
    std::thread t_manual(process_operate_manual,
                         std::ref(exit_flag),
                         std::ref(current_method));
    /* 自動制御の処理を行うスレッド */
    std::thread t_auto(process_operate_auto,
                       std::ref(exit_flag),
                       std::ref(current_method));
    /* キーボードで操作する処理を行うスレッド */
    std::thread t_keyboard(process_keyboard,
                           std::ref(exit_flag),
                           std::ref(current_method));

    t_manual.join();
    t_auto.join();
    t_keyboard.join();

    return 0;
}
```
* 各スレッドに`手動制御`，`自動制御`，`操作と無関係`の３つの役割を割り当てて，`current_method` の値に応じてスレッドの処理を一時停止，再開させる．
* プログラムを実行すると
    * (a). `current_method == jibiki::thread::OPERATE_MANUAL` のときは
        * `process_operate_manual` を実行しているスレッドを実行し，
        * `process_operate_auto` を実行しているスレッドをブロッキング（一時停止）し，
        * `"operate_manual"` を１秒おきに出力する．
    * (b). `current_method == jibiki::thread::OPERATE_AUTO` のときは
        * `process_operate_manual` を実行しているスレッドをブロッキング（一時停止）し，
        * `process_operate_auto` を実行しているスレッドを実行し，
        * `"operate_auto"` を１秒おきに出力する．
    * (c). `process_keyboard` を実行しているスレッドは `current_method` の値に関係なく常に実行される．
* これらの動作の違いは各スレッドのループで呼び出す `jibiki::thread::manage()` の引数の違いによって生じている．
    * (a). `my_method` に `jibiki::thread::OPERATE_MANUAL` を指定．
    * (b). `my_method` に `jibiki::thread::OPERATE_AUTO` を指定．
    * (c). `my_method`，`current_method` を省略して `exit_flag` のみ指定．
* キーボードの `s` を入力すると `current_method` の値が切り替わる（`jibiki::thread::OPERATE_MANUAL` ⇔ `jibiki::thread::OPERATE_AUTO`）．
* キーボードの `q` を入力するとすべてのスレッドが終了し，プログラムも終了する．