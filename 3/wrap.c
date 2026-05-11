#include <stdio.h>

void __real_fred(void);
void __real_john(void);

void __wrap_fred(void) {
    printf("fred is called\n");
    __real_fred();
}

void __wrap_john(void) {
    printf("john is called\n");
    __real_john();
}
