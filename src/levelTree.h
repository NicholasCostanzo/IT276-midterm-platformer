#ifndef __LEVELTREE__
#define __LEVELTREE__

struct node
{
  int level;
  int maxChildNum;
  int depth;
  struct node *child;
  struct node *sibling;
  struct node *parent;
};

void destroyTree(struct node *leaf);

void buildLevelTree();

void preorder(struct node* leaf);

struct node* search(struct node* leaf, int level);

struct node* newNode(int level);

struct node* createTree(struct node* leaf, int data[]);

int rand_int(int n);

void shuffle(int *array, int n);

int getLevelChildren(int level);

#endif
