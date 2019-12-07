#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../../Share/inc/_StdFunc.hpp"
#include "../../Main/inc/Orders.hpp"
using namespace std;

#define ERROR_SEQ 0b001
#define ERROR_MODE 0b010
#define ERROR_PARAM 0b100

/*-----------------------------------------------
 *
 * クラス
 *
-----------------------------------------------*/
class CHECK_ORDER
{
public:
    string name, mode_str;
    vector<string> param;
    int seq[3], mode;

    void clear(void);
    void print(void);
};

void CHECK_ORDER::clear(void)
{
    name.clear();
    param.clear();
    seq[0] = seq[1] = seq[2] = 0;
    mode = 0;
}

void CHECK_ORDER::print(void)
{
    cout << "-------------------------------\n"
         << name << "\n"
         << seq[0] << " " << seq[1] << " " << seq[2] << " " << _Modes[mode][0] << endl;
    cout << " " << flush;
    for (size_t i = 0; i < param.size(); i++)
    {
        cout << param[i] << " " << flush;
    }
    cout << endl;
}

/*-----------------------------------------------
 *
 * プロトタイプ宣言
 *
-----------------------------------------------*/
void GetPath(vector<string> &path);
void LoadOrder(vector<CHECK_ORDER> &Orders, string path);
void CheckSeq(vector<CHECK_ORDER> &Orders);
void CheckMode(vector<CHECK_ORDER> &Orders);
void CheckParam(vector<CHECK_ORDER> &Orders);
std::vector<std::string> Split(const std::string &str, char sep);

/*-----------------------------------------------
 *
 * グローバル変数
 *
-----------------------------------------------*/
uint8_t _Error;
vector<string> _ErrMsg;

/*-----------------------------------------------
 *
 * Main
 *
-----------------------------------------------*/
int main(void)
{
    vector<CHECK_ORDER> Orders;
    vector<string> path;

    GetPath(path); /* Order のパスを用意する */

    for (size_t i = 0; i < path.size(); i++)
    {
        /* 初期化 */
        _Error = 0;
        _ErrMsg.clear();

        /* Order を読み込む */
        LoadOrder(Orders, path[i]);

        /* 空の Order は飛ばす */
        if (Orders.empty())
        {
            continue;
        }

        CheckSeq(Orders);   /* sequence の確認 */
        CheckMode(Orders);  /* mode の確認 */
        CheckParam(Orders); /* param の確認 */

        /* エラーメッセージ */
        if (_Error != 0)
        {
            cout << path[i] << endl;
            for (size_t i = 0; i < _ErrMsg.size(); i++)
            {
                cout << _ErrMsg[i] << endl;
            }
            cout << "---------------------------------------" << endl;
        }
    }
    return 0;
}

/*-----------------------------------------------
 *
 * Order のパスを準備する
 *
-----------------------------------------------*/
void GetPath(vector<string> &path)
{
    /* パスファイルを作成 */
    string order_dir;
#ifdef TEAM_A
    order_dir = string("../Main/Orders_A/");
    system("ls ../Main/Orders_A/pre > pre.txt");
    system("ls ../Main/Orders_A/final > final.txt");
    cout << "[TEAM_A]" << endl;
#endif
#ifdef TEAM_B
    order_dir = string("../Main/Orders_B/");
    system("ls ../Main/Orders_B/pre > pre.txt");
    system("ls ../Main/Orders_B/final > final.txt");
    cout << "[TEAM_B]" << endl;
#endif

    string order_path[2] = {
        string("pre"),
        string("final"),
    };

    for (int i = 0; i < 2; i++)
    {
        /* ファイルを開く */
        fstream file;
        file.open(order_path[i] + ".txt", ios::in);
        if (!file.is_open())
        {
            cout << "open error" << endl;
            _Flags.e(EXIT_FAILURE);
        }

        while (!file.eof())
        {
            /* パスを読み込む */
            string tmp;
            file >> tmp;

            /* 有効な文字列だったら path に追加 */
            if (tmp.find(".txt") != string::npos)
            {
                stringstream sstream;
                sstream << order_dir << order_path[i] << "/" << tmp;
                path.emplace_back(sstream.str());
            }
        }

        /* ファイルを閉じる */
        file.close();
    }
}

/*-----------------------------------------------
 *
 * Order を読み込む
 *
-----------------------------------------------*/
void LoadOrder(vector<CHECK_ORDER> &Orders, string path)
{
    /* 初期化 */
    Orders.clear();

    /* Order の情報を保管する */
    CHECK_ORDER Order;
    Order.clear();

    /* ファイルを開く */
    fstream file;
    file.open(path, ios::in);
    if (!file.is_open())
    {
        cout << "open error" << endl;
        _Flags.e(EXIT_FAILURE);
    }

    /* ファイルから Order を 読み込む */
    while (!file.eof())
    {
        string tmp_str;

        /* name */
        file >> Order.name;

        /* seq, mode */
        file >> Order.seq[0] >> Order.seq[1] >> Order.seq[2] >> Order.mode_str;

        /* param */
        getline(file, tmp_str);
        getline(file, tmp_str);
        Order.param = Split(tmp_str, ' ');

        /* Orders に追加 */
        if (Order.seq[0] != 0)
        {
            Orders.emplace_back(Order);
            Order.clear();
        }
    }

    /* ファイルを閉じる */
    file.close();
}

/*-----------------------------------------------
 *
 * seq が正常か確認する
 *
-----------------------------------------------*/
void CheckSeq(vector<CHECK_ORDER> &Orders)
{
    vector<vector<int>> seq;
    bool error = false;

    /*-----------------------------------------------
    seq を作成
    -----------------------------------------------*/
    for (size_t i = 0; i < Orders.size(); i++)
    {
        vector<int> tmp;
        tmp.emplace_back(Orders[i].seq[0]);
        tmp.emplace_back(Orders[i].seq[1]);
        tmp.emplace_back(Orders[i].seq[2]);
        seq.emplace_back(tmp);
    }

    /*-----------------------------------------------
    seq1
    -----------------------------------------------*/
    /* seq1 の最大値を求める */
    int max_seq1 = 0;
    for (size_t i = 0; i < seq.size(); i++)
    {
        max_seq1 = (max_seq1 < seq[i][0]) ? seq[i][0] : max_seq1;
    }

    /* 2 ~ max_seq1 まであるか確認 */
    int seq1 = 2;
    while (seq1 <= max_seq1)
    {
        auto itr = find(seq.begin(), seq.end(), vector<int>{seq1, 1, 1});
        if (itr == seq.end())
        {
            stringstream sstream;
            sstream << "seq\t(" << seq1 << ", 1, 1)";
            _ErrMsg.emplace_back(sstream.str());
            error = true;
        }
        seq1++;
    }

    /*-----------------------------------------------
    seq2
    -----------------------------------------------*/
    seq1 = 2;
    vector<int> max_seq2_v;
    while (seq1 <= max_seq1)
    {
        /* seq2 の最大値を求める */
        int max_seq2 = 0;
        for (size_t i = 0; i < seq.size(); i++)
        {
            if (seq[i][0] == seq1 && max_seq2 < seq[i][1])
            {
                max_seq2 = seq[i][1];
            }
        }
        max_seq2_v.emplace_back(max_seq2);

        /* 2 ~ max_seq2 まであるか確認 */
        int seq2 = 2;
        while (seq2 <= max_seq2)
        {
            auto itr = find(seq.begin(), seq.end(), vector<int>{seq1, seq2, 1});
            if (itr == seq.end())
            {
                stringstream sstream;
                sstream << "seq\t(" << seq1 << ", " << seq2 << ", 1)";
                _ErrMsg.emplace_back(sstream.str());
                error = true;
            }
            seq2++;
        }
        seq1++;
    }

    /*-----------------------------------------------
    seq3
    -----------------------------------------------*/
    seq1 = 2;

    while (seq1 <= max_seq1)
    {
        int seq2 = 1;
        while (seq2 <= max_seq2_v[seq1 - 2])
        {
            /* seq3 の最大値を求める */
            int max_seq3 = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i][0] == seq1 && seq[i][1] == seq2 && max_seq3 < seq[i][2])
                {
                    max_seq3 = seq[i][2];
                }
            }

            /* 2 ~ max_seq3 まであるか確認 */
            int seq3 = 2;
            while (seq3 <= max_seq3)
            {
                auto itr = find(seq.begin(), seq.end(), vector<int>{seq1, seq2, seq3});
                if (itr == seq.end())
                {
                    stringstream sstream;
                    sstream << "seq\t(" << seq1 << ", " << seq2 << ", " << seq3 << ")";
                    _ErrMsg.emplace_back(sstream.str());
                    error = true;
                }
                seq3++;
            }
            seq2++;
        }
        seq1++;
    }

    if (error)
    {
        _Error |= ERROR_SEQ;
    }
}

/*-----------------------------------------------
 *
 * mode が正常か確認する
 *
-----------------------------------------------*/
void CheckMode(vector<CHECK_ORDER> &Orders)
{
    /* strs を作成 */
    vector<string> strs;
    for (size_t i = 0; i < NUM_MODE; i++)
    {
        strs.emplace_back(string(_Modes[i][0]));
    }

    /* strs から探す */
    bool error = false;
    for (size_t i = 0; i < Orders.size(); i++)
    {
        auto itr = find(strs.begin(), strs.end(), Orders[i].mode_str);
        if (itr == strs.end())
        {
            char str[MAX_STRING_LENGTH];
            snprintf(str, sizeof(str),
                     "mode\t(%d, %d, %d)\t(%s)",
                     Orders[i].seq[0], Orders[i].seq[1], Orders[i].seq[2],
                     Orders[i].mode_str.c_str());
            _ErrMsg.emplace_back(string(str));
            error = true;
        }
        else
        {
            Orders[i].mode = itr - strs.begin();
        }
    }

    if (error)
    {
        _Error |= ERROR_MODE;
    }
}

/*-----------------------------------------------
 *
 * param が正常か確認する
 *
-----------------------------------------------*/
void CheckParam(vector<CHECK_ORDER> &Orders)
{
    /* CheckMode() でエラーが出ていたら終了 */
    if ((_Error & ERROR_MODE) == true)
    {
        return;
    }

    bool error = false;
    for (size_t i = 0; i < Orders.size(); i++)
    {
        if (Orders[i].param.size() != _Modes[Orders[i].mode][1][0])
        {
            char str[MAX_STRING_LENGTH];
            snprintf(str, sizeof(str),
                     "param\t(%d, %d, %d)",
                     Orders[i].seq[0],
                     Orders[i].seq[1],
                     Orders[i].seq[2]);
            _ErrMsg.emplace_back(str);
            error = true;
        }
    }

    if (error)
    {
        _Error |= ERROR_PARAM;
    }
}

/*-----------------------------------------------
 *
 * 文字列 str を区切り文字 sep で分割する
 *
-----------------------------------------------*/
std::vector<std::string> Split(const std::string &str, char sep)
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