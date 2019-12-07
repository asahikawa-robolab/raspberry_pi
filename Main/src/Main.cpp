#include <stdlib.h>
#include <unistd.h>   /* fork */
#include <sys/wait.h> /* wait */
#include "../../Share/inc/_SharedMemory.hpp"
#include "../../Share/inc/_StdFunc.hpp"
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../inc/Main.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"

void SelectProcess(int fork_count);

int main(void)
{
    /*-----------------------------------------------
    子プロセス
    -----------------------------------------------*/
    int fork_count;
    for (fork_count = 0; fork_count < PROCESS_NUM; fork_count++)
    {
        /* 子プロセスを作成 */
        pid_t pid = fork();
        /* エラーチェック */
        if (pid == -1)
        {
            PrintError("main()", "fork failed.");
            return 1;
        }
        /* 各プロセスを実行 */
        else if (pid == 0)
        {
            SelectProcess(fork_count);
            return 0;
        }
    }
    /*-----------------------------------------------
    親プロセス
    -----------------------------------------------*/
    /* 子プロセスの終了を待つ */
    for (int i = 0; i < fork_count; i++)
    {
        wait(NULL);
    }
    printf("\n*** Main end. ***\n");

    /* キー入力があるか，リセットボタンが押されるまで待機 */
    while (kbhit() == false)
    {
        if(_Flags.f[FLAG_MAIN_END] == true)
        {
            _Flags.f[FLAG_MAIN_END] = false;
            break;
        }
    }
    return 0;
}

/*-----------------------------------------------
プロセスを選択する
-----------------------------------------------*/
void SelectProcess(int fork_count)
{
    switch (fork_count)
    {
    case PROCESS_MASTER:
        ProcessMaster();
        break;
    case PROCESS_COM:
        ProcessCom();
        break;
    case PROCESS_DISPLAY:
        ProcessDisplay();
        break;
    }
}