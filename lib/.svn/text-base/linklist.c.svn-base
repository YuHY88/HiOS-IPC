/* Generic linked list routine.
 * Copyright (C) 1997, 2000 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <zebra.h>

#include "linklist.h"
#include "log.h"
#include <lib/memory.h>
#include <lib/memtypes.h>


/* Allocate new list. */
struct list *list_new (void)
{
	struct list *list_temp = NULL;
	
  	do
  	{
  		list_temp = XCALLOC (MTYPE_LINK_LIST, sizeof (struct list));
  		if (list_temp == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(list_temp == NULL);

	memset(list_temp, 0, sizeof (struct list));

	return list_temp;
}

/* Free list. */
void list_free (struct list *l)
{
  XFREE (MTYPE_LINK_LIST, l);
}

/* Allocate new listnode.  Internal use only. */
struct listnode *listnode_new (void)
{
	struct listnode *listnode_temp = NULL;
	
  	do
  	{
  		listnode_temp = XCALLOC (MTYPE_LINK_NODE, sizeof (struct listnode));
  		if (listnode_temp == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(listnode_temp == NULL);

	memset(listnode_temp, 0, sizeof (struct listnode));

	return listnode_temp;
}

/* Free listnode. */
void listnode_free (struct listnode *node)
{
  XFREE (MTYPE_LINK_NODE, node);
}


/* 查找一个节点 */
struct listnode *listnode_lookup (struct list *list, void *data)
{
  struct listnode *node;

  assert(list);
  for (node = listhead(list); node; node = listnextnode (node))
    if (data == listgetdata (node))
      return node;
  return NULL;
}


/* 返回链表头  */
void *listnode_head (struct list *list)
{
  struct listnode *node;

  assert(list);
  node = list->head;

  if (node)
    return node->data;
  return NULL;
}

/* 返回链表尾*/
void *listnode_tail (struct list *list)
{
  struct listnode *node;

  assert(list);
  node = list->tail;

  if (node)
    return node->data;
  return NULL;
}


/* 新增一个节点 */
void listnode_add (struct list *list, void *val)
{
  struct listnode *node;
  
  assert (val != NULL);
  
  node = listnode_new ();

  node->prev = list->tail;
  node->data = val;

  if (list->head == NULL)
    list->head = node;
  else
    list->tail->next = node;
  list->tail = node;

  list->count++;
}


/* 按照顺序插入一个节点
 * Add a node to the list.  If the list was sorted according to the
 * cmp function, insert a new node with the given val such that the
 * list remains sorted.  The new node is always inserted; there is no
 * notion of omitting duplicates.
 */
void listnode_add_sort (struct list *list, void *val)
{
  struct listnode *n;
  struct listnode *new;
  
  assert (val != NULL);
  
  new = listnode_new ();
  new->data = val;

  if (list->cmp)
    {
      for (n = list->head; n; n = n->next)
	{
	  if ((*list->cmp) (val, n->data) < 0)
	    {	    
	      new->next = n;
	      new->prev = n->prev;

	      if (n->prev)
		n->prev->next = new;
	      else
		list->head = new;
	      n->prev = new;
	      list->count++;
	      return;
	    }
	}
    }

  new->prev = list->tail;

  if (list->tail)
    list->tail->next = new;
  else
    list->head = new;

  list->tail = new;
  list->count++;
}


/* 添加到指定位置前面 */
void listnode_add_prev (struct list *list, struct listnode *current, void *val)
{
  struct listnode *node;
  
  assert (val != NULL);
  
  node = listnode_new ();
  node->next = current;
  node->data = val;

  if ((current == NULL) || (current->prev == NULL))
    list->head = node;
  else
    current->prev->next = node;

  node->prev = current->prev;
  current->prev = node;

  list->count++;
}

static void list_add_node_next(struct list *list, struct listnode *current, struct listnode *node)
{        
    if(current == NULL)
    {
        if(list->head)
			list->head->prev = node;
        else list->tail = node;

        node->next = list->head;
        node->prev = current;

        list->head = node;
    }
    else
    {
        if(current->next)
			current->next->prev = node;
        else list->tail = node;

        node->next = current->next;
        node->prev = current;

        current->next = node;
    }
    
    list->count++;
}

/* 添加到指定位置的后面 */
void listnode_add_next (struct list *list, struct listnode *current, void *val)
{
  struct listnode *node;
  
  assert (val != NULL);
  
  node = listnode_new ();
  node->prev = current;
  node->data = val;

  list_add_node_next(list, current, node);
}


/* 删除指定的数据 */
void listnode_delete (struct list *list, void *val)
{
  struct listnode *node;

  assert(list);
  for (node = list->head; node; node = node->next)
    {
      if (node->data == val)
	{
	  if (node->prev)
	    node->prev->next = node->next;
	  else
	    list->head = node->next;

	  if (node->next)
	    node->next->prev = node->prev;
	  else
	    list->tail = node->prev;

	  list->count--;
	  listnode_free (node);
	  return;
	}
    }
}

/* 将指定节点移到链表尾部 */
void listnode_move_to_tail (struct list *l, struct listnode *n)
{
  LISTNODE_DETACH(l,n);
  LISTNODE_ATTACH(l,n);
}


/* 删除整个链表 */
void list_delete (struct list *list)
{
  assert(list);
  list_delete_all_node (list);
  list_free (list);
}


/* 删除链表的所有节点，但是保留链表 */
void list_delete_all_node (struct list *list)
{
  struct listnode *node;
  struct listnode *next;

  assert(list);
  for (node = list->head; node; node = next)
    {
      next = node->next;
      if (list->del)
	(*list->del) (node->data);
      listnode_free (node);
    }
  list->head = list->tail = NULL;
  list->count = 0;
}


/* 添加一个节点 */
void list_add_node(struct list *list, struct listnode *node)
{  
  node->next = NULL;
  node->prev = list->tail;

  if(list->head == NULL)
       list->head = node;
  else list->tail->next = node;

  list->tail = node;
  list->count++;
}


/* 删除并释放一个节点 */
void list_delete_node (struct list *list, struct listnode *node)
{
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;
  list->count--;
  listnode_free (node);
}


/* 将节点从链表删除，但是不释放内存 */
void list_detach_node(struct list *list, struct listnode *node)
{
  if(node->prev)
       node->prev->next = node->next;
  else list->head = node->next;
  
  if(node->next)
       node->next->prev = node->prev;
  else list->tail = node->prev;

  list->count--;
}





/* 将两个链表合并 */
void list_add_list (struct list *l, struct list *m)
{
  struct listnode *n;

  for (n = listhead (m); n; n = listnextnode (n))
    listnode_add (l, n->data);
}


/* add by suxq 2016-11-16*/
/* insert after special node pos*/
void list_insert_bynode(struct list *list, struct listnode *posi, struct listnode *node)
{        
    if(posi == NULL)
    {
        if(list->head)
	         list->head->prev = node;
        else list->tail = node;

        node->next = list->head;
        node->prev = posi;

        list->head = node;
    }
    else
    {
        if(posi->next)
	         posi->next->prev = node;
        else list->tail = node;

        node->next = posi->next;
        node->prev = posi;

        posi->next = node;
    }
    
    list->count++;
}

/* add by suxq 2016-11-16*/
/* append at tail */
void list_add_bynode(struct list *list, struct listnode *node)
{  
  node->next = NULL;
  node->prev = list->tail;

  if(list->head == NULL)
       list->head = node;
  else list->tail->next = node;

  list->tail = node;

  list->count++;
}

/* add by suxq 2016-11-16*/
void list_delete_bynode(struct list *list, struct listnode *node)
{
  if(node->prev)
       node->prev->next = node->next;
  else list->head = node->next;
  
  if(node->next)
       node->next->prev = node->prev;
  else list->tail = node->prev;

  list->count--;
}

