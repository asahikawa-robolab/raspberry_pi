#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"

void thread_kbhit(jibiki::ShareVar<bool> &exit_flag,
                  jibiki::ShareVar<bool> &start_flag,
                  jibiki::ShareVar<bool> &reset_flag,
                  jibiki::ShareVar<int> &pushed_key,
                  jibiki::ShareVarVec<std::string> &executing_order)
{
    if (!jibiki::thread::enable("kbhit"))
        return;

    while (jibiki::thread::manage(exit_flag))
    {
        if (jibiki::kbhit())
            pushed_key = getchar();

        switch (pushed_key.read())
        {
        case 's':
            start_flag ^= 1;
            break;
        case 'r':
            reset_flag ^= 1;
            break;
        case 'q':
            exit_flag = true;
            break;
        }

        if (pushed_key.read() != -1)
            pushed_key = -1;
    }
}