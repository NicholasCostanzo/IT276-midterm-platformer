#include "levelTree.h"
#include <stddef.h>
#include <stdio.h>

#define MaxDepth 3
#define MaxChildren 3
#define NumberOfLevels 5

struct node *levelMap = NULL;

void destroyTree(struct node *leaf)
{
  if(leaf != NULL)
  {
    destroyTree(leaf->sibling);
    destroyTree(leaf->child);
    free(leaf);
  }
}

void preorder(struct node *leaf)
{
  if(leaf == NULL)return;
  fprintf(stdout, "%d ", leaf->level);
  preorder(leaf->child);
  preorder(leaf->sibling);
}

struct node* search(struct node* leaf, int level)
{
  struct node* temp;
  
  if(leaf==NULL)
    return;
  if(level==leaf->level)
    return leaf;
  temp = search(leaf->child,level);
  if(temp==NULL)
    temp = search(leaf->sibling, level);
  return temp;
}

struct node* newNode(int level)
{
  struct node* newnode = (struct node*)malloc(sizeof(struct node));
  newnode->child = NULL;
  newnode->sibling = NULL;
  newnode->level = level;
  return newnode;
}

struct node* createTree(struct node* leaf, int levels[])
{
  int i;
  struct node* newnode = search(leaf, levels[0]);
  struct node* parent;
  
  if(newnode==NULL)
    newnode = newNode(levels[0]);
  
  parent = newnode;
  for(i = 0; i < levels[1]; i++)
  {
    if(i==0)
    {
      parent->child = newNode(levels[i+2]);
      parent = parent->child;
    }
    else
    {
      parent->sibling = newNode(levels[i+2]);
      parent = parent->sibling;
    }
  }
  
  if(leaf == NULL)
    leaf = newnode;
  return leaf;
}

/*the following two functions are for a fisher-yates shuffle*/
static int rand_int(int n) {
  /*int limit = RAND_MAX - RAND_MAX % n;
  int rnd;
  do {
    rnd = rand();
  } while (rnd >= limit);
  return rnd % n;*/
  int rnd = rand();
  return rnd % n;
}
void shuffle(int *array, int n) {
  int i, j, tmp;
  srand(time(NULL));
  for (i = n - 1; i > 0; i--) {
    j = rand_int(i + 1);
    tmp = array[j];
    array[j] = array[i];
    array[i] = tmp;
  }
}

int getLevelChildren(int level) /*this is all hard-coded for now, TODO: have plain text info about goal number in level files, then search through them all here to find those values*/
{
  if(level==1)
    return 1;
  if(level==2)
    return 3;
  if(level==3)
    return 3;
  if(level==4)
    return 3;
  if(level==5)
    return 3;
  return -1;
}

void buildLevelTree()
{
  /*1st element in each row is node Value, 2nd - no of child, ,>=3rd....=>value of child*/
  int levels[NumberOfLevels];
  int data[MaxDepth][MaxChildren + 2]={{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
  int i, j, temp, l;
  
  for(i = 1; i <= NumberOfLevels; i++) /*assuming levels will be numbered from 1 to N*/
  {
    levels[i-1] = i;
  }
  shuffle(&levels, NumberOfLevels);
  
  for(i = 0; i < NumberOfLevels; i++) /*force level 1 to be in the beginning*/
  {
    if(levels[i]==1)
    {
      levels[i] = levels[0];
      levels[0] = 1;
    }
  }
  
  for(i = 0,l=0; i < MaxDepth; i++) /*construct tree based on random input*/
  {
    if(i==0)
    {
      data[i][0]=levels[l];
      data[i][1]=getLevelChildren(levels[l]);
      l++;
      data[i][2]=levels[l];
      data[i+1][0]=levels[l];
      l++;
    }
    else
    {
      temp = getLevelChildren(data[i][0]);
      data[i][1] = temp;
      for(j = 2; j <= temp+1; j++)
      {
	data[i][j] = levels[l];
	if(i+j-1 < MaxDepth) 
	  data[i+j-1][0] = levels[l];
	l++;
	if(l >= NumberOfLevels) break;
      }
    }
    if(l >= NumberOfLevels) break;
  }
  
  /*for(i = 0; i < NumberOfLevels; i++)
  {
    fprintf(stdout, "%d ", levels[i]);
  }
  fprintf(stdout, "\n");
  for(i = 0; i < MaxDepth; i++)
  {
    for(j = 0; j < MaxChildren+2; j++)
    {
      fprintf(stdout, "%d ", data[i][j]);
    }
    fprintf(stdout, "\n");
  }
  fprintf(stdout, "\n");*/
  for(i = 0; i < MaxDepth; i++)
  {
    levelMap = createTree(levelMap, data[i]);
  }
}
