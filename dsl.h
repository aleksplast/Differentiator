#define DL DiffNode(node->leftchild)
#define DR DiffNode(node->rightchild)
#define CL NodeCopy(node->leftchild)
#define CR NodeCopy(node->rightchild)
#define SUB(L, R) CreateNode(OP_TYPE, 0, OP_SUB, NULL, L, R)
#define MUL(L, R) CreateNode(OP_TYPE, 0, OP_MUL, NULL, L, R)
#define DIV(L, R) CreateNode(OP_TYPE, 0, OP_DIV, NULL, L, R)
#define ADD(L, R) CreateNode(OP_TYPE, 0, OP_ADD, NULL, L, R)
#define SIN(L, R) CreateNode(OP_TYPE, 0, OP_SIN, NULL, L, R)
#define COS(L, R) CreateNode(OP_TYPE, 0, OP_COS, NULL, L, R)
#define CREATENUM(num) CreateNode(NUM_TYPE, num, OP_UNKNOWN, NULL, NULL, NULL)
