# jibiki::thread 詳細
[戻る](overview.md/#jibikithread)

# jibiki::thread ::manage
### 宣言
```C++
bool manage(ShareVal<bool> &exit_flag);                 // (1)

bool manage(ShareVal<bool> &exit_flag,
            ShareVal<OperateMethod> &current_method,
            OperateMethod my_method);                   // (2)
```
### 概要
* スレッドの終了，一時停止を管理する関数．
* スレッド内のループにおいて，ループを一周する毎に必ず一度呼び出す必要がある．
* (1)：`exit_flag` によるスレッドを管理のみを行う（終了）．
* (2)：`exit_flag` によるスレッドを管理に加えて `OperateMethod` による管理も行う（終了，一時停止）．
### 引数
|引数名|説明|
|:-|:-|
|exit_flag|終了フラグ．<br>終了する：`true`<br>終了しない：`false`|
|current_method|現在使用している操作方法．<br>操作とは無関係なスレッドで呼び出す場合は省略する．<br>自動制御：`jibiki::thread::OPERATE_AUTO`<br>手動制御：`jibiki::thread::OPERATE_MANUAL`|
|my_method|この関数を呼び出すスレッドで管理する操作方法．<br>操作とは無関係なスレッドで呼び出す場合は省略する．<br>自動制御：`jibiki::thread::OPERATE_AUTO`<br>手動制御：`jibiki::thread::OPERATE_MANUAL`|
### 戻り値
* スレッドを続行：`true`
* スレッドを終了：`false`
### 例外
なし
### コメント
* `exit_flag == true` になると戻り値として `false` を返す（終了）．
* `(my_method != current_method) & !exit_flag` のときは関数内部でブロッキングされる（一時停止）．
* 関数を呼び出すときに `my_method`，`current_method` を省略したら自動的に `my_method == current_method` となる．
* 戻り値として `false` が帰ってきたら直ちにスレッドが終了するようなプログラムにしなければならない．
* 参照はコンストなオブジェクトしかデフォルト引数にとれないため，引数の省略はオーバーロードで対応している．
### サンプル
[長いため別ページ](spec_thread_manage_sample.md)

### 関連
* usleep
* std::thread
* jibiki::kbhit
* [jibiki::ShareVal](overview.md/#jibikiShareVal)
* [jibiki::thread ::enable](#jibikithread-enable)
* [jibiki::thread ::OperateMethod](#jibikithread-OperateMethod)
* jibiki::ProcOperateAuto

# jibiki::thread ::enable
### 宣言
```C++
bool enable(std::string thread_name,
            std::string json_path = "setting.json");
```
### 概要
* スレッドを実行するかどうかを JSON ファイルで管理できるようにする関数．
* 関数自体は JSON ファイルに記載した `true` / `false` の値を返すのみ．
### 引数
|引数名|説明|
|:-|:-|
|thread_name|管理するスレッドを区別する任意の文字列．|
|json_path|スレッドの有効／無効のデータを読み込む JSON ファイルのパスを指定．<br>省略すると `"setting.json"` が指定される．|
### JSON ファイルについて
JSON ファイルは次のフォーマットで記述する．
```
{
    "thread":
    {
        "thread_name1" : true,
        "thread_name2" : false,
            ・
            ・
            ・ 
        "thread_namex": true
    }
}
```
* 最初のブロックの `"thread"` は必須．
* `"thread_name1"`，`"thread_name2"`，`...`，`"thread_namex"` には `jibiki::thread::enable` の引数 `thread_name` に指定した文字列を記述する．
* スレッドを実行する場合は `true`，実行しない場合は `false` を指定する．
### 戻り値
* JSON ファイルに記載した `true` / `false` の値が戻ってくる．
* スレッドを続行：`true`
* スレッドを終了：`false`
### 例外
|例外の型|例外が発生する条件|
|:-|:-|
|std::string|・ `json_path` に指定したファイルが開けなかったとき．<br>・ JSON ファイルのフォーマットがおかしいとき．|
|std::exception|・ JSON ファイルに `thread_name` で指定された文字列の項目がないとき．|
### サンプル
[長いため別ページ](spec_thread_enable_sample.md)

### 関連
* usleep
* std::thread
* jibiki::kbhit
* [jibiki::ShareVal](overview.md/#jibikiShareVal)
* [jibiki::thread ::manage](#jibikithread-manage)

# jibiki::thread ::OperateMethod
### 宣言
```C++
typedef enum
{
    OPERATE_NONE,     /* 操作とは無関係 */
    OPERATE_AUTO,     /* 自動 */
    OPERATE_MANUAL,   /* 手動 */
} OperateMethod;
```
### 概要
* `jibiki::thread::manage` で使用する列挙型．
* ユーザーが実際に使用するのは `OPERATE_AUTO` と `OPERATE_MANUAL` のみ．

### 関連
* [jibiki::thread ::manage](#jibikithread-manage)