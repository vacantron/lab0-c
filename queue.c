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

    el->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, strlen(s));
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

    el->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!el->value) {
        free(el);
        return false;
    }

    strncpy(el->value, s, strlen(s));
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
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
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
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, head, list) {
        if (strcmp(iter->value, safe->value) < 0)
            continue;

        list_del_init(&iter->list);
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
        if (strcmp(iter->value, safe->value) > 0)
            continue;

        list_del_init(&iter->list);
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
