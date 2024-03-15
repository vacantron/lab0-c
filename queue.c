#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, head, list)
        q_release_element(iter);

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *el = (element_t *) malloc(sizeof(element_t));
    if (!el)
        return false;

    size_t len = strlen(s);
    el->value = (char *) malloc(sizeof(char) * (len + 1));
    if (!el->value) {
        free(el);
        return false;
    }

    /* null-terminated */
    strncpy(el->value, s, len);
    el->value[len] = 0;
    list_add(&el->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *el = (element_t *) malloc(sizeof(element_t));
    if (!el)
        return false;

    size_t len = strlen(s);
    el->value = (char *) malloc(sizeof(char) * (len + 1));
    if (!el->value) {
        free(el);
        return false;
    }

    /* null-ternimated */
    strncpy(el->value, s, strlen(s));
    el->value[len] = 0;
    list_add_tail(&el->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *el = list_first_entry(head, element_t, list);
    list_del_init(&el->list);

    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }

    return el;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *el = list_last_entry(head, element_t, list);
    list_del_init(&el->list);

    if (sp) {
        strncpy(sp, el->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }

    return el;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int i = 0;
    struct list_head *iter;
    list_for_each (iter, head) {
        i++;
    }

    return i;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = slow;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del_init(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    q_sort(head, 0);

    bool is_dup = false;
    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, head, list) {
        if (&safe->list != head && !strcmp(iter->value, safe->value)) {
            list_del_init(&iter->list);
            q_release_element(iter);
            is_dup = true;
            continue;
        }

        if (is_dup) {
            list_del_init(&iter->list);
            q_release_element(iter);
            is_dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    LIST_HEAD(t0);
    LIST_HEAD(t1);

    size_t cnt = 0;
    struct list_head *iter, *safe, *_head = &t0, *__head = &t1;
    list_for_each_safe (iter, safe, head) {
        list_del_init(iter);
        list_add(iter, _head);

        if (cnt & 1)
            list_splice_tail_init(_head, __head);

        cnt++;
    }

    if (cnt & 1)
        list_splice_tail_init(_head, __head);

    list_splice_init(__head, head);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *iter, *safe;
    list_for_each_safe (iter, safe, head) {
        iter->prev = (struct list_head *) ((uintptr_t) iter->prev ^
                                           (uintptr_t) iter->next);
        iter->next = (struct list_head *) ((uintptr_t) iter->next ^
                                           (uintptr_t) iter->prev);
        iter->prev = (struct list_head *) ((uintptr_t) iter->prev ^
                                           (uintptr_t) iter->next);
    }

    head->prev =
        (struct list_head *) ((uintptr_t) head->prev ^ (uintptr_t) head->next);
    head->next =
        (struct list_head *) ((uintptr_t) head->next ^ (uintptr_t) head->prev);
    head->prev =
        (struct list_head *) ((uintptr_t) head->prev ^ (uintptr_t) head->next);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    LIST_HEAD(t0);
    LIST_HEAD(t1);

    size_t cnt = 0;
    struct list_head *iter, *safe, *_head = &t0, *__head = &t1;
    list_for_each_safe (iter, safe, head) {
        list_del_init(iter);
        list_add(iter, _head);

        cnt++;

        if (cnt != k)
            continue;

        cnt = 0;
        list_splice_tail_init(_head, __head);
    }

    if (cnt)
        list_splice_tail_init(_head, __head);

    list_splice_init(__head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    LIST_HEAD(t0);
    LIST_HEAD(t1);

    struct list_head *_head = &t0, *__head = &t1;
    element_t *iter, *safe;

    list_cut_position(_head, head, head->next);

    list_for_each_entry_safe (iter, safe, head, list) {
        bool done = false;
        element_t *_iter;

        list_for_each_entry (_iter, _head, list) {
            if (descend) {
                if (strcmp(iter->value, _iter->value) > 0) {
                    list_del_init(&iter->list);
                    list_cut_position(__head, _head, _iter->list.prev);
                    list_add_tail(&iter->list, __head);
                    list_splice_init(__head, _head);
                    done = true;
                    break;
                }
            } else {
                if (strcmp(iter->value, _iter->value) < 0) {
                    list_del_init(&iter->list);
                    list_cut_position(__head, _head, _iter->list.prev);
                    list_add_tail(&iter->list, __head);
                    list_splice_init(__head, _head);
                    done = true;
                    break;
                }
            }
        }

        if (!done) {
            list_del_init(&iter->list);
            list_add_tail(&iter->list, _head);
        }
    }

    list_splice_init(_head, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, head, list) {
        if (&safe->list == head)
            break;

        for (element_t *_iter = safe; &_iter->list != head;
             _iter = container_of(_iter->list.next, element_t, list)) {
            if (strcmp(iter->value, _iter->value) >= 0) {
                list_del_init(&iter->list);
                q_release_element(iter);
                goto iter_end;
            }
        }
    iter_end:;
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, head, list) {
        if (&safe->list == head)
            break;

        for (element_t *_iter = safe; &_iter->list != head;
             _iter = container_of(_iter->list.next, element_t, list)) {
            if (strcmp(iter->value, _iter->value) <= 0) {
                list_del_init(&iter->list);
                q_release_element(iter);
                goto iter_end;
            }
        }
    iter_end:;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *ctx = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return ctx->size;

    for (struct list_head *iter = head->next->next; iter != head;
         iter = iter->next) {
        queue_contex_t *_ctx = list_entry(iter, queue_contex_t, chain);
        list_splice_tail_init(_ctx->q, ctx->q);
        _ctx->size = 0;
        ctx->size = q_size(ctx->q);
    }

    q_sort(ctx->q, descend);
    return ctx->size;
}
