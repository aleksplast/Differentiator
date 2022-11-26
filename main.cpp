#include <stdio.h>

#include "differentiator.h"

int main()
{
    char input[100] = "input.txt";

    gets(input);
    //DifferentiatorMain(input);

    Node* node = GetG(input, 1);

    NodePrint(node);

    char log[40] = "pictures\\log.htm";
    Tree tree = {};
    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    fprintf(stderr, "YOUR MUM\n");

    return 1;
}
