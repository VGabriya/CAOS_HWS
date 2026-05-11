#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

void bill(void) {
    printf("bill is called\n");
    void (*real_bill)(void) = dlsym(RTLD_NEXT, "bill");
    real_bill();
}

void sam(void) {
    printf("sam is called\n");
    void (*real_sam)(void) = dlsym(RTLD_NEXT, "sam");
    real_sam();
}
