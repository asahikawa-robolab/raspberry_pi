#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>   /* fcntl */
#include <termios.h> /* tcsetattr */
#include <unistd.h>  /* tcsetattr */
#include "../inc/_StdFunc.hpp"

void PrintMsg(const char *str, bool compact)
{
    /* ターミナルに出力 */
    if (!compact)
        printf("\n\n");
    for (unsigned int i = 0; i < strlen(str); i++)
        printf("*");
    printf("\n%s\n", str);
    for (unsigned int i = 0; i < strlen(str); i++)
        printf("*");
    printf("\n");
    if (!compact)
        printf("\n");
}

void PrintError(const char *place, const char *content)
{
    char tmp[MAX_STRING_LENGTH];
    snprintf(tmp, MAX_STRING_LENGTH, "error : %s [%s]", place, content);
    PrintMsg(tmp);
}

void PrintError(const char *place, const char *content, double num)
{
    char tmp[MAX_STRING_LENGTH];
    snprintf(tmp, MAX_STRING_LENGTH, "error : %s [%s(%lf)]", place, content, num);
    PrintMsg(tmp);
}

void PrintError(const char *place, const char *content, char *str)
{
    char tmp[MAX_STRING_LENGTH];
    snprintf(tmp, MAX_STRING_LENGTH, "error : %s [%s(%s)]", place, content, str);
    PrintMsg(tmp);
}

void PrintError(const char *place, const char *content, const char *str)
{
    char tmp[MAX_STRING_LENGTH];
    snprintf(tmp, MAX_STRING_LENGTH, "error : %s [%s(%s)]", place, content, (char *)str);
    PrintMsg(tmp);
}

FILE *Myfopen(const char *path, const char *mode)
{
    FILE *fp = fopen(path, mode);
    if (fp == NULL)
    {
        PrintError("Myfopen()", path);
        exit(EXIT_FAILURE);
    }
    return fp;
}

int kbhit(void)
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

struct timespec GetTime(void)
{
    struct timespec tmp;
    timespec_get(&tmp, TIME_UTC);
    return tmp;
}