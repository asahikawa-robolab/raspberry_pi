#ifndef _CLASS_HPP
#define _CLASS_HPP

#include <unistd.h>
#include "_SharedMemory.hpp"

/*-----------------------------------------------
 *
 * COM_STATE
 *
-----------------------------------------------*/
class COM_STATE
{
public:
    int *cnt;
    COM_STATE(void);
    ~COM_STATE(void);
    void init(void);
};

inline COM_STATE::COM_STATE(void)
{
    cnt = (int *)shmOpen(SHM_COM_STATE,
                         sizeof(int) * NUM_COM_STATE,
                         SHM_NAME_COM_STATE);
    init();
    printf("COM_STATE was constructed.\n");
}

inline COM_STATE::~COM_STATE(void)
{
    shmClose(SHM_COM_STATE);
}

inline void COM_STATE::init(void)
{
    for (int i = 0; i < NUM_COM_STATE; i++)
    {
        cnt[i] = 0;
    }
}

/*-----------------------------------------------
 *
 * CONTROL_PERIOD
 *
-----------------------------------------------*/
class CONTROL_PERIOD
{
public:
    CONTROL_PERIOD(void);
    void adjust(void);

private:
    struct timespec time_s;
};

inline CONTROL_PERIOD::CONTROL_PERIOD(void)
{
    time_s = GetTime();
}

inline void CONTROL_PERIOD::adjust(void)
{
    const int time_us = 50;
    long int wait_time_us = time_us - CALC_SEC(time_s, GetTime()) * 1E6;

    /* time_us 経過していなかったら待機 */
    if (wait_time_us > 0)
    {
        usleep(wait_time_us);
    }

    /* 更新 */
    time_s = GetTime();
}

#endif