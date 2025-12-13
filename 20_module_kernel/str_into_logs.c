#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE ("GPL");

static int __init
str_into_logs_init (void)
{
  pr_info ("HI, I'm IN!!!\n");
  return 0;
}

// void, bc always success
static void __exit
str_into_logs_exit (void)
{
  pr_info ("BYE, I'm outta here\n");
}

module_init (str_into_logs_init);
module_exit (str_into_logs_exit);