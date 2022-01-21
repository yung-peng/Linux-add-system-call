#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

struct data_segment
{
    unsigned long start_code;
    unsigned long end_code;
};

int main()
{
    struct data_segment my_data_segment;
    printf("Start: %lx\nEnd: %lx\n", my_data_segment.start_code, my_data_segment.end_code);

    int a = syscall(333, getpid(), (void*)&my_data_segment);

    printf("Start: %lx\nEnd: %lx\n", my_data_segment.start_code, my_data_segment.end_code);
    printf("return: %d\n", a);
    return 0;
}
