#include <stdlib.h>
#include <unistd.h>
#include "../inc/_Flags.hpp"
#include "../inc/_StdFunc.hpp"

FLAGS _Flags;

FLAGS::FLAGS(void)
{
    f = (char *)shmOpen(SHM_FLAGS, sizeof(char) * NUM_FLAGS, SHM_NAME_FLAGS);
    init();
    printf("FLAGS was constructed.\n");
}

FLAGS::~FLAGS(void)
{
    shmClose(SHM_FLAGS);
}

void FLAGS::init(void)
{
    /* 初期化 */
    for (int i = 0; i < NUM_FLAGS; i++)
    {
        f[i] = false;
    }
    f[FLAG_EMERGENCY] = START;
}

void FLAGS::k(void)
{
    f[FLAG_KILL] = KILL_OK;
}

void FLAGS::e(int exit_status)
{
    f[FLAG_KILL] = KILL_OK;
    exit(exit_status);
}

bool FLAGS::p(void)
{
    /* 終了処理 */
    if (f[FLAG_KILL] == KILL_OK)
    {
        return false;
    }
    return true;
}