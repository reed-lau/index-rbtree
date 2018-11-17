#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// reference :
// https://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf

typedef struct rbtree_node {
    int32_t parent;
    int32_t left;
    int32_t right;
    uint8_t color;
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

static rbtree_node_t *rbtree_min(rbtree_node_t *node)
{
    while (node->left != sentinel) {
        node = INDEX2NODE(node->left);
    }

    return node;
}

void rbtree_delete(rbtree_node_t *node)
{
    uint8_t red;
    int32_t *root;
    rbtree_node_t *subst, *temp, *w;

    /* a binary tree delete */

    root = &root1;

    if (node->left == sentinel) {
        temp = INDEX2NODE(node->right);
        subst = node;
    } else if (node->right == sentinel) {
        temp = INDEX2NODE(node->left);
        subst = node;
    } else {
        subst = rbtree_min(INDEX2NODE(node->right));

        if (subst->left != sentinel) {
            temp = INDEX2NODE(subst->left);
        } else {
            temp = INDEX2NODE(subst->right);
        }
    }

    if (subst == INDEX2NODE(*root)) {
        *root = NODE2INDEX(temp);
        rbt_black(temp);

        /* DEBUG stuff */
        node->left = -1;
        node->right = -1;
        node->parent = -1;
        // node->key = 0;

        return;
    }

    red = rbt_is_red(subst);

    if (subst == INDEX2NODE(INDEX2NODE(subst->parent)->left)) {
        INDEX2NODE(subst->parent)->left = NODE2INDEX(temp);
    } else {
        INDEX2NODE(subst->parent)->right = NODE2INDEX(temp);
    }

    if (subst == node) {
        temp->parent = subst->parent;
    } else {
        if (subst->parent == NODE2INDEX(node)) {
            temp->parent = NODE2INDEX(subst);
        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        rbt_copy_color(subst, node);

        if (node == INDEX2NODE(*root)) {
            *root = NODE2INDEX(subst);
        } else {
            if (node == INDEX2NODE(INDEX2NODE(node->parent)->left)) {
                INDEX2NODE(node->parent)->left = NODE2INDEX(subst);
            } else {
                INDEX2NODE(node->parent)->right = NODE2INDEX(subst);
            }
        }

        if (subst->left != sentinel) {
            INDEX2NODE(subst->left)->parent = NODE2INDEX(subst);
        }

        if (subst->right != sentinel) {
            INDEX2NODE(subst->right)->parent = NODE2INDEX(subst);
        }
    }

    /* DEBUG stuff */
    node->left = -1;
    node->right = -1;
    node->parent = -1;
    // node->key = 0;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != INDEX2NODE(*root) && rbt_is_black(temp)) {

        if (temp == INDEX2NODE(INDEX2NODE(temp->parent)->left)) {
            w = INDEX2NODE(INDEX2NODE(temp->parent)->right);

            if (rbt_is_red(w)) {
                rbt_black(w);
                rbt_red(INDEX2NODE(temp->parent));
                left_rotate(root, INDEX2NODE(temp->parent));
                w = INDEX2NODE(INDEX2NODE(temp->parent)->right);
            }

            if (rbt_is_black(INDEX2NODE(w->left)) && rbt_is_black(INDEX2NODE(w->right))) {
                rbt_red(w);
                temp = INDEX2NODE(temp->parent);
            } else {
                if (rbt_is_black(INDEX2NODE(w->right))) {
                    rbt_black(INDEX2NODE(w->left));
                    rbt_red(w);
                    right_rotate(root, w);
                    w = INDEX2NODE(INDEX2NODE(temp->parent)->right);
                }

                rbt_copy_color(w, INDEX2NODE(temp->parent));
                rbt_black(INDEX2NODE(temp->parent));
                rbt_black(INDEX2NODE(w->right));
                left_rotate(root, INDEX2NODE(temp->parent));
                temp = INDEX2NODE(*root);
            }

        } else {
            w = INDEX2NODE(INDEX2NODE(temp->parent)->left);

            if (rbt_is_red(w)) {
                rbt_black(w);
                rbt_red(INDEX2NODE(temp->parent));
                right_rotate(root, INDEX2NODE(temp->parent));
                w = INDEX2NODE(INDEX2NODE(temp->parent)->left);
            }

            if (rbt_is_black(INDEX2NODE(w->left)) && rbt_is_black(INDEX2NODE(w->right))) {
                rbt_red(w);
                temp = INDEX2NODE(temp->parent);

            } else {
                if (rbt_is_black(INDEX2NODE(w->left))) {
                    rbt_black(INDEX2NODE(w->right));
                    rbt_red(w);
                    left_rotate(root, w);
                    w = INDEX2NODE(INDEX2NODE(temp->parent)->left);
                }

                rbt_copy_color(w, INDEX2NODE(temp->parent));
                rbt_black(INDEX2NODE(temp->parent));
                rbt_black(INDEX2NODE(w->left));
                right_rotate(root, INDEX2NODE(temp->parent));
                temp = INDEX2NODE(*root);
            }
        }
    }

    rbt_black(temp);
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
