# SwitchData
[戻る](overview.md/#SwitchData)

# 目次
1. [ヘッダファイル](#1-ヘッダファイル)
2. [データの準備](#2-データの準備)
3. [データの読み出し](#3-データの読み出し)

# 1. ヘッダファイル
```C++
#include "../../share/inc/module.hpp"
```

# 2. データの準備
`SwitchData::set` でデータを準備する．以下に例を示す．
```C++
void com_controller(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 0, 8, B57600, name, false);

    /* 受信データを読み込ませる */
    if (com.receive())
        g_controller.set(com);
}
```

# 3. データの読み出し
`SwitchData::push_l()`, `SwitchData::push_r()`, `SwitchData::toggle()`, `SwitchData::lu()`, `SwitchData::ld()`, `SwitchData::ru()`, `SwitchData::rd()`, `SwitchData::slide()` でデータを読み出す．