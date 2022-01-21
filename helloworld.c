#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/init_task.h>


// you need to pass this data struct to userspace and print on terminal
struct data_segment
{
    unsigned long start_code;
    unsigned long end_code;
};

asmlinkage int sys_helloworld(pid_t user_pid, void* __user user_address)
{
    printk("Welcome to my call kernel\n");
    struct data_segment my_data_segment;
    struct task_struct *task;
    for_each_process(task)
    {
        if(task->pid == user_pid)
        {
            printk("Find the process!\n");
            //what is x1, x2, x3 
            my_data_segment.start_code = task->mm->start_code;
            //what is y1, y2, y3 
            my_data_segment.end_code = task->mm->end_code;
            // xxx is key function
            copy_to_user(user_address, &my_data_segment, sizeof(struct data_segment));
        }
    }
    printk("System call is finished!\n");

    return 0;
}
