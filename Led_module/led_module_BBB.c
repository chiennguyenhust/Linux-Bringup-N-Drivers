#include <linux/module.h>
#include <linux/printk.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/uaccess.h>

#define GPIO_0_BASE_ADDRESS 0x44E07000
#define GPIO_0_SIZE 0x1000
#define GPIO_DATA_OUT 0x13c
#define GPIO_OE       0x134

static ssize_t led_module_write(struct file *file, const char __user *buf, size_t len, loff_t *pos);
static ssize_t led_module_read(struct file *file, char __user *buf, size_t len, loff_t *pos);
static int led_module_open(struct inode *inode, struct file *file);
static int led_module_close(struct inode *inodep, struct file *filp);

void led_on(void);
void led_off(void);

static const struct file_operations led_module_fops = 
{
    .owner			= THIS_MODULE,
    .write			= led_module_write,
	.read			= led_module_read,
    .open			= led_module_open,
    .release		= led_module_close
};

struct miscdevice led_module = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_module_test",      // tên device file được tạo ra sau khi load kernel module lên
    .fops = &led_module_fops,
};

void __iomem * GPIO_0_Virtual_Ptr = NULL;


static ssize_t led_module_write(struct file *file, const char __user *buf, size_t len, loff_t *pos)
{
    char received_data = 0;
    int ret = 0;

    pr_info("%d %s\n", __LINE__, __func__);

    ret = copy_from_user(&received_data, &buf[0], 1);

    if(ret != 0)
    {
        pr_info("Copy data failed\n");
        return -1;
    }

    switch(received_data)
    {
        case '0':
            led_off();
            break;
        case '1':
            led_on();
            break;
        default:
            pr_info("Invalid data\n");
            led_off();
    }

    return 1;
}

static ssize_t led_module_read(struct file *file, char __user *buf, size_t len, loff_t *pos)
{
    int ret = 0;
    u32 data = 0;

    pr_info("%d %s\n", __LINE__, __func__);

    data = readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);
    data = (data >> 31) & 0xff;

    ret = copy_to_user(&buf[0], &data, 1);
    if(ret != 0)
    {
        pr_info("Copy to user failed\n");
        return -1;
    }

    return 1;
}

static int led_module_open(struct inode *inode, struct file *file)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

static int led_module_close(struct inode *inodep, struct file *filp)
{
    pr_info("%d %s\n", __LINE__, __func__);

    return 0;
}

void led_on(void)
{
    u32 data = 0;

    data |= (1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));
}

void led_off(void)
{
    u32 data = 0;

    data = readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);
    data &= ~(1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));
}

/* Module Init */
static int __init led_module_init(void)
{
    int ret = 0;
    u32 data = 0;

    pr_info("led module BBB init\n");

    ret = misc_register(&led_module);
    if(ret != 0)
    {
        pr_info("misc failed\n");
        return -1;
    }

    GPIO_0_Virtual_Ptr = ioremap(GPIO_0_BASE_ADDRESS, GPIO_0_SIZE);
    if(GPIO_0_Virtual_Ptr == NULL)
    {
        pr_info("ioremap failed\n");
        return -1;
    }
    pr_info("GPIO_0_Virtual_Ptr = %d\n", GPIO_0_Virtual_Ptr);

    /* Configure GPIO as output */
    data = readl(GPIO_0_Virtual_Ptr + GPIO_OE);
    data &= ~(1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_OE));

    /* Turn on LED */
    data = readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);
    data |= (1 << 31);
    writel(data, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));
    data = readl(GPIO_0_Virtual_Ptr + GPIO_DATA_OUT);

    return 0;
}

/* Module Exit */
static void __exit led_module_exit(void)
{
    misc_deregister(&led_module);


    pr_info("GPIO_0_Virtual_Ptr = %d\n", GPIO_0_Virtual_Ptr);

    /* Turn off LEDs */
    writel(0, (GPIO_0_Virtual_Ptr + GPIO_DATA_OUT));

    pr_info("led module BBB exit\n");
}

module_init(led_module_init);
module_exit(led_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chien nguyen - nguyendinhchien.work@gmail.com");
MODULE_DESCRIPTION("led kernel module");