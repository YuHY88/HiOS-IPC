/*
 * Buffering of output and input. 
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 * 
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA. 
 */

#include <zebra.h>

#include "memory.h"
#include "buffer.h"
#include "log.h"
#include "network.h"
#include <stddef.h>
#include "pkt_type.h"
#include <lib/pkt_buffer.h>
#include "module_id.h"
#include "ftm/pkt_ip.h"
#include "ftm/pkt_tcp.h"


/* It should always be true that: 0 <= sp <= cp <= size */

/* Default buffer size (used if none specified).  It is rounded up to the
   next page boundery. */
#define BUFFER_SIZE_DEFAULT		4096


#define BUFFER_DATA_FREE(D) XFREE(MTYPE_BUFFER, (D))

/* Make new buffer. */
struct buffer *
buffer_new (size_t size)
{
  struct buffer *b;

  b = XCALLOC (MTYPE_BUFFER, sizeof (struct buffer));

  if (size)
    b->size = size;
  else
    {
      static size_t default_size;
      if (!default_size)
        {
	  long pgsz = sysconf(_SC_PAGESIZE);
	  default_size = ((((BUFFER_SIZE_DEFAULT-1)/pgsz)+1)*pgsz);
	}
      b->size = default_size;
    }

  return b;
}

/* Free buffer. */
void
buffer_free (struct buffer *b)
{
  buffer_reset(b);
  XFREE (MTYPE_BUFFER, b);
}

/* Make string clone. */
char *
buffer_getstr (struct buffer *b)
{
  size_t totlen = 0;
  struct buffer_data *data;
  char *s;
  char *p;

  for (data = b->head; data; data = data->next)
    totlen += data->cp - data->sp;
  if (!(s = XMALLOC(MTYPE_TMP, totlen+1)))
    return NULL;
  p = s;
  for (data = b->head; data; data = data->next)
    {
      memcpy(p, data->data + data->sp, data->cp - data->sp);
      p += data->cp - data->sp;
    }
  *p = '\0';
  return s;
}

/* Return 1 if buffer is empty. */
int
buffer_empty (struct buffer *b)
{
  return (b->head == NULL);
}

/* Clear and free all allocated data. */
void
buffer_reset (struct buffer *b)
{
  struct buffer_data *data;
  struct buffer_data *next;
  
  for (data = b->head; data; data = next)
    {
      next = data->next;
      BUFFER_DATA_FREE(data);
    }
  b->head = b->tail = NULL;
}

/* Add buffer_data to the end of buffer. */
static struct buffer_data *
buffer_add (struct buffer *b)
{
  struct buffer_data *d;

  d = XCALLOC(MTYPE_BUFFER, offsetof(struct buffer_data, data) + b->size);
  d->cp = d->sp = 0;
  d->next = NULL;

  if (b->tail)
    b->tail->next = d;
  else
    b->head = d;
  b->tail = d;

  return d;
}

/* Delete current buffer_data from buffer. add by guoli*/
void buffer_data_delete (struct buffer *b, struct buffer_data *data)
{
  struct buffer_data *d_prev;
	struct buffer_data *d_next;
	struct buffer_data *d;
		
	d = b->head;
	assert(data);

	zlog_debug(ZLOG_LIB_DBG_PKG, "buffer_data_delete\n");

	/* delete head bufdata*/
	if(d == data)
	{
		/*head bufdata is not only bufdata in buffer*/
		if(d->next)
		{
			d_next = d->next;
			b->head = d_next;
		}
		else/*only head bufdata*/
		{
			b->head = b->tail = NULL;		
		}
		BUFFER_DATA_FREE(data);
	}
	else /*delete middle bufdata or tail bufdata*/
	{
		/*find bufdata in buffer, and delete*/
		for(; d != b->tail; d = d->next)
		{	
			if(d->next == data && d->next != b->tail)
			{
				d_prev = d;
				d_next = data->next;
				d_prev->next = data->next;
				BUFFER_DATA_FREE(data);
				break;
			}
			/*bufdata is the tail*/
			else if(d->next == data && d->next == b->tail)
			{
				d_prev = d;
				b->tail = d_prev;
				d->next = NULL;
				BUFFER_DATA_FREE(data);
				break;
			}
		}
		if(d == b->tail)
		{
			fprintf(stderr, "fail to find buffer_data, none buffer_data has been deleted\n");
			exit(0);
		}
	}
}


/* Write data to buffer. */
void
buffer_put(struct buffer *b, const void *p, size_t size)
{
  size_t chunk;
  struct buffer_data *data = b->tail;
  const char *ptr = p;
	
//	zlog_debug("buffer_put size:%d\nbuf_put:%s\n",size, p);

  /* We use even last one byte of data buffer. */
  while (size)    
    {
      /* If there is no data buffer add it. */
      if (data == NULL || data->cp == b->size)
      {
		data = buffer_add (b);
      }
	  
      chunk = ((size <= (b->size - data->cp)) ? size : (b->size - data->cp));
      memcpy ((data->data + data->cp), ptr, chunk);
      size -= chunk;
      ptr += chunk;
      data->cp += chunk;
    }
}

/*put data in current bufdata. add by guoli*/
void
buffer_put_current_bufdata(struct buffer *b, const void *p, size_t *size)
{
  size_t chunk;
  struct buffer_data *data = b->tail;
  const char *ptr = p;
	 
  if (data == NULL || data->cp == b->size)
  {
//		printf("thread_id %d %s %d \n",pthread_self(),__FUNCTION__,__LINE__);
		return ;
  }
   
  chunk = ((*size <= (b->size - data->cp)) ? *size : (b->size - data->cp));
  memcpy ((data->data + data->cp), ptr, chunk);
  *size -= chunk;
  ptr += chunk;
  data->cp += chunk;
}

/*alloc a new bufdata, and put data in. add by guoli*/
struct buffer_data*
buffer_put_new_bufdata(struct buffer *b, const void *p, size_t *size)
{
    size_t chunk;
  	struct buffer_data *data = b->tail;
  	const char *ptr = p;

	zlog_debug(ZLOG_LIB_DBG_PKG, "##RECV:buffer_put_new_bufdata size:%d\nbuf_put:%s\n",*size, ptr);
	if(*size < b->size)
	{
		data = buffer_add (b);
		memcpy ((data->data + data->cp), p, *size);
		data->cp += *size;
		zlog_debug(ZLOG_LIB_DBG_PKG, "data->cp:%d,size:%d\n",data->cp, *size);

		*size = 0;
	}
	else
	{
	    /* Add a new buffer_data. */
		data = buffer_add (b);
	    chunk = ((*size <= (b->size - data->cp)) ? *size : (b->size - data->cp));
	    memcpy ((data->data + data->cp), ptr, chunk);
	    *size -= chunk;
	    ptr += chunk;
	    data->cp += chunk;
	}

	return data;
}

/* Insert character into the buffer. */
void
buffer_putc (struct buffer *b, u_char c)
{
  buffer_put(b, &c, 1);
}

/* Put string to the buffer. */
void
buffer_putstr (struct buffer *b, const char *c)
{
  buffer_put(b, c, strlen(c));
}

/* Keep flushing data to the fd until the buffer is empty or an error is
   encountered or the operation would block. */
buffer_status_t
buffer_flush_all (struct buffer *b, int fd)
{
  buffer_status_t ret;
  struct buffer_data *head;
  size_t head_sp;

  if (!b->head)
    return BUFFER_EMPTY;
  head_sp = (head = b->head)->sp;
  /* Flush all data. */
  while ((ret = buffer_flush_available(b, fd)) == BUFFER_PENDING)
    {
      if ((b->head == head) && (head_sp == head->sp) && (errno != EINTR))
        /* No data was flushed, so kernel buffer must be full. */
	return ret;
      head_sp = (head = b->head)->sp;
    }

  return ret;
}

/* Flush enough data to fill a terminal window of the given scene (used only
   by vty telnet interface). */
buffer_status_t
buffer_flush_window (struct buffer *b, int fd, int width, int height, 
		     int erase_flag, int no_more_flag)
{
  int nbytes;
  int iov_alloc;
  int iov_index;
  struct iovec *iov;
  struct iovec small_iov[3];
  char more[] = " --More-- ";
  char erase[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		   ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		   0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08};
  struct buffer_data *data;
  int column;

  if (!b->head)
    return BUFFER_EMPTY;

  if (height < 1)
    {
      zlog_warn("%s called with non-positive window height %d, forcing to 1",
      		__func__, height);
      height = 1;
    }
  else if (height >= 2)
    height--;
  if (width < 1)
    {
      zlog_warn("%s called with non-positive window width %d, forcing to 1",
      		__func__, width);
      width = 1;
    }

  /* For erase and more data add two to b's buffer_data count.*/
  if (b->head->next == NULL)
    {
      iov_alloc = array_size(small_iov);
      iov = small_iov;
    }
  else
    {
      iov_alloc = ((height*(width+2))/b->size)+10;
      iov = XMALLOC(MTYPE_TMP, iov_alloc*sizeof(*iov));
    }
  iov_index = 0;

  /* Previously print out is performed. */
  if (erase_flag)
    {
      iov[iov_index].iov_base = erase;
      iov[iov_index].iov_len = sizeof erase;
      iov_index++;
    }

  /* Output data. */
  column = 1;  /* Column position of next character displayed. */
  for (data = b->head; data && (height > 0); data = data->next)
    {
      size_t cp;

      cp = data->sp;
      while ((cp < data->cp) && (height > 0))
        {
	  /* Calculate lines remaining and column position after displaying
	     this character. */
	  if (data->data[cp] == '\r')
	    column = 1;
	  else if ((data->data[cp] == '\n') || (column == width))
	    {
	      column = 1;
	      height--;
	    }
	  else
	    column++;
	  cp++;
        }
      iov[iov_index].iov_base = (char *)(data->data + data->sp);
      iov[iov_index++].iov_len = cp-data->sp;
      data->sp = cp;

      if (iov_index == iov_alloc)
	/* This should not ordinarily happen. */
        {
	  iov_alloc *= 2;
	  if (iov != small_iov)
	    {
	      zlog_warn("%s: growing iov array to %d; "
			"width %d, height %d, size %lu",
			__func__, iov_alloc, width, height, (u_long)b->size);
	      iov = XREALLOC(MTYPE_TMP, iov, iov_alloc*sizeof(*iov));
	    }
	  else
	    {
	      /* This should absolutely never occur. */
	      zlog_err("%s: corruption detected: iov_small overflowed; "
		       "head %p, tail %p, head->next %p",
		       __func__, (void *)b->head, (void *)b->tail,
		       (void *)b->head->next);
	      iov = XMALLOC(MTYPE_TMP, iov_alloc*sizeof(*iov));
	      memcpy(iov, small_iov, sizeof(small_iov));
	    }
	}
    }

  /* In case of `more' display need. */
  if (b->tail && (b->tail->sp < b->tail->cp) && !no_more_flag)
    {
      iov[iov_index].iov_base = more;
      iov[iov_index].iov_len = sizeof more;
      iov_index++;
    }


#ifdef IOV_MAX
  /* IOV_MAX are normally defined in <sys/uio.h> , Posix.1g.
     example: Solaris2.6 are defined IOV_MAX size at 16.     */
  {
    struct iovec *c_iov = iov;
    nbytes = 0; /* Make sure it's initialized. */

    while (iov_index > 0)
      {
	 int iov_size;

	 iov_size = ((iov_index > IOV_MAX) ? IOV_MAX : iov_index);
	 if ((nbytes = writev(fd, c_iov, iov_size)) < 0)
	   {
	     zlog_warn("%s: writev to fd %d failed: %s",
		       __func__, fd, safe_strerror(errno));
	     break;
	   }

	 /* move pointer io-vector */
	 c_iov += iov_size;
	 iov_index -= iov_size;
      }
  }
#else  /* IOV_MAX */
   if ((nbytes = writev (fd, iov, iov_index)) < 0)
     zlog_warn("%s: writev to fd %d failed: %s",
	       __func__, fd, safe_strerror(errno));
#endif /* IOV_MAX */

  /* Free printed buffer data. */
  while (b->head && (b->head->sp == b->head->cp))
    {
      struct buffer_data *del;
      if (!(b->head = (del = b->head)->next))
        b->tail = NULL;
      BUFFER_DATA_FREE(del);
    }

  if (iov != small_iov)
    XFREE (MTYPE_TMP, iov);

  return (nbytes < 0) ? BUFFER_ERROR :
  			(b->head ? BUFFER_PENDING : BUFFER_EMPTY);
}

/* This function (unlike other buffer_flush* functions above) is designed
to work with non-blocking sockets.  It does not attempt to write out
all of the queued data, just a "big" chunk.  It returns 0 if it was
able to empty out the buffers completely, 1 if more flushing is
required later, or -1 on a fatal write error. */
buffer_status_t
buffer_flush_available(struct buffer *b, int fd)
{

/* These are just reasonable values to make sure a significant amount of
data is written.  There's no need to go crazy and try to write it all
in one shot. */
#ifdef IOV_MAX
#define MAX_CHUNKS ((IOV_MAX >= 16) ? 16 : IOV_MAX)
#else
#define MAX_CHUNKS 16
#endif
#define MAX_FLUSH 131072

  struct buffer_data *d;
  size_t written;
  struct iovec iov[MAX_CHUNKS];
  size_t iovcnt = 0;
  size_t nbyte = 0;

  for (d = b->head; d && (iovcnt < MAX_CHUNKS) && (nbyte < MAX_FLUSH);
       d = d->next, iovcnt++)
    {
      iov[iovcnt].iov_base = d->data+d->sp;
      nbyte += (iov[iovcnt].iov_len = d->cp-d->sp);
    }

  if (!nbyte)
    /* No data to flush: should we issue a warning message? */
    return BUFFER_EMPTY;

  /* only place where written should be sign compared */
  if ((ssize_t)(written = writev(fd,iov,iovcnt)) < 0)
    {
      if (ERRNO_IO_RETRY(errno))
	/* Calling code should try again later. */
        return BUFFER_PENDING;
      zlog_warn("%s: write error on fd %d: %s",
		__func__, fd, safe_strerror(errno));
      return BUFFER_ERROR;
    }

  /* Free printed buffer data. */
  while (written > 0)
    {
      struct buffer_data *d;
      if (!(d = b->head))
        {
          zlog_err("%s: corruption detected: buffer queue empty, "
		   "but written is %lu", __func__, (u_long)written);
	  break;
        }
      if (written < d->cp-d->sp)
        {
	  d->sp += written;
	  return BUFFER_PENDING;
	}

      written -= (d->cp-d->sp);
      if (!(b->head = d->next))
        b->tail = NULL;
      BUFFER_DATA_FREE(d);
    }

  return b->head ? BUFFER_PENDING : BUFFER_EMPTY;

#undef MAX_CHUNKS
#undef MAX_FLUSH
}

buffer_status_t
buffer_write(struct buffer *b, int fd, const void *p, size_t size)
{
  ssize_t nbytes;

#if 0
  /* Should we attempt to drain any previously buffered data?  This could help
     reduce latency in pushing out the data if we are stuck in a long-running
     thread that is preventing the main select loop from calling the flush
     thread... */
  if (b->head && (buffer_flush_available(b, fd) == BUFFER_ERROR))
    return BUFFER_ERROR;
#endif
  if (b->head)
    /* Buffer is not empty, so do not attempt to write the new data. */
    nbytes = 0;
  else if ((nbytes = write(fd, p, size)) < 0)
    {
      if (ERRNO_IO_RETRY(errno))
        nbytes = 0;
      else
        {
	  zlog_warn("%s: write error on fd %d: %s",
		    __func__, fd, safe_strerror(errno));
	  return BUFFER_ERROR;
	}
    }
  /* Add any remaining data to the buffer. */
  {
    size_t written = nbytes;
    if (written < size)
      buffer_put(b, ((const char *)p)+written, size-written);
  }
  return b->head ? BUFFER_PENDING : BUFFER_EMPTY;
}

buffer_status_t
buffer_flush_window_ipc (struct buffer *b, uint32_t ip, uint16_t port, uint16_t vpn, int width, int height, 
		     int erase_flag, int no_more_flag)
{
  int nbytes = 0;
	//struct pkt_buffer *pkt = NULL;
//	char   *send_buf;
	struct send_type send_type[100];
	int send_index;

  char more[] = " --More-- ";
	//0x08是退格键\b，该数组用来清除上一页显示时最后一行的" --More-- "，实现与上页最后一行相接
  char erase[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		   ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		   0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08};
  struct buffer_data *data;
  int column;

  if (!b->head)
    return BUFFER_EMPTY;

	zlog_debug(ZLOG_LIB_DBG_PKG, "#WRITE:buffer_flush_window_ipc\n");
	if(width == 0)
		width = 80;	
	if(height == 0)
		height = 40;
	
  if (height < 1)
  {
    zlog_warn("#WRITE:%s called with non-positive window height %d, forcing to 1",
    		__func__, height);
    height = 1;
  }
	//height-- 是为了在处理more状态时将最后一行留给--more--字符
  else if (height >= 2)
    height--;
  if (width < 1)
  {
    zlog_warn("#WRITE:%s called with non-positive window width %d, forcing to 1",
    		__func__, width);
    width = 1;
  }

	send_index = 0;
  if (erase_flag)
  {
		send_type[send_index].send_buf = erase;
		send_type[send_index].send_len = sizeof(erase);
    send_index++;
  }

  /* Output data. */
	//下一个字符所要输出的列
  column = 1;  /* Column position of next character displayed. */
	//缓存中数据输完或处理完一页内容
  for (data = b->head; data && (height > 0); data = data->next)
  {
    size_t cp;

    cp = data->sp;

		//在这里计算剩余的行数
    while ((cp < data->cp) && (height > 0))
    {
		  /* Calculate lines remaining and column position after displaying
		     this character. */
		     //回车，光标回到行首，此时column = 1			     
		  for(int i = 0; i < 1000 && cp < data->cp && height > 0; i++)
		  {
			  if (data->data[cp] == '\r')
			    column = 1;
				//换行或光标达到行尾(width为行宽)，光标回到行首切换一行，height为余下的行数
			  else if ((data->data[cp] == '\n') || (column == width))
			    {
			      column = 1;
			      height--;
			    }
			  else
			    column++;
			  cp++;
		  }
			send_type[send_index].send_buf = (char *)(data->data + data->sp);
			send_type[send_index].send_len = cp - data->sp;
			send_index++;	
			data->sp = cp;
    }
  }

  /* In case of `more' display need. */
  if (b->tail && (b->tail->sp < b->tail->cp) && !no_more_flag)
    {
      send_type[send_index].send_buf = more;
      send_type[send_index].send_len = sizeof more;
      send_index++;
    }

	zlog_debug(ZLOG_LIB_DBG_PKG, "#WRITE:buffer_flush_window_ipc send_index:%d\n",send_index);
	for(int i = 0; i < send_index; i++)
	{
		buffer_pkt_send(ip, port, TCP_PORT_TELNET, vpn, send_type[i]);
	}

  /* Free printed buffer data. */
  while (b->head && (b->head->sp == b->head->cp))
    {
      struct buffer_data *del;
      if (!(b->head = (del = b->head)->next))
        b->tail = NULL;
      BUFFER_DATA_FREE(del);
    }

  return (nbytes < 0) ? BUFFER_ERROR :
  			(b->head ? BUFFER_PENDING : BUFFER_EMPTY);
}


int buffer_pkt_send(uint32_t dip, uint16_t dport, uint16_t sport, uint16_t vpn, struct send_type send)
{
	union pkt_control pkt_ctrl;
	//char pkt_buf[1024];
	int ret;
	//char *ppkt_buf;
	//unsigned int length_send_buf;
	
	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
	pkt_ctrl.ipcb.dip = dip;
	pkt_ctrl.ipcb.protocol = IP_P_TCP;
	pkt_ctrl.ipcb.dport = dport;
	pkt_ctrl.ipcb.sport = sport;
	pkt_ctrl.ipcb.vpn = vpn;
	pkt_ctrl.ipcb.ttl = 64;
	pkt_ctrl.ipcb.is_changed = 1;
	pkt_ctrl.ipcb.tos = 4;

	ret = pkt_send(PKT_TYPE_TCP, &pkt_ctrl, send.send_buf, send.send_len);
	if(ret != NET_SUCCESS)
	{
		zlog_err("%s[%d] call pkt_send failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

buffer_status_t
buffer_flush_available_ipc(struct buffer *b, uint32_t ip, uint16_t port, uint16_t vpn)
{

/* These are just reasonable values to make sure a significant amount of
data is written.  There's no need to go crazy and try to write it all
in one shot. */
#define MAX_FLUSH 131072
	struct send_type send_type1[100];
  struct buffer_data *data;
	int send_index = 0;
	int data_buf_num = 0;

  if (!b->head)
 	 return BUFFER_EMPTY;

	zlog_debug(ZLOG_LIB_DBG_PKG, "buffer_flush_available_ipc\n"); 

	for (data = b->head; data; data = data->next)
	{
		int send_buf_num = (data->cp - data->sp)/4096 + 1;
		if(data!=NULL)

		for(int i = 0; i < send_buf_num -1; i++)
		{
			send_type1[send_index].send_buf = (char *)(data->data + data->sp);
			
			send_type1[send_index].send_len = 4096;
			send_index++;	
			data->sp += 4096;
					
		}

		if(data->sp < data->cp)
		{
			send_type1[send_index].send_buf = (char *)(data->data + data->sp);
			send_type1[send_index].send_len = data->cp - data->sp;
			send_index++;	
			data->sp = data->cp;
		}
		data_buf_num++;
	}

	for(int i = 0; i < send_index; i++)
	{
		buffer_pkt_send(ip, port, TCP_PORT_TELNET, vpn, send_type1[i]);
	}

	for(int i = 0; i < data_buf_num; i++)
	{
		struct buffer_data *data;
		if(!(data = b->head))
		{
			zlog_err("buffer queue empty, ");
		  break;
		}

    if (!(b->head = data->next))
 			 b->tail = NULL;  //处理完后，链表中只剩下一个buffer_data，head == tail，将其指向NULL
				
	  BUFFER_DATA_FREE(data);	
	}

  return b->head ? BUFFER_PENDING : BUFFER_EMPTY;
}

/* Keep flushing data to the fd until the buffer is empty or an error is
   encountered or the operation would block. */
buffer_status_t
buffer_flush_all_ipc (struct buffer *b, uint32_t ip, uint16_t port, uint16_t vpn)
{
  buffer_status_t ret;
  struct buffer_data *head;
  size_t head_sp;

  if (!b->head)
    return BUFFER_EMPTY;
//	printf("ip:%x\tport:%x\n", ip, port);
  head_sp = (head = b->head)->sp;
  /* Flush all data. */
  while ((ret = buffer_flush_available_ipc(b, ip, port, vpn)) == BUFFER_PENDING)
    {
      if ((b->head == head) && (head_sp == head->sp) && (errno != EINTR))
        /* No data was flushed, so kernel buffer must be full. */
	return ret;
      head_sp = (head = b->head)->sp;
    }

  return ret;
}


