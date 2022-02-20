#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *node = malloc(sizeof(struct list_head));
    if (!node)
        return NULL;

    INIT_LIST_HEAD(node);
    return node;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    while (!list_empty(l)) {
        struct list_head *curr = l->next;
        list_del(curr);
        q_release_element(container_of(curr, element_t, list));
    }
    list_del(l);
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *el = malloc(sizeof(element_t));
    if (!el)
        return false;

    size_t buf = strlen(s) + 1;
    el->value = malloc(buf);
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, buf);
    list_add(&el->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *el = malloc(sizeof(element_t));
    if (!el)
        return false;

    size_t buf = strlen(s) + 1;
    el->value = malloc(buf);
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, buf);
    list_add_tail(&el->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    struct list_head *curr = head->next;
    list_del_init(curr);
    element_t *el = container_of(curr, element_t, list);
    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return el;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    struct list_head *curr = head->prev;
    list_del_init(curr);
    element_t *el = container_of(curr, element_t, list);
    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return el;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (list_empty(head))
        return 0;

    int cnt = 0;
    struct list_head *curr = head->next;
    while (curr != head) {
        ++cnt;
        curr = curr->next;
    }
    return cnt;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return NULL if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    int n = q_size(head) >> 1;
    struct list_head *curr = head->next;
    for (size_t i = 0; i < n; i++) {
        curr = curr->next;
    }
    list_del(curr);
    q_release_element(container_of(curr, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;

    element_t *s = list_entry(head->next, element_t, list);
    element_t *t = list_entry(head->next->next, element_t, list);
    while (&t->list != head) {
        size_t len = strlen(s->value) > strlen(t->value) ? strlen(t->value)
                                                         : strlen(s->value);
        if (strncmp(s->value, t->value, len + 1) == 0) {
            element_t *tmp = t;
            t = list_entry((&t->list)->next, element_t, list);
            list_del(&tmp->list);
            q_release_element(tmp);
        } else {
            s = t;
            t = list_entry((&t->list)->next, element_t, list);
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    int n = q_size(head) >> 1;
    bool is_odd = q_size(head) % 2;
    struct list_head *prev = head, *curr = head->next;
    for (size_t i = 0; i < n; i++) {
        struct list_head *next = curr->next, *tmp;
        prev->next = next;
        tmp = next->next;
        next->prev = prev;
        next->next = curr;
        curr->prev = next;
        prev = curr;
        curr = tmp;
    }
    if (is_odd) {
        prev->next = head->prev;
        head->prev->prev = prev;
    } else {
        prev->next = head;
        head->prev = prev;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head))
        return;

    struct list_head *curr = head, *tmp = head->next;
    while (tmp != head) {
        struct list_head *next = tmp;
        curr->prev = next;
        tmp = next->next;
        next->next = curr;
        curr = next;
    }
    curr->prev = head;
    head->next = curr;
}

void swap(element_t *, element_t *);
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head))
        return;

    bool is_swapped = false;
    int displacement = q_size(head) - 1;
    do {
        is_swapped = false;
        element_t *s = list_entry(head->next, element_t, list);
        element_t *t = list_entry(head->next->next, element_t, list);

        for (size_t i = 0; i < displacement; i++) {
            size_t len = strlen(s->value) > strlen(t->value) ? strlen(t->value)
                                                             : strlen(s->value);
            if (strncmp(s->value, t->value, ++len) > 0) {
                swap(s, t);
                is_swapped = true;
            }
            s = t;
            t = list_entry((&s->list)->next, element_t, list);
        }
        if (!is_swapped)
            --displacement;
    } while (is_swapped);
}

void swap(element_t *s, element_t *t)
{
    struct list_head *prev = (&s->list)->prev;
    struct list_head *next = (&t->list)->next;
    (&s->list)->prev = &t->list;
    (&s->list)->next = next;
    (&t->list)->prev = prev;
    (&t->list)->next = &s->list;

    prev->next = &t->list;
    next->prev = &s->list;
}