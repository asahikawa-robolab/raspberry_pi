# std::thread
[戻る](../../README.md/#リファレンス)

# 概要
* スレッドを使って並列処理を行うためのクラス．
* ここでは基本的な使い方を説明する．
* 最低でも [3. 引数付きの関数を渡す（参照渡し）](#3-引数付きの関数を渡す参照渡し) までは理解すること．

# 目次
1. [引数なしの関数を渡す](#1-引数なしの関数を渡す)
2. [引数付きの関数を渡す（値渡し）](#2-引数付きの関数を渡す値渡し)
3. [引数付きの関数を渡す（参照渡し）](#3-引数付きの関数を渡す参照渡し)
4. [（発展）メンバ関数を渡す](#4-発展メンバ関数を渡す)

# 1. 引数なしの関数を渡す
### main.cpp
```C++
#include <iostream>
#include <thread>
#include <unistd.h> /* usleep */

void func1(void)
{
    for (size_t i = 0; i < 6; ++i)
    {
        usleep(1E6);
        std::cout << "func1\n" << std::flush;
    }
}

void func2(void)
{
    for (size_t i = 0; i < 2; ++i)
    {
        usleep(3E6);
        std::cout << "\tfunc2\n" << std::flush;
    }
}

int main(void)
{
    std::thread t1(func1);
    std::thread t2(func2);

    t1.join();
    t2.join();

    return 0;
}
```
### コンパイル
```bash
$ g++ main.cpp -pthread
```
### 実行結果
```
$ ./a.out 
func1
func1
    func2
func1
func1
func1
	func2
func1

```
* `usleep` は引数に数値で指定した時間 [μs] だけプログラムをブロッキング（停止）する関数（`1E6` は `1 * 10^6` という意味）．
* 戻り値と引数の型がともに `void` の関数（`func1`，`func2`）を用意し，`std::thread` オブジェクトの作成時に引数として関数を渡すことで `func1`，`func2` を並列で実行できる．
* `std::thread` のオブジェクトが破棄される前に [`std::thread::join`](https://cpprefjp.github.io/reference/thread/thread/join.html) か [`std::thread::detach`](https://cpprefjp.github.io/reference/thread/thread/detach.html) を呼び出さなければならない（特別な事情がなければ `join` を使う）．

# 2. 引数付きの関数を渡す（値渡し）
### main.cpp
```C++
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h> /* usleep */

void func(size_t num, size_t duration, std::string str)
{
    for (size_t i = 0; i < num; ++i)
    {
        usleep(duration * 1E6);
        std::cout << str << "\n" << std::flush;
    }
}

int main(void)
{
    std::thread t1(func, 6, 1, "func1");
    std::thread t2(func, 2, 3, "\tfunc2");

    t1.join();
    t2.join();

    return 0;
}
```
### コンパイル
```bash
$ g++ main.cpp -pthread
```
### 実行結果
```
$ ./a.out 
func1
func1
	func2
func1
func1
func1
	func2
func1

```
* `size_t num, size_t duration, std::string str` を引数に持つ関数 `func` が定義されている．
* `std::thread` のオブジェクトを作成するときに一つ目の引数に関数（`func`）を渡し，二つ目以降の引数に `func` の引数を渡す．

# 3. 引数付きの関数を渡す（参照渡し）
### main.cpp
```C++
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>                      /* usleep */
#include "../../share/inc/_thread.hpp"   /* jibiki::ShareVal */
#include "../../share/inc/_std_func.hpp" /* jibiki::kbhit */

void func(jibiki::ShareVal<bool> &exit_flag)
{
    while (1)
    {
        if (exit_flag.read())
            break;
        usleep(1E6);
        std::cout << "func\n" << std::flush;
    }
}

int main(void)
{
    /* 終了フラグ */
    jibiki::ShareVal<bool> exit_flag(false);

    /* スレッドを作成 */
    std::thread t1(func, std::ref(exit_flag));

    /* キー入力があったら exit_flag をセット */
    while (1)
    {
        if (jibiki::kbhit())
        {
            std::cout << "kbhit\n" << std::flush;
            exit_flag = true;
            break;
        }
    }

    /* スレッドの終了を待機 */
    t1.join();

    return 0;
}
```
### コンパイル
```bash
$ make -j4
```
### 実行結果
```
$ ./all.out 
func
func
func
kbhit
func

```
* 引数に参照を持つ関数をスレッドに渡すときは，`std::ref` をつける．
* [`jibiki::ShareVal`](../_thread/overview.md/#jibikiShareVal) は内部で排他制御を行ってデータ競合を防ぎ，スレッド間で変数を共有できるようにするクラス．
* `jibiki::kbhit` はキー入力を受け付ける関数で，キー入力があれば `true` を返す．
* このように参照を使うことでスレッドを外部から終了させることができる．
* 実行結果に関して，`"kbhit"` が出力された後にも `"func"` が出力されているのは `func` 中で `usleep` が呼び出されており，`exit_flag` が `true` になってもすぐに無限ループを抜ける処理に入らないため．

# 4. （発展）メンバ関数を渡す
### main.cpp
```C++
#include <iostream>
#include <thread>
#include <string>

class MyClass
{
public:
    void func(void)
    {
        std::thread t(&MyClass::print, this, "test");
        t.join();
    }
    void print(std::string str)
    {
        std::cout << str << std::endl;
    }
};

int main(void)
{
    MyClass my_class;
    my_class.func();

    return 0;
}
```
### コンパイル
```bash
$ make -j4
```
### 実行結果
```
$ ./all.out 
test

```
* メンバ関数は次のようにスレッドに渡す．
```C++
std::thread t(&MyClass::print, this, "test");
```
* メンバ関数の前には `&クラス名::` を付ける（今回の場合は `&MyClass::`）．
* `std::thread` の二つ目の引数にはオブジェクト自身のアドレスを表す `this` を指定する．
* メンバ関数の引数は三つ目以降の引数に指定する．