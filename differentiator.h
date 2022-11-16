#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "diftree.h"
#include "text-sort.h"
#include "common.h"
#include "dsl.h"

int DifferentiatorMain(char* input);

Tree DataParser(char* inputdata);

int FillCurrNode(Node* currnode, char* input);

Node* NodeCopy(Node* node);

Node* DiffNode(Node* node);

int TeXPrint(Tree* tree, char* output);

int NodePrint(Node* node);

int DataPrint(Node* node, FILE* out);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Node* leftchild, Node* rightchild);

OperType IsOper(char* str);

#endif //DIFFERENTIATOR_H
