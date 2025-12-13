#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>

MODULE_LICENSE ("GPL");

static char str[15] = "HEY!";
static rwlock_t lock;

static ssize_t
sysfs_show (struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{

  ssize_t cnt;
  read_lock (&lock);
  cnt = scnprintf (buff, PAGE_SIZE, "%s", str);
  read_unlock (&lock);

  return cnt;
}

static ssize_t
sysfs_store (struct kobject *kobj, struct kobj_attribute *attr,
             const char *buff, size_t size)
{
  int rc = 0;

  if (size > 15)
    return -EINVAL;

  write_lock (&lock);
  rc = scnprintf (str, sizeof (str), "%s", buff);

  write_unlock (&lock);
  return size;
}

static struct kobject *string_kobj;
static struct kobj_attribute string_attribute
    = __ATTR (string, 0664, sysfs_show, sysfs_store);

static int __init
startup (void)
{
  int ret;

  pr_info ("string module!\n");
  rwlock_init (&lock);

  string_kobj = kobject_create_and_add ("string_module", kernel_kobj);
  if (!string_kobj)
    {
      pr_err ("failed to create kobject\n");
      return -ENOMEM;
    }

  ret = sysfs_create_file (string_kobj, &string_attribute.attr);
  if (ret)
    {
      pr_err ("failed to create sysfs file\n");
      kobject_put (string_kobj);
      return ret;
    }

  pr_info ("sysfs file created: /sys/kernel/string_module/string\n");
  return 0;
}

static void __exit
cleanup (void)
{
  sysfs_remove_file (string_kobj, &string_attribute.attr);
  kobject_put (string_kobj);

  pr_info ("i'm outta here!\n");
}

module_init (startup);
module_exit (cleanup);