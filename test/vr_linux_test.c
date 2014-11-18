#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sylvain Afchain");
MODULE_DESCRIPTION("Vrouter unit test kernel module");

# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif

#define TEST_F(X) void X(int *ret); const char *wrapper_##X(int *ret) {\
    const char *test_name = get_test_name(__FUNCTION__); \
    X(ret);\
    return test_name; \
} \
void X(int *ret)

#define TEST(X) wrapper_##X
typedef const char *(*TEST_SUITE)(int *);
typedef const char *(*TEST)(int *);

#define ASSERT_EQUALS(X,Y) if (X != Y) { *ret = 0; return; }

const char *get_test_name(const char *name) {
    return name + 8;
}

static void print_tests_number(TEST *test_suite) {
    TEST *test = test_suite;
    int total = 1;

    while(*test != NULL) {
        total++;
        test++;
    }

    printk(KERN_INFO "1..%d\n", total);
}

static int RUN_TESTS(TEST *test_suite) {
    TEST *test = test_suite;
    int ret, result = 1, total = 1;
    const char *test_name;

    print_tests_number(test_suite);

    while(*test != NULL) {
        ret = 1;
        test_name = (*test)(&ret);
        if (ret) {
            printk(KERN_INFO "ok %d %s\n", total, test_name);
        } else {
            printk(KERN_INFO "not ok %d %s\n", total, test_name);
            result = 0;
        }

        total++;
        test++;
    }

    return result;
}


/* below unit tests */
TEST_F(test_ko) {
    ASSERT_EQUALS(0,1);
}

TEST_F(test_ok) {
    ASSERT_EQUALS(2, 2);
}


static int __init vr_linux_test_init(void) {
    TEST_SUITE test_suite[] = {
        TEST(test_ok),
        TEST(test_ko),
        NULL
    };

    printk(KERN_INFO "Vrouter Unit Tests loaded!\n");

    RUN_TESTS(test_suite);

    return 0;
}

static void __exit vr_linux_test_cleanup(void) {
    printk(KERN_INFO "Vrouter Unit Tests unloaded!\n");
}

module_init(vr_linux_test_init);
module_exit(vr_linux_test_cleanup);
