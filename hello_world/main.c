#include <linux/module.h>

static int __init hello_world_init(void) 
{
   pr_info("hello, world!\n"); 
   return 0;
}


static void __exit hello_world_cleanup(void)
{
   pr_info("Good bye, world!\n"); 
}

module_init(hello_world_init);
module_exit(hello_world_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harish");
MODULE_DESCRIPTION("A simple hello world kernel module");
