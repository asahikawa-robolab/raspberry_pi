#ifndef _FLAGS_HPP
#define _FLAGS_HPP

#include "_SharedMemory.hpp"
#include "_Defines.hpp"

#define START true
#define STOP false
#define KILL_NONE 0
#define KILL_OK 1
#define OPERATE_AUTO false
#define OPERATE_MANUAL true

class FLAGS
{
public:
    char *f;
    FLAGS(void);
    ~FLAGS(void);
    void init(void);
    void k(void);            /* kill */
    void e(int exit_status); /* exit */
    bool p(void);            /* process */
private:
    /* private 隠蔽 */
    FLAGS(const FLAGS &obj);                  /* コピーコンストラクタ */
    const FLAGS &operator=(const FLAGS &rhs); /* operator= のオーバーロード */
};

extern FLAGS _Flags;

#endif