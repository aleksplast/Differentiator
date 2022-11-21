#include <stdio.h>

#include "differentiator.h"

int main()
{
    char input[100] = "";
    gets(input);

//    DifferentiatorMain(input);

    printf("%d", GetG(input));

    return 0;
}
