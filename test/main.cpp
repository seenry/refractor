#include "test.h"

uint8_t (*test_list[])(char*) = {
    test_surf_init,
    test_surf_0,
    test_surf_1,
    test_tesselation_0,
};

int main () {
    char err_str[256];

    int n_test = sizeof(test_list) / sizeof(test_list[0]);
    int n_ok = 0;
    for (int i = 0; i < n_test; i++) {
        printf("Test %d/%d: ", i+1, n_test);
        if (test_list[i](err_str) == 0) {
            printf("PASSED.\n");
            n_ok++;
        } else {
            printf("\e[1m\033[31mFAILED\033[0m\e[m.\n");
            fprintf(stderr, "  %s", err_str);
        }
    }

    printf("\nSummary: \e[1m\033[32m%d/%d\033[0m\e[m tests passed.\n\n", n_ok, n_test);

    return 0;
}
