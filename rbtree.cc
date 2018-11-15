#include <stdio.h>

// reference :
// https://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf

typedef struct rbtree_node {
    int32_t parent;
    int32_t left;
    int32_t right;
    bool    color;
    int32_t value;
    char    key[0];
} rbtree_node_t;

static char* ptr_ = (char*)0x000001000;

#define INDEX2NODE(idx) ((rbtree_node_t*)( (char*)(ptr_) + (idx)))
#define NODE2INDEX(p)   ((int32_t)((char*)(p) - (char*)(ptr_)))
#define sentinel -1

void rote_left(rbtree_node_t **root, rbtree_node_t *node)
{
    rbtree_node_t  *temp;

    temp = INDEX2NODE(node->right);
    node->right = temp->left;

    if ( temp->left != sentinel) {
        INDEX2NODE(temp->left)->parent = NODE2INDEX(node);
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;
    } else if (node == INDEX2NODE(INDEX2NODE(node->parent)->left) ) {
       INDEX2NODE(node->parent)->left = NODE2INDEX(temp);
    } else {
        INDEX2NODE(node->parent)->right =NODE2INDEX(temp);
    }

    temp->left = NODE2INDEX(node);
    node->parent = NODE2INDEX(temp);
}

int main()
{

}
