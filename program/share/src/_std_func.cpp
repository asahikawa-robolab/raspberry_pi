/* Last updated : 2020/10/06, 00:38 */
/* C++ */
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
/* UNIX */
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
/* jibiki */
#include "../inc/_std_func.hpp"

namespace jibiki
{
    /*-----------------------------------------------
     *
     * キー入力
     *
    -----------------------------------------------*/
    int kbhit(void) noexcept
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
     * 文字列 str を区切り文字 sep で分割する
     *
    -----------------------------------------------*/
    std::vector<std::string> split(const std::string &str, char sep)
    {
        std::vector<std::string> v;
        std::stringstream ss(str);
        std::string buffer;

        while (std::getline(ss, buffer, sep))
            v.emplace_back(buffer);

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
        /* ファイルを読み込む */
        std::ifstream file(path, std::ios::in);
        if (file.fail())
        {
            std::stringstream sstr;
            sstr << path << " が開けませんでした．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }
        const std::string str_raw((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        file.close();

        /* 解析 */
        picojson::value json_val;
        const std::string err = picojson::parse(json_val, str_raw);
        if (!err.empty())
        {
            jibiki::print_err(__PRETTY_FUNCTION__, err);
            throw std::runtime_error(""); /* エラー発生 */
        }

        return json_val;
    }

    /*-----------------------------------------------
    *
    * エラーメッセージを表示
    *
    -----------------------------------------------*/
    void print_err(const char *func, std::string err_msg) noexcept
    {
        std::cout << "\n*** error ***\t" << func << '\n'
                  << err_msg << '\n';
        if (err_msg != "")
            std::cout << '\n';
    }

    /*-----------------------------------------------
    *
    * 時間差を計算
    *
    -----------------------------------------------*/
    double calc_sec(time_point start, time_point end)
    {
        double duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return duration * 1E-6;
    }

} // namespace jibiki