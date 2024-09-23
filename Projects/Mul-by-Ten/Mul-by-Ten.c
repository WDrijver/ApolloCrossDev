#include "Mul_by_Ten.h"

int main()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        printf("Line %d\n", i);
        printf("10 * %d = %d = %d\n", i, 10*i, mul_by_ten(i));
        printf("--------\n");
    }

    return 0;
}
