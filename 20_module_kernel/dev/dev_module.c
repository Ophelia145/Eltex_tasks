#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE ("GPL");

static int major = 0;
static char str[15] = "HEY!";
static rwlock_t lock;

static ssize_t
device_read (struct file *fd, char __user *buff, size_t size, loff_t *off)
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
device_write (struct file *fd, const char __user *buff, size_t size,
              loff_t *off)
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

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
};

static int __init
startup (void)
{
  pr_info ("chrdev module loading\n");
  rwlock_init (&lock);

  major = register_chrdev (major, "STRING", &fops);
  if (major < 0)
    {
      return major;
    }

  pr_info ("dev reg, major number: %d\n", major);

  return 0;
}

static void __exit
cleanup (void)
{
  unregister_chrdev (major, "STRING");
  pr_info ("dev unreg. Im outta here!\n");
}

module_init (startup);
module_exit (cleanup);