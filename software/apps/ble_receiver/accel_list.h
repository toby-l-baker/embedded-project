#ifndef SLIST_H
#define SLIST_H

struct snode {
    float time_stamp;
    float accel;
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
void slist_add_tail(slist * list, float time_stamp, float data);
void slist_add_head(slist * list, float time_stamp, float data);
float slist_remove_head(slist * list);
float slist_remove_tail(slist * list);
float slist_remove(slist *list, snode *node, snode *previous);
unsigned int slist_get_count(const slist * list);

#endif /* SLIST_H */