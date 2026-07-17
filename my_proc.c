#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAX_LEN 4096
static struct proc_dir_entry *proc_entry;
static char *info;

ssize_t read_proc(struct file *f, char *user_buf, size_t count, loff_t *off)
{
        int len;
        if (*off > 0)
                return 0;

        len = strlen(info);
        if (count < len)
                len = count;

        if (copy_to_user(user_buf, info, len))
                return -EFAULT;

        *off += len;
        return len;
}

ssize_t write_proc(struct file *f, const char *user_buf, size_t count, loff_t *off)
{
        if (count > MAX_LEN - 1)
                count = MAX_LEN - 1;

        if (copy_from_user(info, user_buf, count))
                return -EFAULT;

        info[count] = '\0';
        return count;
}

struct file_operations proc_fops = {
        .read = read_proc,
        .write = write_proc,
};

int init_module(void)
{
        int ret = 0;

        info = kmalloc(MAX_LEN, GFP_KERNEL);
        if (!info) {
                printk(KERN_INFO "Failed to allocate memory.\n");
                return -ENOMEM;
        }
        memset(info, 0, MAX_LEN);

        proc_entry = proc_create("myproc", 0666, NULL, &proc_fops);
        if (!proc_entry) {
                printk(KERN_INFO "Failed to create /proc/myproc.\n");
                kfree(info);
                return -ENOMEM;
        }

        printk(KERN_INFO "test_proc created.\n");
        return ret;
}

void cleanup_module(void)
{
        remove_proc_entry("myproc", NULL);
        kfree(info);
        printk(KERN_INFO "test_proc removed.\n");
}

MODULE_LICENSE("GPL");
