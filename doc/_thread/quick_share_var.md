# jibiki::ShareVar
[戻る](overview.md/#jibikiShareVar)

# 目次
1. [ヘッダファイル](#1-ヘッダファイル)
2. [宣言・初期化](#2-宣言初期化)
3. [値の書き込み](#3-値の書き込み)
4. [値の読み出し](#4-値の読み出し)
5. [演算子](#5-演算子)
6. [関数に渡す](#6-関数に渡す)

# 1. ヘッダファイル
```C++
#include "../../share/inc/_thread.hpp"
```

# 2. 宣言・初期化
### 宣言
* `jibiki::ShareVar` の後ろにテンプレート引数 `<>` を付けて型を指定する．
* STL のコンテナクラス（`std::vector` や `std::map` など）は使用できない．

```C++
jibiki::ShareVar<int> var_int;            /* int 型 */
jibiki::ShareVar<uint8_t> var_uint8_t;    /* uint8_t 型 */
jibiki::ShareVar<std::string> var_string; /* std::string 型 */
```

### 初期化
変数名の直後に丸括弧`（）`で初期値を指定することで，宣言と同時に初期化できる．

```C++
jibiki::ShareVar<int> var_int(0);
jibiki::ShareVar<uint8_t> var_uint8_t(0);
jibiki::ShareVar<std::string> var_string("str"); /* 
```

# 3. 値の書き込み
代入演算子 `operator=()` を使用する．

```C++
jibiki::ShareVar<int> data;

/* data に 10 を書き込む */
data = 10;
```

`jibiki::ShareVar` を右辺値として代入することはできない．
```C++
jibiki::ShareVar<int> data1(10);
jibiki::ShareVar<int> data2;

data2 = data1;  /* error! */
```

# 4. 値の読み出し
`jibiki::ShareVar::read()` を使用する．

```C++
jibiki::ShareVar<int> data(10);

std::cout << data.read() << std::endl;  /* 10 が出力される */
```

# 5. 演算子
* 一部の複合代入演算子 `operator+=()`，`operator-=()`, `operator^=()` が定義されている．
* 今後他の演算子も追加していく予定．

```C++
jibiki::ShareVar<int> data(0);

data ^= 1;  /* data : 1 になる */
data += 10; /* data : 11 になる */
data -= 5;  /* data : 6 になる */
```

# 6. 関数に渡す
コピーコンストラクタが定義されていないため，関数に `jibiki::ShareVar` を渡すときは必ず参照渡しを使用する．

### 例１）ダメな例（値渡し）→ コンパイルエラー
```C++
#include <iostream>
#include "../../share/inc/_thread.hpp"

void func(jibiki::ShareVar<int> data)
{
    std::cout << data.read() << std::endl;
}

int main(void)
{
    jibiki::ShareVar<int> data(13);

    func(data); /* error */

    return 0;
}
```

### 例２）良い例（参照渡し）
```C++
#include <iostream>
#include "../../share/inc/_thread.hpp"

void func(jibiki::ShareVar<int> &data)
{
    std::cout << data.read() << std::endl;
}

int main(void)
{
    jibiki::ShareVar<int> data(13);

    func(data); /* ok */

    return 0;
}
```

上記の２つの例で異なるのは func の定義の仮引数に `&` がついているかどうかだけ．

```C++
void func(jibiki::ShareVar<int> &data)
```