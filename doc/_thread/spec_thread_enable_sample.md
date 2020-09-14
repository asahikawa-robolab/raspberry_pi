# jibiki::thread ::enable サンプル
[戻る](spec_thread.md/#jibikithread-enable)

# 目次
1. [基本的な機能](#例１基本的な機能)
2. [実用的な使い方](#例２実用的な使い方)

# 例１）基本的な機能
### main.cpp
```C++
#include <iostream>
#include "../../share/inc/_thread.hpp"

int main(void)
{
    bool val1 = jibiki::thread::enable("test1");
    bool val2 = jibiki::thread::enable("test2");

    std::cout << "test1 : " << val1 << std::endl;
    std::cout << "test2 : " << val2 << std::endl;

    return 0;
}
```
### setting.json
```JSON
{
    "thread":
    {
        "test1": true,
        "test2": false
    }
}
```
### 実行結果
```
$ ./all.out 
test1 : 1
test2 : 0
```
* `setting.json` の値が `jibiki::thread::enable()` の戻り値として返ってくる．
* JSON ファイルの `"thread"` の文字列は固定．
* JSON ファイルから値を読み込む処理を省くことができる．
* 実際には以下（例２）に示す使い方をする．


# 例２）実用的な使い方
### main.cpp
```C++
#include <iostream>
#include <thread>
#include <exception>                     /* 例外処理 */
#include <unistd.h>                      /* usleep */
#include "../../share/inc/_std_func.hpp" /* キー入力 */
#include "../../share/inc/_thread.hpp"

void func1(jibiki::ShareVar<bool> &exit_flag)
{
    try
    {
        /* enable が false だったら即終了 */
        /* 引数の "test" は JSON ファイルの "test" と一致させる */
        if (!jibiki::thread::enable("test1"))
            return;

        while (jibiki::thread::manage(exit_flag))
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                exit_flag = true;
                break;
            }

            std::cout << "hello1\n" << std::flush;
            usleep(1E6);
        }
    }
    catch (std::string err)
    {
        std::cout << err << std::endl;
        exit_flag = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit_flag = true;
    }
}

void func2(jibiki::ShareVar<bool> &exit_flag)
{
    try
    {
        if (!jibiki::thread::enable("test2"))
            return;

        while (jibiki::thread::manage(exit_flag))
        {
            std::cout << "\thello2\n" << std::flush;
            usleep(1E6);
        }
    }
    catch (std::string err)
    {
        std::cout << err << std::endl;
        exit_flag = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit_flag = true;
    }
}

int main(void)
{
    jibiki::ShareVar<bool> exit_flag; /* 終了フラグ */

    std::thread t1(func1, std::ref(exit_flag));
    std::thread t2(func2, std::ref(exit_flag));

    t1.join();
    t2.join();

    return 0;
}
```
### setting.json
```json
{
    "thread":
    {
        "test1": true,
        "test2": true
    }
}
```
* `setting.json` の `testx` （x：1 or 2）に対応する値が
    * `true` であれば `funcx()` を実行するスレッドが実行され続け，一秒おきに `"hellox"` が出力される．
    * `false` であれば `funcx()` を実行するスレッドはすぐに終了する．