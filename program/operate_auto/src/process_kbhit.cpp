#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"

void thread_kbhit(jibiki::ShareVal<bool> &exit_flag,
                  jibiki::ShareVal<bool> &start_flag,
                  jibiki::ShareVal<bool> &reset_flag,
                  jibiki::ShareVal<int> &pushed_key,
                  jibiki::ShareValVec<std::string> &executing_order)
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
            start_flag = start_flag.read() ^ 1;
            // std::cout << "start : " << start_flag.read() << std::endl;
            break;
        case 'r':
            reset_flag = reset_flag.read() ^ 1;
            // std::cout << "reset : " << reset_flag.read() << std::endl;
            break;
        case 'p':
            for (size_t i = 0; i < executing_order.size(); ++i)
                std::cout << "\t\t" << executing_order.read(i) << std::endl;
            break;
        case 'q':
            exit_flag = true;
            break;
        }

        if (pushed_key.read() != 0)
            pushed_key = 0;
    }
}