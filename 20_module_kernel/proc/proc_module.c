#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE ("GPL");

static char str[15] = "HEY!";
static rwlock_t lock;

static ssize_t
proc_read (struct file *fd, char __user *buff, size_t size, loff_t *off)
{
  size_t len;
  size_t rc;

  read_lock (&lock);
  len = strlen (str);
  rc = simple_read_from_buffer (buff, size, off, str, len);
  read_unlock (&lock);
  return rc;
}

static ssize_t
proc_write (struct file *fd, const char __user *buff, size_t size, loff_t *off)
{
  ssize_t rc;

  if (size > 14) // \0
    return -EINVAL;

  write_lock (&lock);
  rc = simple_write_to_buffer (str, 14, off, buff, size);
  if (rc > 0)
    str[rc] = '\0';
  write_unlock (&lock);

  return rc;
}

static struct proc_ops proc_fops
    = { .proc_read = proc_read, .proc_write = proc_write };

static int __init
startup (void)
{
  pr_info ("string module loading\n");
  rwlock_init (&lock);

  proc_create ("string", 0666, NULL, &proc_fops);

  pr_info ("proc file created: /proc/string\n");

  return 0;
}

static void __exit
cleanup (void)
{
  remove_proc_entry ("string", NULL);
  pr_info ("removed entry, Im outta here!\n");
}

module_init (startup);
module_exit (cleanup);