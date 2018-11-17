#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// reference :
// https://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf

typedef struct rbtree_node {
    int32_t parent;
    int32_t left;
    int32_t right;
    int32_t color;
    int32_t value;
    // char    key[0];
    char    key[10];
} rbtree_node_t;

static char* ptr_ = 0;

#define rbt_red(node)               ((node)->color = 1)
#define rbt_black(node)             ((node)->color = 0)
#define rbt_is_red(node)            ((node)->color)
#define rbt_is_black(node)          (!rbt_is_red(node))
#define rbt_copy_color(n1, n2)      (n1->color = n2->color)

#define INDEX2NODE(idx) ((rbtree_node_t*)( (char*)(ptr_) + (idx)))
#define NODE2INDEX(p)   ((int32_t)((char*)(p) - (char*)(ptr_)))

#define g_sentinel -1
#define sentinel -1
int32_t root1 = -1;

void left_rotate(int32_t *root, rbtree_node_t *node)
{
    rbtree_node_t  *temp;

    temp = INDEX2NODE(node->right);
    node->right = temp->left;

    if ( temp->left != g_sentinel) {
        INDEX2NODE(temp->left)->parent = NODE2INDEX(node);
    }

    temp->parent = node->parent;

    if (node == INDEX2NODE(*root)) {
        *root = NODE2INDEX(temp);
    } else if ( NODE2INDEX(node) == INDEX2NODE(node->parent)->left ) {
       INDEX2NODE(node->parent)->left = NODE2INDEX(temp);
    } else {
        INDEX2NODE(node->parent)->right =NODE2INDEX(temp);
    }

    temp->left = NODE2INDEX(node);
    node->parent = NODE2INDEX(temp);
}


void right_rotate(int32_t *root, rbtree_node_t *node)
{
    rbtree_node_t  *temp;

    temp = INDEX2NODE(node->left);
    node->left = temp->right;

    if (temp->right != g_sentinel) {
        INDEX2NODE(temp->right)->parent = NODE2INDEX(node);
    }

    temp->parent = node->parent;

    if (node == INDEX2NODE(*root)) {
        *root = NODE2INDEX(temp);
    } else if ( NODE2INDEX(node) == INDEX2NODE(node->parent)->right) {
        INDEX2NODE(node->parent)->right = NODE2INDEX(temp);
    } else {
        INDEX2NODE(node->parent)->left = NODE2INDEX(temp);
    }

    temp->right = NODE2INDEX(node);
    node->parent = NODE2INDEX(temp);
}

void insert_str(rbtree_node_t *temp, rbtree_node_t *node)
{
    int32_t *p;

    for ( ;; ) {
        // p = strcmp(node->key, temp->key) ? temp->left : temp->right;
        p = strcmp(node->key, temp->key) <0 ? &temp->left : &temp->right;
        if ( *p == sentinel) {
            break;
        }
        temp = INDEX2NODE(*p);
    }

    *p = NODE2INDEX(node);
    node->parent = NODE2INDEX(temp);
    node->left   = sentinel;
    node->right  = sentinel;
    rbt_red(node);
    printf("p l r c k v = %d,%d,%d %d %s %d\n",
            node->parent, node->left, node->right, node->color,
            node->key, node->value);
}

void insert(rbtree_node_t *node)
{
    int32_t *root;
    rbtree_node_t *temp;

    root = &root1;
    /* a binary tree insert */

    // insert the first node as the root
    if ( *root == sentinel) {
        node->parent = sentinel;
        node->left = sentinel;
        node->right = sentinel;
        rbt_black(node);
        *root = NODE2INDEX(node);

        printf("p l r c k v = %d,%d,%d %d %s %d\n",
            node->parent, node->left, node->right, node->color,
            node->key, node->value);
        return;
    }

    insert_str(INDEX2NODE(*root), node);

    /* re-balance tree */
    while ( node != INDEX2NODE(*root) && rbt_is_red(INDEX2NODE(node->parent)) ) {
        printf("balance\n");

        if ( node->parent == INDEX2NODE(INDEX2NODE(node->parent)->parent)->left) {
            temp = INDEX2NODE(INDEX2NODE(INDEX2NODE(node->parent)->parent)->right);

            if (rbt_is_red(temp)) {
                rbt_black( INDEX2NODE(node->parent));
                rbt_black(temp);
                rbt_red( INDEX2NODE(INDEX2NODE(node->parent)->parent));
                node = INDEX2NODE(INDEX2NODE(node->parent)->parent);
            } else {
                if (node == INDEX2NODE(INDEX2NODE(node->parent)->right)) {
                    node = INDEX2NODE(node->parent);
                    left_rotate(root, node);
                }

                rbt_black( INDEX2NODE(node->parent));
                rbt_red(INDEX2NODE(INDEX2NODE(node->parent)->parent));
                right_rotate(root, INDEX2NODE(INDEX2NODE(node->parent)->parent));
            }
        } else {
            temp = INDEX2NODE(INDEX2NODE(INDEX2NODE(node->parent)->parent)->left);

            if (rbt_is_red(temp)) {
                rbt_black(INDEX2NODE(node->parent));
                rbt_black(temp);
                rbt_red(INDEX2NODE(INDEX2NODE(node->parent)->parent));
                node = INDEX2NODE(INDEX2NODE(node->parent)->parent);

            } else {
                if (node == INDEX2NODE(INDEX2NODE(node->parent)->left)) {
                    node = INDEX2NODE(node->parent);
                    right_rotate(root, node);
                }

                rbt_black(INDEX2NODE(node->parent));
                rbt_red(INDEX2NODE(INDEX2NODE(node->parent)->parent));
                left_rotate(root,  INDEX2NODE(INDEX2NODE(node->parent)->parent));
            }
        }
    }

    rbt_black(INDEX2NODE(*root));
}

void tr1(int32_t r)
{
    if ( r == sentinel )
        return;
    rbtree_node_t *node = INDEX2NODE(r);
    printf("k=%s v =%d n l p r c: %+4.1d %+4.1d %+4.1d %+4.1d %+4.1d\n",
            node->key, node->value, r,
            node->left, node->parent, node->right, node->color);

    tr1( INDEX2NODE(r)->left );
    tr1( INDEX2NODE(r)->right );
}

void debug() {
   int32_t idx;

   idx = root1;

   tr1(idx);
}

int main()
{
    rbtree_node_t *buf;
    buf = (rbtree_node_t*)malloc(1024*1024*100);

    ptr_ = (char*)buf;

    rbtree_node_t node;

    strcpy(node.key, "0");
    node.value = 0;
    memcpy(buf + 0, &node, sizeof(node));

    strcpy(node.key, "5");
    node.value = 1;
    memcpy(buf + 1, &node, sizeof(node));

    strcpy(node.key, "2");
    node.value = 2;
    memcpy(buf + 2, &node, sizeof(node));

    strcpy(node.key, "3");
    node.value = 3;
    memcpy(buf + 3, &node, sizeof(node));

    strcpy(node.key, "1");
    node.value = 3;
    memcpy(buf + 4, &node, sizeof(node));

    insert(buf + 0);
    insert(buf + 1);
    insert(buf + 2);
    insert(buf + 3);
    insert(buf + 4);

    debug();

}
