/*-----------------------------------------------
 *
 * Last updated : 2020/08/23, 16:03
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "../inc/_std_func.hpp"

namespace jibiki
{

/*-----------------------------------------------
 *
 * キー入力
 *
-----------------------------------------------*/
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

/*-----------------------------------------------
 *
 * 現在時刻の取得
 *
-----------------------------------------------*/
struct timespec get_time(void)
{
    struct timespec tmp;
    timespec_get(&tmp, TIME_UTC);
    return tmp;
}

/*-----------------------------------------------
 *
 * 文字列 str を区切り文字 sep で分割する
 *
-----------------------------------------------*/
std::vector<std::string> split(const std::string &str, char sep)
{
    std::vector<std::string> v;
    std::stringstream ss(str);
    std::string buffer;
    while (std::getline(ss, buffer, sep))
    {
        v.push_back(buffer);
    }
    return v;
}

/*-----------------------------------------------
 *
 * path に指定した json ファイルを picojson で読み込む．
 * ファイルが開けなかったら std::string 型の例外を投げる
 *
-----------------------------------------------*/
picojson::value load_json_file(std::string path)
{
    /* using 宣言 */
    using picojson::array;
    using picojson::object;

    /* ファイルを読み込む */
    std::ifstream file(path, std::ios::in);
    if (file.fail())
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "\n"
             << "ファイルが開けませんでした" << std::endl
             << path << std::endl;
        throw sstr.str();
    }
    const std::string str_raw((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    file.close();

    /* 解析 */
    picojson::value json_value;
    const std::string err = picojson::parse(json_value, str_raw);
    if (!err.empty())
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "\n"
             << err << std::endl;
        throw sstr.str();
    }

    return json_value;
}

} // namespace jibiki