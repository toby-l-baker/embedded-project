#ifndef SLIST_H
#define SLIST_H

struct snode {
    float data;
    struct snode * next;
};
typedef struct snode snode;

struct slist {
    snode * head;
    snode * tail;
    unsigned int count;
};
typedef struct slist slist;

typedef void (*slist_forfn)(float);

slist * slist_create(void);
void slist_empty(slist * list);
void slist_delete(slist * list);
void slist_add_tail(slist * list, float data);
void slist_add_head(slist * list, float data);
float slist_remove_head(slist * list);
float slist_remove_tail(slist * list);
float slist_remove(slist *list, snode *node, snode *previous);
void slist_insert_before(slist * list, snode * node, snode * previous, float data);
snode * slist_insert_after(slist * list, snode * node,float data);
void slist_for_each(const slist * list, slist_forfn fun);
unsigned int slist_get_count(const slist * list);

#endif /* SLIST_H */
