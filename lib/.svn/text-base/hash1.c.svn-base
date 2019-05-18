/***********************************************************************
*
* hash.c
*
* Implementation of hash tables.  Each item inserted must include
* a hash_bucket member.
*
* Copyright (C) 2002 Roaring Penguin Software Inc.
*
* This software may be distributed under the terms of the GNU General
* Public License, Version 2 or (at your option) any later version.
*
* LIC: GPL
*
***********************************************************************/

#include <limits.h>
#include "hash1.h"
#include <types.h>


#define BITS_IN_int     ( sizeof(int) * CHAR_BIT )
#define THREE_QUARTERS  ((int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH      ((int) (BITS_IN_int / 8))
#define HIGH_BITS       ( ~((unsigned int)(~0) >> ONE_EIGHTH ))
#define NULL 0


/**********************************************************************
* %FUNCTION: hash_init
* %ARGUMENTS:
*  tab -- hash table
*  hash_offset -- offset of hash_bucket data member in inserted items
*  compute -- pointer to function to compute hash value
*  compare -- pointer to comparison function.  Returns 0 if items are equal,
*             non-zero otherwise.
* %RETURNS:
*  Nothing
* %DESCRIPTION:
*  Initializes a hash table.
***********************************************************************/
void hios_hash_init(struct hash_table *tab, unsigned int hash_size,
	  unsigned int (*compute)(void *hash_key), int (*compare)(void *item, void *hash_key))
{
    unsigned int i;

    tab->hash_size = hash_size;
	tab->num_entries = 0;
    tab->compute_hash = compute;
    tab->compare = compare;
	
    for (i=0; i<HASHTAB_SIZE; i++) 
	{
		tab->buckets[i] = NULL;
    }
}


unsigned int hios_hash_num_entries(struct hash_table *tab)
{
    return tab->num_entries;
}


/******************************************************************************************
* %FUNCTION: hash_add
* %ARGUMENTS:
*  tab -- hash table to insert into
*  item -- the item we're inserting
* %RETURNS:
*  Nothing
* %DESCRIPTION:
*  Inserts an item into the hash bucket head.  It must not currently be in any hash table.
*******************************************************************************************/
int hios_hash_add(struct hash_table *tab, struct hash_bucket *item)
{
    unsigned int val;

    if(tab->hash_size == tab->num_entries)
        return -1;
	
	val = tab->compute_hash(item->hash_key);
    item->hashval = val;
	
    val %= HASHTAB_SIZE;
    item->prev = NULL;
    item->next = tab->buckets[val]; /*insert to the bucket head */
    if (item->next) 
	{
		item->next->prev = item;
    }
    tab->buckets[val] = item;
    tab->num_entries++;
	return 0;
}


/**********************************************************************
* %FUNCTION: hash_remove
* %ARGUMENTS:
*  tab -- hash table
*  item -- item in hash table
* %RETURNS:
*  Nothing
* %DESCRIPTION:
*  Removes item from hash table
***********************************************************************/
void hios_hash_delete(struct hash_table *tab, struct hash_bucket *item)
{
    unsigned int val = item->hashval % HASHTAB_SIZE;

    if (item->prev) 
	{
		item->prev->next = item->next;
    } 
	else 
	{
		tab->buckets[val] = item->next;
    }
    if (item->next) 
	{
		item->next->prev = item->prev;
    }
	
    tab->num_entries--;
}


/**********************************************************************
* %FUNCTION: hash_find
* %ARGUMENTS:
*  tab -- hash table
*  item -- item equal to one we're seeking (in the compare-function sense)
* %RETURNS:
*  A pointer to the item in the hash table, or NULL if no such item
* %DESCRIPTION:
*  Searches hash table for item.
***********************************************************************/
struct hash_bucket *hios_hash_find(struct hash_table *tab, void *hash_key)
{
    unsigned int val = tab->compute_hash(hash_key) % HASHTAB_SIZE;
    struct hash_bucket *item = NULL;
    struct hash_bucket *b = NULL;
	
    for ((b = tab->buckets[val]); b; b = b->next) 
	{
		item = b;
		if (tab->compare(item, hash_key) == 0) 
		{
			return item;
		}
    }
    return NULL;
}


/**********************************************************************
* %FUNCTION: hash_find_next
* %ARGUMENTS:
*  tab -- hash table
*  item -- an item returned by hash_find or hash_find_next
* %RETURNS:
*  A pointer to the next equal item in the hash table, or NULL if no such item
* %DESCRIPTION:
*  Searches hash table for anoter item equivalent to this one.  Search
*  starts from item.
***********************************************************************/
struct hash_bucket *hios_hash_find_next(struct hash_table *tab, void *item)
{
	struct hash_bucket *b = (struct hash_bucket *)item;
    struct hash_bucket *item2 = NULL;
	
    for (b = b->next; b; b = b->next) 
	{
		item2 = b;
		if (tab->compare(item, item2->hash_key) == 0) 
		{
			return item2;
		}
    }
    return NULL;
}


/**********************************************************************
* %FUNCTION: hash_start
* %ARGUMENTS:
*  tab -- hash table
*  cursor -- a void pointer to keep track of location
* %RETURNS:
*  "first" entry in hash table, or NULL if table is empty
* %DESCRIPTION:
*  Starts an iterator -- sets cursor so hash_next will return next entry.
***********************************************************************/
struct hash_bucket *hios_hash_start(struct hash_table *tab, void **cursor)
{
    int i;
    for (i=0; i<HASHTAB_SIZE; i++) 
	{
		if (tab->buckets[i]) 
		{
		    /* Point cursor to NEXT item so it is valid even if current item is free'd */
		    *cursor = hios_hash_next_cursor(tab, tab->buckets[i]);
		    return tab->buckets[i];
		}
    }
    *cursor = NULL;
    return NULL;
}


/**********************************************************************
* %FUNCTION: hash_next
* %ARGUMENTS:
*  tab -- hash table
*  cursor -- cursor into hash table
* %RETURNS:
*  Next item of cursor in table, or NULL.
* %DESCRIPTION:
*  Steps cursor to next item in table.
***********************************************************************/
struct hash_bucket *hios_hash_next(struct hash_table *tab, void **cursor)
{
    struct hash_bucket *b;

    if (!*cursor) return NULL;

    b = (struct hash_bucket *) *cursor;
    *cursor = hios_hash_next_cursor(tab, b);
    return b;
}


/**********************************************************************
* %FUNCTION: hash_next_cursor
* %ARGUMENTS:
*  tab -- a hash table
*  b -- a hash bucket
* %RETURNS:
*  Cursor value for bucket following b in hash table.
***********************************************************************/
void *hios_hash_next_cursor(struct hash_table *tab, struct hash_bucket *b)
{
    unsigned int i;
    if (!b) return NULL;
    if (b->next) return b->next;

    i = b->hashval % HASHTAB_SIZE;
    for (++i; i<HASHTAB_SIZE; ++i) 
	{
		if (tab->buckets[i]) 
			return tab->buckets[i];
    }
    return NULL;
}


/**********************************************************************
* %FUNCTION: hash_string
* %ARGUMENTS:
*  str -- a zero-terminated string
* %RETURNS:
*  calcuate hash value using the hashpjw algorithm
* %DESCRIPTION:
*  An adaptation of Peter Weinberger's (PJW) generic hashing
*  algorithm based on Allen Holub's version. Accepts a pointer
*  to a datum to be hashed and returns an unsigned integer.
***********************************************************************/
unsigned int hios_hash_string(char const *str)
{
    unsigned int hash_value, i;

    for (hash_value = 0; *str; ++str) 
	{
        hash_value = ( hash_value << ONE_EIGHTH ) + *str;
        if (( i = hash_value & HIGH_BITS ) != 0 ) 
		{
            hash_value = ( hash_value ^ ( i >> THREE_QUARTERS )) & (~HIGH_BITS);
		}
    }
    return hash_value;
}
