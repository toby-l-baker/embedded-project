#include <stdlib.h>
#include <accel_list.h>

static snode * snode_create(float time_stamp, float accel)
{
    snode * node = malloc(sizeof(snode));
    if (node) {
        node->time_stamp = time_stamp;
        node->accel = accel;
        node->next = NULL;
    }

    return node;
}

slist * slist_create(void)
{
    slist * list = malloc(sizeof(slist));
    if (list) {
        list->head = NULL;
        list->tail = NULL;
        list->count = 0;
    }

    return list;
}

void slist_empty(slist * list)
{
    snode * node, * temp;
    node = list->head;
    while (node != NULL) {
        temp = node->next;
        free(node);
        node = temp;
    }
}

void slist_delete(slist * list)
{
    if (list) {
        slist_empty(list);
        free(list);
    }
}

void slist_add_tail(slist * list, float time_stamp, float accel)
{
    snode * node = snode_create(time_stamp, accel);
    if (list->head == NULL) {
        /* Adding the first node */
        list->head = node;
        list->tail = node;
    }
    else {
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;
}

void slist_add_head(slist * list, float time_stamp, float accel)
{
    snode * node = snode_create(time_stamp, accel);
    if (list->tail == NULL) {
        /* Adding the first node */
        list->head = node;
        list->tail = node;
    }
    else {
        node->next = list->head;
        list->head = node;
    }
    list->count++;
}

float slist_remove_head(slist * list)
{
    float data = 0.0;

    if (list->head) {
        snode *temp = list->head;
        if (list->head->next) {
            list->head = list->head->next;
        }
        else {
            /* List is now empty */
            list->head = NULL;
            list->tail = NULL;
        }
        data = temp->time_stamp;
        free(temp);
        list->count--;
        if (list->count == 1) {
            list->tail = list->head;
        }
    }

    return data;
}

float slist_remove_tail(slist * list)
{
    float data = 0;

    if (list->tail) {
        snode *current, *previous = NULL;
        current = list->head;
        while (current->next) {
            previous = current;
            current = current->next;
        }
        data = current->time_stamp;
        free(current);
        if (previous) {
            previous->next = NULL;
        }
        else {
            /* List is now empty */
            list->head = NULL;
            list->tail = NULL;
        }
        list->count--;
        if (list->count == 1) {
            list->head = list->tail;
        }
    }

    return data;
}

float slist_remove(slist *list, snode *node, snode *previous)
{
    float data;
    if (node == list->head) {
        data = slist_remove_head(list);
    }
    else {
        previous->next = node->next;
        data = node->time_stamp;
        list->count--;
        if (list->count == 1) {
            list->tail = list->head;
        }
        else if (node == list->tail) {
            list->tail = previous;
        }
        free(node);
    }
    return data;
}


unsigned int slist_get_count(const slist * list)
{
    return list->count;
}