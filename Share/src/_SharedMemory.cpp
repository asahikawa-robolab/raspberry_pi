#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../inc/_SharedMemory.hpp"
#include "../inc/_StdFunc.hpp"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
int ShmFd[NOShm]; /* 共有メモリで使用するファイルディスクリプタ */

/*-----------------------------------------------
開く
-----------------------------------------------*/
void *shmOpen(SHM_KIND shm_kind, int shm_size, const char *mem_name)
{
    /* shm_open */
    if (BETWEEN(0, shm_kind, NOShm) == false)
    {
        PrintError("shmOpen()", "");
        exit(EXIT_FAILURE);
    }
    ShmFd[shm_kind] = shm_open(mem_name, O_RDWR | O_CREAT, FILE_MODE);

    /* shm_map */
    struct stat status;
    ftruncate(ShmFd[shm_kind], shm_size);
    fstat(ShmFd[shm_kind], &status);
    /* メッセージ */
    // printf("%s was mapped.\n", mem_name);

    return ((void *)mmap(NULL,
                         status.st_size,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         ShmFd[shm_kind],
                         0));
}

/*-----------------------------------------------
閉じる
-----------------------------------------------*/
void shmClose(void)
{
    for (int i = 0; i < NOShm; i++)
        close(ShmFd[i]);
    printf("shm was closed.\n");
}

void shmClose(SHM_KIND shm_kind)
{
    if (BETWEEN(0, shm_kind, NOShm) == true)
    {
        close(ShmFd[shm_kind]);
    }
}