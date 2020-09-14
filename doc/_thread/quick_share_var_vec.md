# jibiki::ShareVarVec
[戻る](overview.md/#jibikiShareVarVec)

# 目次
1. [ヘッダファイル](#1-ヘッダファイル)
2. [宣言](#2-宣言)
3. [末尾に要素を追加する](#3-末尾に要素を追加する)
4. [要素数を取得する](#4-要素数を取得する)
5. [要素数を変更する](#5-要素数を変更する)
6. [値を書き込む](#6-値を書き込む)
7. [値を読み出す](#7-値を読み出す)
8. [指定した要素を削除する](#8-指定した要素を削除する)
9. [すべての要素を削除する](#9-すべての要素を削除する)
10. [ 代入](#10-代入)
11. [ 関数に渡す](#11-関数に渡す)


# 1. ヘッダファイル
```C++
#include "../../share/inc/_thread.hpp"
```

# 2. 宣言
`jibiki::ShareVarVec` の後ろにテンプレート引数 `<>` を付けて型を指定する．

```C++
jibiki::ShareVarVec<int> var_int;            /* int 型 */
jibiki::ShareVarVec<uint8_t> var_uint8_t;    /* uint8_t 型 */
jibiki::ShareVarVec<std::string> var_string; /* std::string 型 */
```

# 3. 末尾に要素を追加する
`jibiki::ShareVarVec::push_back()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
```

# 4. 要素数を取得する
`jibiki::ShareVarVec::size()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
vec.push_back(11);
vec.push_back(12);

std::cout << vec.size() << std::endl;   /* 3 が出力される */
```


# 5. 要素数を変更する
`jibiki::ShareVarVec::resize()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

std::cout << vec.size() << std::endl;   /* 0 が出力される */
vec.resize(5);
std::cout << vec.size() << std::endl;   /* 5 が出力される */

```

# 6. 値を書き込む
`jibiki::ShareVarVec::write()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.resize(1);
vec.write(0, 13);   /* vec の 0 番目に 13 が書き込まれる */
```

# 7. 値を読み出す
`jibiki::ShareVarVec::read()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
std::cout << vec.read(0) << std::endl;  /* 10 が出力される */
```

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
vec.push_back(11);
vec.push_back(12);

/* 10, 11, 12 が出力される */
for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec.read(i) << std::endl;
```

# 8. 指定した要素を削除する
`jibiki::ShareVarVec::erase()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
vec.push_back(11);
vec.push_back(12);

vec.erase(1);   /* 1 番目の要素を削除 */

/* 10, 12 が出力される */
for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec.read(i) << std::endl;
```

# 9. すべての要素を削除する
`jibiki::ShareVarVec::clear()` を使用する．

```C++
jibiki::ShareVarVec<int> vec;

vec.push_back(10);
vec.push_back(11);
vec.push_back(12);

std::cout << vec.size() << std::endl;   /* 3 が出力される */
vec.clear();                            /* すべての要素を削除 */
std::cout << vec.size() << std::endl;   /* 0 が出力される */
```

# 10. 代入
`jibiki::ShareVarVec` に対して代入演算子 `operator=()` が定義されていないため，次のように他の変数に代入することはできない．

```C++
jibiki::ShareVarVec<int> vec1;
jibiki::ShareVarVec<int> vec2;

vec1.push_back(10);
vec1.push_back(11);
vec1.push_back(12);

vec2 = vec1;    /* error! */

```

# 11. 関数に渡す
コピーコンストラクタが定義されていないため，関数に `jibiki::ShareVarVec` を渡すときは必ず参照渡しを使用する．

### 例１）ダメな例（値渡し）→ コンパイルエラー
```C++
#include <iostream>
#include "../../share/inc/_thread.hpp"

void func(jibiki::ShareVarVec<int> vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
        std::cout << vec.read(i) << std::endl;
}

int main(void)
{
    jibiki::ShareVarVec<int> vec;

    vec.push_back(10);
    vec.push_back(11);
    vec.push_back(12);

    func(vec);

    return 0;
}
```

### 例２）良い例（参照渡し）
```C++
#include <iostream>
#include "../../share/inc/_thread.hpp"

void func(jibiki::ShareVarVec<int> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
        std::cout << vec.read(i) << std::endl;
}

int main(void)
{
    jibiki::ShareVarVec<int> vec;

    vec.push_back(10);
    vec.push_back(11);
    vec.push_back(12);

    func(vec);

    return 0;
}
```

上記の２つの例で異なるのは func の定義の仮引数に `&` がついているかどうかだけ．

```C++
void func(jibiki::ShareVarVec<int> &vec)
```