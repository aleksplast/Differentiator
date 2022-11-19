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
#define SQRT(L, R) CreateNode(OP_TYPE, 0, OP_SQRT, NULL, L, R)
#define POWER(L, R) CreateNode(OP_TYPE, 0, OP_POWER, NULL, L, R)
#define LN(L,R) CreateNode(OP_TYPE, 0, OP_LN, NULL, L, R)
#define CH(L,R) CreateNode(OP_TYPE, 0, OP_CH, NULL, L, R)
#define SH(L, R) CreateNode(OP_TYPE, 0, OP_SH, NULL, L, R)
#define IS_OP(oper) (node->type == OP_TYPE) && (node->optype == oper)
#define IS_LVAL(value) (node->leftchild->type == NUM_TYPE) && (compare(node->leftchild->val, value) == 0)
#define IS_RVAL(value) (node->rightchild->type == NUM_TYPE) && (compare(node->rightchild->val, value) == 0)
#define CREATENUM(num) CreateNode(NUM_TYPE, num, OP_UNKNOWN, NULL, NULL, NULL)
#define CREATEVAR(var) CreateNode(VAR_TYPE, 0, OP_UNKNOWN, #var, NULL, NULL)
