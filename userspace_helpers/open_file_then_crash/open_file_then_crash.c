#include <stdio.h>

int main(int argc, char *arv[]) {
    FILE *f;
    f = fopen("test.txt", "r");

    // crasha
    FILE *f2;
    f2 = fopen("/sys/kernel/crash_sysfs/null_ptr_deref_crash", "r");
    char buffer[256];
    fread(buffer, 256, 256, f2);
    // f2 = fopen("/sys/kernel/crash_sysfs/null_ptr_deref_crash", "r");
    // f2 = fopen("/sys/kernel/crash_sysfs/null_ptr_deref_crash", "r");

    return 0;
}