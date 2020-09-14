/*-----------------------------------------------
*
* デッドロックが発生するサンプル．
* デッドロックは複数のリソース（std::mutex） にアクセスするときに発生し得る．
* jibiki::ShareVar，jibiki::ShareVarVec ではリソースへのアクセスを１つずつ行うため
* デッドロックは発生しない（はず）．
* 
* lock_a を取得しているときはかならず unlock_a をしてから lock_b をすればデッドロックは生じない．
*
-----------------------------------------------*/
#include <thread>
#include <mutex>
#include <unistd.h>

std::mutex g_mtx1;
std::mutex g_mtx2;

void func1(void)
{
    g_mtx1.lock();
    usleep(1000);  /* 1 ms 待機 */
    g_mtx2.lock(); /* ここでデッドロックが発生 */

    /* unlock の順序は関係ない */
    g_mtx1.unlock();
    g_mtx2.unlock();
}

void func2(void)
{
    g_mtx2.lock();
    usleep(1000);  /* 1 ms 待機 */
    g_mtx1.lock(); /* ここでデッドロックが発生 */

    /* unlock の順序は関係ない */
    g_mtx1.unlock();
    g_mtx2.unlock();
}

int main(void)
{
    std::thread t1(func1);
    std::thread t2(func2);

    t1.join();
    t2.join();

    return 0;
}