/*
   Command interpreter routine for virtual terminal [aka TeletYpe]
   Copyright (C) 1997, 98, 99 Kunihiro Ishiguro
   Copyright (C) 2013 by Open Source Routing.
   Copyright (C) 2013 by Internet Systems Consortium, Inc. ("ISC")

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2, or (at your
option) any later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <zebra.h>


#include "memory.h"
#include "memtypes.h"
#include "log.h"
#include "version.h"
#include "linklist.h"
#include "inet_ip.h"
#include "thread.h"
#include "command.h"
//#include "workqueue.h"
#include "sockunion.h"


/* Command vector which includes some level of command lists. Normally
   each daemon maintains each own cmdvec. */
vector cmdvec = NULL;

struct cmd_token token_cr;
char *command_cr = NULL;

struct match_flag validity_check;

enum filter_type
{
  FILTER_RELAXED,
  FILTER_STRICT
};

enum matcher_rv
{
  MATCHER_OK,
  MATCHER_COMPLETE,
  MATCHER_INCOMPLETE,
  MATCHER_NO_MATCH,
  MATCHER_AMBIGUOUS,
  MATCHER_EXCEED_ARGC_MAX,
  MATCHER_KEYWORD_UNMATCH,  //added by gl
  MATCHER_OPTIONAL_UNMATCH
};

#define MATCHER_ERROR(matcher_rv) \
  (   (matcher_rv) == MATCHER_INCOMPLETE \
   || (matcher_rv) == MATCHER_NO_MATCH \
   || (matcher_rv) == MATCHER_AMBIGUOUS \
   || (matcher_rv) == MATCHER_EXCEED_ARGC_MAX \
  )

/* Standard command node structures. */
static struct cmd_node auth_node =
{
  AUTH_NODE,
  "Password: ",
};

static struct cmd_node user_node =
{
    USER_NODE,
    "%s# ",
};

static struct cmd_node config_node =
{
  CONFIG_NODE,
  "%s(config)# ",
  1
};

static struct cmd_node sysrun_node =
{
  SYSRUN_NODE,
  "%s(sysrun)# ",
};

static const struct facility_map {
  int facility;
  const char *name;
  size_t match;
} syslog_facilities[] =
  {
    { LOG_KERN, "kern", 1 },
    { LOG_USER, "user", 2 },
    { LOG_MAIL, "mail", 1 },
    { LOG_DAEMON, "daemon", 1 },
    { LOG_AUTH, "auth", 1 },
    { LOG_SYSLOG, "syslog", 1 },
    { LOG_LPR, "lpr", 2 },
    { LOG_NEWS, "news", 1 },
    { LOG_UUCP, "uucp", 2 },
    { LOG_CRON, "cron", 1 },
#ifdef LOG_FTP
    { LOG_FTP, "ftp", 1 },
#endif
    { LOG_LOCAL0, "local0", 6 },
    { LOG_LOCAL1, "local1", 6 },
    { LOG_LOCAL2, "local2", 6 },
    { LOG_LOCAL3, "local3", 6 },
    { LOG_LOCAL4, "local4", 6 },
    { LOG_LOCAL5, "local5", 6 },
    { LOG_LOCAL6, "local6", 6 },
    { LOG_LOCAL7, "local7", 6 },
    { 0, NULL, 0 },
  };

//static const char *
//facility_name(int facility)
//{
//  const struct facility_map *fm;

//  for (fm = syslog_facilities; fm->name; fm++)
//    if (fm->facility == facility)
//      return fm->name;
//  return "";
//}

//static int
//facility_match(const char *str)
//{
//  const struct facility_map *fm;

//  for (fm = syslog_facilities; fm->name; fm++)
//    if (!strncmp(str,fm->name,fm->match))
//      return fm->facility;
//  return -1;
//}
/*

static int
level_match(const char *s)
{
  int level ;

  for ( level = 0 ; zlog_priority [level] != NULL ; level ++ )
    if (!strncmp (s, zlog_priority[level], 2))
      return level;
  return ZLOG_DISABLED;
}*/

/* This is called from main when a daemon is invoked with -v or --version. */
void
print_version (const char *progname)
{

}


/* Utility function to concatenate argv argument into a single string
   with inserting ' ' character between each argument.  */
char *
argv_concat (const char **argv, int argc, int shift)
{
  int i;
  size_t len;
  char *str;
  char *p;

  len = 0;
  for (i = shift; i < argc; i++)
    len += strlen(argv[i])+1;
  if (!len)
    return NULL;
  p = str = XMALLOC(MTYPE_TMP, len);
  for (i = shift; i < argc; i++)
    {
      size_t arglen;
      memcpy(p, argv[i], (arglen = strlen(argv[i])));
      p += arglen;
      *p++ = ' ';
    }
  *(p-1) = '\0';
  return str;
}

/* Install top node of command vector. */
void
install_node_factory (struct cmd_node *node,
	      int (*func) (struct vty *))
{
  vector_set_index (cmdvec, node->node, node);
  node->func_factory = func;
  node->cmd_vector = vector_init (VECTOR_MIN_SIZE);
/*
fprintf(stdout, "node = %d\n", node->node);
if(node->node == DEBUG_NODE)
fprintf(stdout, "DEBUG_NODE\n");
*/
}

/* Install top node of command vector. */
void
install_node (struct cmd_node *node,
	      int (*func) (struct vty *))
{
  vector_set_index (cmdvec, node->node, node);
  node->func = func;
  node->cmd_vector = vector_init (VECTOR_MIN_SIZE);
/*
fprintf(stdout, "node = %d\n", node->node);
if(node->node == DEBUG_NODE)
fprintf(stdout, "DEBUG_NODE\n");
*/
}

/* Breaking up string into each command piece. I assume given
   character is separated by a space character. Return value is a
   vector which includes char ** data element. */
vector
cmd_make_strvec (const char *string)
{
  const char *cp, *start;
  char *token;
  int strlen;
  vector strvec;

  if (string == NULL)
    return NULL;

  cp = string;

  /* Skip white spaces. */
  while (isspace ((int) *cp) && *cp != '\0')
    cp++;

  /* Return if there is only white spaces */
  if (*cp == '\0')
    return NULL;

  if (*cp == '!' || *cp == '#')
    return NULL;

  /* Prepare return vector. */
  strvec = vector_init (VECTOR_MIN_SIZE);

  /* Copy each command piece and set into vector. */
  while (1)
    {
      start = cp;
      while (!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n') &&
	     *cp != '\0')
	cp++;
      strlen = cp - start;
      token = XMALLOC (MTYPE_STRVEC, strlen + 1);
      memcpy (token, start, strlen);
      *(token + strlen) = '\0';
      vector_set (strvec, token);

      while ((isspace ((int) *cp) || *cp == '\n' || *cp == '\r') &&
	     *cp != '\0')
	cp++;

      if (*cp == '\0')
	return strvec;
    }
}

/* Free allocated string vector. */
void
cmd_free_strvec (vector v)
{
  unsigned int i;
  char *cp;

  if (!v)
    return;

  for (i = 0; i < vector_active (v); i++)
    if ((cp = vector_slot (v, i)) != NULL)
      XFREE (MTYPE_STRVEC, cp);

  vector_free (v);
}

struct format_parser_state
{
  vector topvect; /* Top level vector */
  vector intvect; /* Intermediate level vector, used when there's
                   * a multiple in a keyword. */
  vector curvect; /* current vector where read tokens should be
                     appended. */

  const char *string; /* pointer to command string, not modified */
  const char *cp; /* pointer in command string, moved along while
                     parsing */
  const char *dp;  /* pointer in description string, moved along while
                     parsing */

	int in_brackets; /* flag to remember if we are in a brackets group */
  int in_keyword; /* flag to remember if we are in a keyword group */
  int in_multiple; /* flag to remember if we are in a multiple group */
  int just_read_word; /* flag to remember if the last thing we red was a
                       * real word and not some abstract token */
};

static void
format_parser_error(struct format_parser_state *state, const char *message)
{
  int offset = state->cp - state->string + 1;

  fprintf(stderr, "\nError parsing command: \"%s\"\n", state->string);
  fprintf(stderr, "                        %*c\n", offset, '^');
  fprintf(stderr, "%s at offset %d.\n", message, offset);
  fprintf(stderr, "This is a programming error. Check your DEFUNs etc.\n");
  exit(1);
}

static char *
format_parser_desc_str(struct format_parser_state *state)
{
  const char *cp, *start;
  char *token;
  int strlen;

  cp = state->dp;

  if (cp == NULL)
    return NULL;

  /* Skip white spaces. */
  while (isspace ((int) *cp) && *cp != '\0')
    cp++;

  /* Return if there is only white spaces */
  if (*cp == '\0')
    return NULL;

  start = cp;

  while (!(*cp == '\r' || *cp == '\n') && *cp != '\0')
    cp++;

  strlen = cp - start;
  token = XMALLOC (MTYPE_CMD_TOKENS, strlen + 1);
  memcpy (token, start, strlen);
  *(token + strlen) = '\0';

  state->dp = cp;

  return token;
}

static void
format_parser_begin_keyword(struct format_parser_state *state)
{
  struct cmd_token *token;
  vector keyword_vect;

  if (state->in_keyword
      || state->in_multiple)
    format_parser_error(state, "Unexpected '{'");

  state->cp++;
  state->in_keyword = 1;

  token = XCALLOC(MTYPE_CMD_TOKENS, sizeof(*token));
  token->type = TOKEN_KEYWORD;
  token->keyword = vector_init(VECTOR_MIN_SIZE);

  keyword_vect = vector_init(VECTOR_MIN_SIZE);
  vector_set(token->keyword, keyword_vect);

  vector_set(state->curvect, token);
  state->curvect = keyword_vect;
}

static void
format_parser_begin_multiple(struct format_parser_state *state)
{
  struct cmd_token *token;

  if (state->in_keyword == 1)
    format_parser_error(state, "Keyword starting with '('");

  if (state->in_multiple)
    format_parser_error(state, "Nested group");

  state->cp++;
  state->in_multiple = 1;
  state->just_read_word = 0;

  token = XCALLOC(MTYPE_CMD_TOKENS, sizeof(*token));
  token->type = TOKEN_MULTIPLE;
  token->multiple = vector_init(VECTOR_MIN_SIZE);

  vector_set(state->curvect, token);
  if (state->curvect != state->topvect)
    state->intvect = state->curvect;
  state->curvect = token->multiple;
}

static void
format_parser_handle_brackets(struct format_parser_state *state)
{
  struct cmd_token *token;
  //vector brackets_vect;

  if (state->in_multiple)
    format_parser_error(state, "Unexpected '['");

	if (state->in_keyword == 1)
    format_parser_error(state, "keyword starting with '('");

  state->cp++;
  state->in_brackets = 1;

  token = XCALLOC(MTYPE_CMD_TOKENS, sizeof(*token));
  token->type = TOKEN_BRACKETS;
  token->brackets = vector_init(VECTOR_MIN_SIZE);

//  vector_set(token->brackets, token);
	vector_set(state->curvect, token);
	if (state->curvect != state->topvect)
		state->intvect = state->curvect;
//  brackets_vect = vector_init(VECTOR_MIN_SIZE);
  state->curvect = token->brackets;
}

static void
format_parser_end_brackets(struct format_parser_state *state)
{
  if (state->in_multiple)
    format_parser_error(state, "Unexpected ']'");

  state->cp++;
  state->in_brackets = 0;

	if (state->intvect)
    state->curvect = state->intvect;
  else
    state->curvect = state->topvect;
}

static void
format_parser_end_keyword(struct format_parser_state *state)
{
  if (state->in_multiple
      || !state->in_keyword)
    format_parser_error(state, "Unexpected '}'");

  if (state->in_keyword == 1)
    format_parser_error(state, "Empty keyword group");

  state->cp++;
  state->in_keyword = 0;
  state->curvect = state->topvect;
}

static void
format_parser_end_multiple(struct format_parser_state *state)
{
  char *dummy;

  if (!state->in_multiple)
    format_parser_error(state, "Unepexted ')'");

  if (vector_active(state->curvect) == 0)
    format_parser_error(state, "Empty multiple section");

  if (!state->just_read_word)
    {
      /* There are constructions like
       * 'show ip ospf database ... (self-originate|)'
       * in use.
       * The old parser reads a description string for the
       * word '' between |) which will never match.
       * Simulate this behvaior by dropping the next desc
       * string in such a case. */

      dummy = format_parser_desc_str(state);
      XFREE(MTYPE_CMD_TOKENS, dummy);
    }

  state->cp++;
  state->in_multiple = 0;

  if (state->intvect)
    state->curvect = state->intvect;
  else
    state->curvect = state->topvect;
}

static void
format_parser_handle_pipe(struct format_parser_state *state)
{
  struct cmd_token *keyword_token;
  vector keyword_vect;

  if (state->in_multiple)
    {
      state->just_read_word = 0;
      state->cp++;
    }
  else if (state->in_keyword)
    {
      state->in_keyword = 1;
      state->cp++;

/*get the topvect of '{' token whith '|' piple is in */
      keyword_token = vector_slot(state->topvect,
                                  vector_active(state->topvect) - 1);
      keyword_vect = vector_init(VECTOR_MIN_SIZE);
      vector_set(keyword_token->keyword, keyword_vect);
      state->curvect = keyword_vect;
    }
  else
    {
      format_parser_error(state, "Unexpected '|'");
    }
}

static void
format_parser_read_word(struct format_parser_state *state)
{
  const char *start;
  int len;
  char *cmd;
  struct cmd_token *token;

  start = state->cp;

  while (state->cp[0] != '\0'
         && !strchr("\r\n(){}[]|", state->cp[0])
         && !isspace((int)state->cp[0]))
    state->cp++;

  len = state->cp - start;
  cmd = XMALLOC(MTYPE_CMD_TOKENS, len + 1);
  memcpy(cmd, start, len);
  cmd[len] = '\0';

  token = XCALLOC(MTYPE_CMD_TOKENS, sizeof(*token));
  token->type = TOKEN_TERMINAL;
  if (strcmp (cmd, "A.B.C.D") == 0)
    token->terminal = TERMINAL_IPV4;
  else if (strcmp (cmd, "A.B.C.D/M") == 0)
    token->terminal = TERMINAL_IPV4_PREFIX;
  else if (strcmp (cmd, "X:X::X:X") == 0)
    token->terminal = TERMINAL_IPV6;
  else if (strcmp (cmd, "X:X::X:X/M") == 0)
    token->terminal = TERMINAL_IPV6_PREFIX;
  else if (cmd[0] == '[')
    token->terminal = TERMINAL_OPTION;
  else if (cmd[0] == '.')
    token->terminal = TERMINAL_VARARG;
  else if (cmd[0] == '<')
    token->terminal = TERMINAL_RANGE;
  else if (cmd[0] >= 'A' && cmd[0] <= 'Z')
  	{
  		if(strcmp (cmd, "XX:XX:XX:XX:XX:XX") == 0)
				token->terminal = TERMINAL_MAC;
			else if(strcmp (cmd, "USP") == 0)
				token->terminal = TERMINAL_USP;
			else if(strcmp (cmd, "NAME") == 0)
				token->terminal = TERMINAL_NAME;
		  else
 		    token->terminal = TERMINAL_VARIABLE;
  	}
  else
    token->terminal = TERMINAL_LITERAL;

  token->cmd = cmd;
  token->desc = format_parser_desc_str(state);
  vector_set(state->curvect, token);

  if (state->in_keyword == 1)
    state->in_keyword = 2;

  state->just_read_word = 1;
}

/**
 * Parse a given command format string and build a tree of tokens from
 * it that is suitable to be used by the command subsystem.
 *
 * @param string Command format string.
 * @param descstr Description string.
 * @return A vector of struct cmd_token representing the given command,
 *         or NULL on error.
 */
static vector
cmd_parse_format(const char *string, const char *descstr)
{
  struct format_parser_state state;

  if (string == NULL)
    return NULL;

  memset(&state, 0, sizeof(state));
  state.topvect = state.curvect = vector_init(VECTOR_MIN_SIZE);
  state.cp = state.string = string;
  state.dp = descstr;

  while (1)
    {
      while (isspace((int)state.cp[0]) && state.cp[0] != '\0')
        state.cp++;

      switch (state.cp[0])
        {
        case '\0':
          if (state.in_keyword
              || state.in_multiple)
            format_parser_error(&state, "Unclosed group/keyword");
          return state.topvect;
        case '{':
          format_parser_begin_keyword(&state);
          break;
        case '(':
          format_parser_begin_multiple(&state);
          break;
        case '}':
          format_parser_end_keyword(&state);
          break;
        case ')':
          format_parser_end_multiple(&state);
          break;
        case '|':
          format_parser_handle_pipe(&state);
          break;
				case '[':
					format_parser_handle_brackets(&state);
					break;
				case ']':
					format_parser_end_brackets(&state);
					break;
        default:
          format_parser_read_word(&state);
        }
    }
}

/* Return prompt character of specified node. */
char *
cmd_prompt (enum node_type node)
{
  struct cmd_node *cnode;

  if(cmdvec->active < node)
  {
	  zlog_info ( "%s[%d] Command node %d cmdvec->active == %d, may erro ???please check it\n", __FUNCTION__, __LINE__, node,
	  cmdvec->active );

	return NULL;
  }
  cnode = vector_slot (cmdvec, node);
  if(cnode == NULL)
  {
	zlog_info ( "%s[%d] Command node %d cnode == NULL, may erro ???please check it\n", __FUNCTION__, __LINE__, node );
  	return NULL;
  }
  return cnode->prompt;
}

/* Install a command into a node. */
void
install_element_daemon_order_level (enum node_type ntype, struct cmd_element *cmd,
	enum cmd_privilege_level level, enum sync_type sync_flag, int deamon_num, ...)
{
	va_list args;
	int deamon_index = 0;
	int *order_index = NULL;
	long long deamon_flag = 0;
	cmd->order = list_new ();

	va_start(args, deamon_num);
	for(int i = 0; i < deamon_num; i++)
	{
		deamon_flag = va_arg(args, long long);
		VTYSH_DAEMON_INDEX(deamon_flag, deamon_index);

		order_index = (int *)malloc(sizeof(int));		
		*order_index = deamon_index;
		listnode_add (cmd->order, order_index);
			deamon_index = 0;
	}

	install_element_level (ntype, cmd, level, sync_flag);
	va_end(args);
}

/* Install a command into a node. */
void
install_element_daemon_order (enum node_type ntype, struct cmd_element *cmd, enum sync_type sync_flag, int deamon_num, ...)
{
	va_list args;
	int deamon_index = 0;
	int *order_index = NULL;
	long long deamon_flag = 0;
	cmd->order = list_new ();

	va_start(args, deamon_num);
	for(int i = 0; i < deamon_num; i++)
	{
		deamon_flag = va_arg(args, long long);
		VTYSH_DAEMON_INDEX(deamon_flag, deamon_index);

		order_index = (int *)malloc(sizeof(int));
		*order_index = deamon_index;
		listnode_add (cmd->order, order_index);
			deamon_index = 0;
	}

	install_element (ntype, cmd, sync_flag);
	va_end(args);
}


/* Install a command into a node. */
void
install_element (enum node_type ntype, struct cmd_element *cmd, enum sync_type sync_flag)
{
  struct cmd_node *cnode;

  /* cmd_init hasn't been called */
  if (!cmdvec)
    return;

  cnode = vector_slot (cmdvec, ntype);
//fprintf(stdout, "nodetype = %d\n", ntype);
//fprintf(stdout, "%s\n", cmd->string);
/*
if(ntype == DEBUG_NODE)
{
fprintf(stdout, "DEBUG_NODE\n");
fprintf(stdout, "%s\n", cmd->string);
}
*/
  if (cnode == NULL)
    {
      fprintf (stderr, "Command node %d doesn't exist, please check it\n",
	       ntype);
      exit (1);
    }

  cmd->sync = sync_flag;
  vector_set (cnode->cmd_vector, cmd);
  if (cmd->tokens == NULL)
    cmd->tokens = cmd_parse_format(cmd->string, cmd->doc);
}

/* Install a command into a node. */
void
install_element_level (enum node_type ntype, struct cmd_element *cmd, enum cmd_privilege_level level, enum sync_type sync_flag)
{
	struct cmd_node *cnode;

	/* cmd_init hasn't been called */
	if (!cmdvec)
	return;

	cnode = vector_slot (cmdvec, ntype);

	if(level > ROOT_LEVE)
	{
		fprintf(stderr, "Command:'%s' has wrong level, use default\n", cmd->string);
		cmd->level = 0;
	}
	else
		cmd->level = level;

	if (cnode == NULL)
	{
	  fprintf (stderr, "Command node %d doesn't exist, please check it\n",
	       ntype);
	  exit (1);
	}

	cmd->sync = sync_flag;

	if(pthread_mutex_init(&cmd->daemon_lock, NULL) != 0)
	{
		perror("daemon_lock init failed\n");
		exit(1);
	}
	
	vector_set (cnode->cmd_vector, cmd);
	if (cmd->tokens == NULL)
	cmd->tokens = cmd_parse_format(cmd->string, cmd->doc);
}

static const unsigned char itoa64[] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void
to64(char *s, long v, int n)
{
  while (--n >= 0)
    {
      *s++ = itoa64[v&0x3f];
      v >>= 6;
    }
}

char *
zencrypt (const char *passwd)
{
  char salt[6];
  struct timeval tv;
  char *crypt (const char *, const char *);

  gettimeofday(&tv,0);

  to64(&salt[0], random(), 3);
  to64(&salt[3], tv.tv_usec, 3);
  salt[5] = '\0';

  return crypt (passwd, salt);
}

/* Utility function for getting command vector. */
static vector
cmd_node_vector (vector v, enum node_type ntype)
{
  struct cmd_node *cnode = vector_slot (v, ntype);
  return cnode->cmd_vector;
}

/* Completion match types. */
enum match_type
{
  no_match,
  extend_match,
  optional_match,
  ipv4_prefix_match,
  ipv4_match,
  ipv6_prefix_match,
  ipv6_match,
  mac_match,
  ifname_match,
  range_match,
  vararg_match,
  partly_match,
  exact_match
};

static enum match_type
cmd_ipv4_match (const char *str)
{
  const char *sp;
  int dots = 0, nums = 0, section_num = 0;
  char buf[4];

  if (str == NULL)
    return partly_match;

  for (;;)
  {
    memset (buf, 0, sizeof (buf));
    sp = str;  //sp always at begin of current number
    while (! DELIMITER (*str))
		{
		  if (*str == '.')
	    {
	      if (dots >= 3)  //too many dots
					return no_match;

	      if (*(str + 1) == '.') //double dots
					return no_match;

	      if (*(str + 1) == '\0') //dots is at end of string
					return no_match;
//		return partly_match;

	      dots++;
	      break;
	    }
		  if (!isdigit ((int) *str))
		    return no_match;

			section_num++;
		  str++;
		}

    if (str - sp > 3)  //check length of num
			return no_match;
		else if (str - sp == 0) //dot at the beginning or empty string
			return no_match;

    strncpy (buf, sp, str - sp);
    if (atoi (buf) > 255)
			return no_match;

      nums++;

    if (*str == '\0')
			break;

		if (DELIMITER (*str))
		{
      break;
    }

      str++; //skip the dot
   }

  if (nums < 4)
		return no_match;
//    return partly_match;

  return exact_match;
}

static enum match_type
cmd_ipv4_prefix_match (const char *str)
{
  const char *sp;
  int dots = 0;
  char buf[4];

  if (str == NULL)
    return partly_match;

  for (;;)
    {
      memset (buf, 0, sizeof (buf));
      sp = str;
      while (*str != '\0' && *str != '/')
	{
	  if (*str == '.')
	    {
	      if (dots == 3)
		return no_match;

	      if (*(str + 1) == '.' || *(str + 1) == '/')
		return no_match;

	      if (*(str + 1) == '\0')
		return no_match;

	      dots++;
	      break;
	    }

	  if (!isdigit ((int) *str))
	    return no_match;

	  str++;
	}

      if (str - sp > 3)
	return no_match;

      strncpy (buf, sp, str - sp);
      if (atoi (buf) > 255)
	return no_match;

      if (dots == 3)
	{
	  if (*str == '/')
	    {
	      if (*(str + 1) == '\0')
		return no_match;

	      str++;
	      break;
	    }
	  else if (*str == '\0')
	    return no_match;
	}

      if (*str == '\0')
	return no_match;

      str++;
    }

  sp = str;
  while (*str != '\0')
    {
      if (!isdigit ((int) *str))
	return no_match;

      str++;
    }

  if (atoi (sp) > 32)
    return no_match;

  return exact_match;
}

#define IPV6_ADDR_STR		"0123456789abcdefABCDEF:.%"
#define IPV6_PREFIX_STR		"0123456789abcdefABCDEF:.%/"
#define STATE_START		1
#define STATE_COLON		2
#define STATE_DOUBLE		3
#define STATE_ADDR		4
#define STATE_DOT               5
#define STATE_SLASH		6
#define STATE_MASK		7

#ifdef HAVE_IPV6

static enum match_type
cmd_ipv6_match (const char *str)
{
  struct sockaddr_in6 sin6_dummy;
  int ret;

  if (str == NULL)
    return partly_match;

  if (strspn (str, IPV6_ADDR_STR) != strlen (str))
    return no_match;

  /* use inet_pton that has a better support,
   * for example inet_pton can support the automatic addresses:
   *  ::1.2.3.4
   */
  ret = inet_pton(AF_INET6, str, &sin6_dummy.sin6_addr);

  if (ret == 1)
    return exact_match;

  return no_match;
}

static enum match_type
cmd_ipv6_prefix_match (const char *str)
{
  int state = STATE_START;
  int colons = 0, nums = 0, double_colon = 0;
  int mask;
  const char *sp = NULL;
  char *endptr = NULL;

  if (str == NULL)
    return partly_match;

  if (strspn (str, IPV6_PREFIX_STR) != strlen (str))
    return no_match;

  while (*str != '\0' && state != STATE_MASK)
    {
      switch (state)
	{
	case STATE_START:
	  if (*str == ':')
	    {
	      if (*(str + 1) != ':' && *(str + 1) != '\0')
		return no_match;
	      colons--;
	      state = STATE_COLON;
	    }
	  else
	    {
	      sp = str;
	      state = STATE_ADDR;
	    }

	  continue;
	case STATE_COLON:
	  colons++;
	  if (*(str + 1) == '/')
	    return no_match;
	  else if (*(str + 1) == ':')
	    state = STATE_DOUBLE;
	  else
	    {
	      sp = str + 1;
	      state = STATE_ADDR;
	    }
	  break;
	case STATE_DOUBLE:
	  if (double_colon)
	    return no_match;

	  if (*(str + 1) == ':')
	    return no_match;
	  else
	    {
	      if (*(str + 1) != '\0' && *(str + 1) != '/')
		colons++;
	      sp = str + 1;

	      if (*(str + 1) == '/')
		state = STATE_SLASH;
	      else
		state = STATE_ADDR;
	    }

	  double_colon++;
	  nums += 1;
	  break;
	case STATE_ADDR:
	  if (*(str + 1) == ':' || *(str + 1) == '.'
	      || *(str + 1) == '\0' || *(str + 1) == '/')
	    {
	      if (str - sp > 3)
		return no_match;

	      for (; sp <= str; sp++)
		if (*sp == '/')
		  return no_match;

	      nums++;

	      if (*(str + 1) == ':')
		state = STATE_COLON;
	      else if (*(str + 1) == '.')
		{
		  if (colons || double_colon)
		    state = STATE_DOT;
		  else
		    return no_match;
		}
	      else if (*(str + 1) == '/')
		state = STATE_SLASH;
	    }
	  break;
	case STATE_DOT:
	  state = STATE_ADDR;
	  break;
	case STATE_SLASH:
	  if (*(str + 1) == '\0')
	    return partly_match;

	  state = STATE_MASK;
	  break;
	default:
	  break;
	}

      if (nums > 11)
	return no_match;

      if (colons > 7)
	return no_match;

      str++;
    }

  if (state < STATE_MASK)
    return partly_match;

  mask = strtol (str, &endptr, 10);
  if (*endptr != '\0')
    return no_match;

  if (mask < 0 || mask > 128)
    return no_match;

/* I don't know why mask < 13 makes command match partly.
   Forgive me to make this comments. I Want to set static default route
   because of lack of function to originate default in ospf6d; sorry
       yasu
  if (mask < 13)
    return partly_match;
*/

  return exact_match;
}

#endif /* HAVE_IPV6  */

static
int is_hex_num(const char* pBuf, int bufSize)
{
    int i = 0;

    if ((NULL == pBuf) || (0 == bufSize))
    {
        return 0;
    }

    for (i = 0; i < bufSize; ++i)
    {
        if (('0' > pBuf[i] || '9' < pBuf[i])
            && ('A' > pBuf[i] || 'F' < pBuf[i])
            && ('a' > pBuf[i] || 'f' < pBuf[i]))
        {
            return 0;
        }
    }

    return 1;
}

static enum match_type
cmd_mac_match (const char *str)
{
  const char *sp = NULL;
  int dots = 0;
  int nums = 0;

	if(str == NULL)
		return partly_match;

  sp = str;

  /* Loop until we see delimiter.  */
  while (! DELIMITER (*str))
    {
      if (*str == ':')
        {
          if (nums == 0)
            return no_match;

          if (dots >= 5)
            return no_match;

          if (*(str + 1) == ':')
            return no_match;

          dots++;
          str++;
          sp = str;
        }
      else
        {
          /* If the character is not alnum, return.  */
          if (! isalnum ((int) *str))
            return no_match;

					if (!is_hex_num(str, 1))
					{
						return no_match;
					}


          /* If the character orrur more than two, it it error.  */
          if (str - sp >= 2)
            return no_match;

          /* Increment the character pointer.  */
          nums++;
          str++;
        }
    }

  /* Matched, check this is exact match or not.  */
  if (dots != 5 || nums != 12)
    return no_match;

  return exact_match;

}

static enum match_type
cmd_ifname_match (const char *str)
{
	const char *sp;
	int dots = 0;
	int nums = 0;
	int dividers = 0;
	char buf[5];
	int data_tmp;

	if(str == NULL)
		return partly_match;

	for(; ;)
	{
		sp = str;
		while (! DELIMITER (*str))
		{
				if (*str == '/')
				{
					if(dividers > 2)
						return no_match;

					dividers++;
					break;
				}

				//first two part must be digits, the last part may have a dot '.' means sub ifname index
				if(dividers < 2)
					if(!isdigit ((int) *str))
						return no_match;

				if(dividers == 2)
					if(!isdigit ((int) *str) && *str != '.')
						return no_match;

				if(dividers == 2 && *str == '.')
				{
					dots ++;
					break;
				}

				if(dots > 1)  //more than one dots
					return no_match;

				str++;
		}
//	if (dots == 1 && DELIMITER (*str)) //sub ifname index after the dot, part between dot and end of the string

		nums++;
		if(str - sp == 0)
			return no_match;

		if(str - sp > 4)
			return no_match;

		memset(buf, 0, sizeof(buf));
		strncpy(buf, sp , str - sp);
		data_tmp = atoi(buf);

		switch (nums)
		{
			case 1:  //first part
				if(data_tmp > 7)
					return no_match;
				else
					break;

			case 2:  //second part
				if(data_tmp > 31)
					return no_match;
				else
					break;

			case 3:  //third part
//				if(dots == 0 || (dots == 1 && *str == '.')) //part between '/' and '.'
				if(data_tmp > 2000 || data_tmp == 0)
					return no_match;
				else
					break;

			case 4:
//				else if(dots == 1 && DELIMITER (*str)) //part between '.' and end line
				if(data_tmp > 4095 || data_tmp == 0)
					return no_match;
				else
					break;

			default:
				break;
		}

    if (DELIMITER (*str))
 		 break;

		str++; //skip '\'
	}

	if(nums < 3)
		return no_match;

	return exact_match;
}


#define DECIMAL_STRLEN_MAX 10
/*
支持几种模式
<1-10>
<10>
<-10-10>  目前只有负数到正数的情况
*/
static int
cmd_range_match (const char *range, const char *str)
{
	char *p;
	char buf[DECIMAL_STRLEN_MAX + 1];
	char *endptr = NULL;
	int64_t min, max, val;
    int negval = 0;

    if(str == NULL)
    {
        return 1;
    }

    if((str[0] == '0') && (strlen(str) >= 2))
    {
		return 0;
    }

	if(str[0] == '-')//是负数
	{
		if((str[1] == '0') || ((strlen(str) < 2)))
	    {
			return 0;
	    }
		str++;
		negval = 1;
	}
    val = (int64_t)strtoull(str, &endptr, 10);
    if(*endptr != '\0')
    {
        return 0;
    }
    if(negval == 1)
    {
		val = 0 - val;
    }

	negval = 0;
    range++;
    if(range[0] == '-')
    {
		negval = 1;
		range++;
    }
    p = strchr(range, '-');   //rang:nn-mm>  p:-mm>

    if(p == NULL)
    {		//<单个数> 时没有负数的情况
    	min = (int64_t)strtoull(range, &endptr, 10);
    	if(val != min)
    	{
			return 0;
    	}
    	else
    	{
			return 1;
    	}
    }

    if(p - range > DECIMAL_STRLEN_MAX)
    {
        return 0;
    }

    strncpy(buf, range, p - range); // buf:nn
    buf[p - range] = '\0';
    min = (int64_t)strtoull(buf, NULL, 10); //min:
    if(*endptr != '\0')
    {
        return 0;
    }
    if(negval == 1)
    {
		min = 0 - min;
    }
    range = p + 1;   // rang:mm>
    p = strchr(range, '>');// p:>

    if(p == NULL)
    {
        return 0;
    }

    if(p - range > DECIMAL_STRLEN_MAX)
    {
        return 0;
    }

    strncpy(buf, range, p - range); //buf:mm
    buf[p - range] = '\0';
    max = (int64_t)strtoull(buf, &endptr, 10);
    if(*endptr != '\0')
    {
        return 0;
    }

    if(val < min || val > max)
    {
        return 0;
    }

  return 1;
}

static enum match_type
cmd_word_match(struct cmd_token *token,
               enum filter_type filter,
               const char *word)
{
  const char *str;
  enum match_type match_type;

  str = token->cmd;

  if (filter == FILTER_RELAXED)
    if (!word || !strlen(word))
    	{
      return partly_match;
    	}

  if (!word)
    return no_match;

  switch (token->terminal)
    {
      case TERMINAL_VARARG:
        return vararg_match;

      case TERMINAL_RANGE:
        if (cmd_range_match(str, word))
          return range_match;
		else
		{
			validity_check.range_check_faile = 1;
			validity_check.match_fail_cnt++;
		}
        break;
#ifdef HAVE_IPV6
      case TERMINAL_IPV6:
        match_type = cmd_ipv6_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
          || (filter == FILTER_STRICT && match_type == exact_match))
          return ipv6_match;
        break;

      case TERMINAL_IPV6_PREFIX:
        match_type = cmd_ipv6_prefix_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
            || (filter == FILTER_STRICT && match_type == exact_match))
          return ipv6_prefix_match;
        break;
#endif
      case TERMINAL_IPV4:
        match_type = cmd_ipv4_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
            || (filter == FILTER_STRICT && match_type == exact_match))
          return ipv4_match;
				else
				{
					validity_check.ipv4_check_faile = 1;
					validity_check.match_fail_cnt++;
				}
        break;

      case TERMINAL_IPV4_PREFIX:
        match_type = cmd_ipv4_prefix_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
            || (filter == FILTER_STRICT && match_type == exact_match))
          return ipv4_prefix_match;
				else
				{
					validity_check.ipv4_prefix_check_faile = 1;
					validity_check.match_fail_cnt++;
				}
        break;

			case TERMINAL_MAC:
				match_type = cmd_mac_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
            || (filter == FILTER_STRICT && match_type == exact_match))
          return mac_match;
				else
				{
					validity_check.mac_check_faile = 1;
					validity_check.match_fail_cnt++;
				}
        break;

			case TERMINAL_USP:
				match_type = cmd_ifname_match(word);
        if ((filter == FILTER_RELAXED && match_type != no_match)
            || (filter == FILTER_STRICT && match_type == exact_match))
            return ifname_match;
        else
        {
					validity_check.ifname_check_faile = 1;
					validity_check.match_fail_cnt++;
        }
				break;

//      case TERMINAL_OPTION:
//				match_type = cmd_optional_match(word);
//        return optional_match;

      case TERMINAL_VARIABLE:
				if(strlen(word) < STRING_LEN)
					return extend_match;
				else
				{
					validity_check.string_check_faile = 1;
					validity_check.match_fail_cnt++;
				}
				break;

			case TERMINAL_NAME:
				if(strlen(word) < NAME_STRING_LEN)
					return extend_match;
				else
				{
					validity_check.name_check_faile = 1;
					validity_check.match_fail_cnt++;
				}
				break;

			case TERMINAL_OPTION:
				return extend_match;

      case TERMINAL_LITERAL:
        if (filter == FILTER_RELAXED && !strncmp(str, word, strlen(word)))
          {
            if (!strcmp(str, word))
              return exact_match;
            return partly_match;
          }
        if (filter == FILTER_STRICT && !strcmp(str, word))
          return exact_match;
        break;

      default:
        assert (0);
    }

  return no_match;
}

struct cmd_matcher
{
  struct cmd_element *cmd; /* The command element the matcher is using */
  enum filter_type filter; /* Whether to use strict or relaxed matching */
  vector vline; /* The tokenized commandline which is to be matched */
  unsigned int index; /* The index up to which matching should be done */

  /* If set, construct a list of matches at the position given by index */
  enum match_type *match_type;
  vector *match;

  unsigned int word_index; /* iterating over vline */
};

static int
push_argument(int *argc, const char **argv, const char *arg)
{
  if (!arg || !strlen(arg))
    arg = NULL;

  if (!argc || !argv)
    return 0;

  if (*argc >= CMD_ARGC_MAX)
    return -1;

  argv[(*argc)++] = arg;
  return 0;
}

static void
cmd_matcher_record_match(struct cmd_matcher *matcher,
                         enum match_type match_type,
                         struct cmd_token *token)
{
  if (matcher->word_index != matcher->index)
    return;
  if (matcher->match)
    {
      if (!*matcher->match)
        *matcher->match = vector_init(VECTOR_MIN_SIZE);
      vector_set(*matcher->match, token);
    }

  if (matcher->match_type)
    {
      if (match_type > *matcher->match_type)
        *matcher->match_type = match_type;
    }
}

static int
cmd_matcher_words_left(struct cmd_matcher *matcher)
{
  return matcher->word_index < vector_active(matcher->vline);
}

static const char*
cmd_matcher_get_word(struct cmd_matcher *matcher)
{
  assert(cmd_matcher_words_left(matcher));

  return vector_slot(matcher->vline, matcher->word_index);
}

static enum matcher_rv
cmd_matcher_match_brackets(struct cmd_matcher *matcher,
                           struct cmd_token *token,
                           int *argc, const char **argv,
														struct cmd_token *token_next)
	{
		//unsigned int i;
		//unsigned int brackets_index;
		enum match_type brackets_match;
		enum match_type word_match;
		//vector brackets_vector;
		struct cmd_token *word_token;
		const char *word;
		//int keyword_argc;
		//const char **keyword_argv;
		//const char *arg = NULL;
		//enum matcher_rv rv = MATCHER_NO_MATCH;

		if (!cmd_matcher_words_left(matcher))
		{
			if (push_argument(argc, argv, NULL))
				return MATCHER_EXCEED_ARGC_MAX;

			return MATCHER_OK;
		}

			//get word in vline of current word_index
		word = cmd_matcher_get_word(matcher);

		brackets_match = no_match;

		word_token = vector_slot(token->brackets, 0);

		word_match = cmd_word_match(word_token, matcher->filter, word);

		if (word_match == no_match)
		{
			if((vector_active(token->brackets) == 1) && (token_next != NULL))
			{
				if (push_argument(argc, argv, NULL))
					return MATCHER_EXCEED_ARGC_MAX;
			}

			//中括号没在命令行尾的情况下
			if(token_next != NULL)
				{
				return MATCHER_OPTIONAL_UNMATCH;
				}
			else
	  	{
				return MATCHER_NO_MATCH;
	  	}
		}

			/*记录最后一个匹配，用于命令行联想(输入问号最后一个字符是null，用来匹配记录下一个token)*/
			 cmd_matcher_record_match(matcher, word_match, word_token);

			if(matcher->match
	  	&& matcher->filter == FILTER_RELAXED
	  	&& matcher->word_index == matcher->index
	  	&& token_next)
		  {
			  if (!word || !strlen(word))
			  	{
				  vector_set(*matcher->match, token_next);
			  	}
		  }

			if(vector_active(token->brackets) == 1)
			{
			//填写参数,如果中括号内只有关键字，没有参数，则将arg = null填入参数
				 if (TERMINAL_RECORD (word_token->terminal))
				 {
					 if (push_argument(argc, argv, word))
						 return MATCHER_EXCEED_ARGC_MAX;
				 }
				 if (word_token->terminal == TERMINAL_LITERAL)
				 {
				 	 if (push_argument(argc, argv, word_token->cmd))
						 return MATCHER_EXCEED_ARGC_MAX;
//					 if (push_argument(argc, argv, NULL))
//						 return MATCHER_EXCEED_ARGC_MAX;
				 }
			}

			if (word_match > brackets_match)
      {
     	  brackets_match = word_match;
        //arg = word;
				//无论哪种情况，参数肯定是最后一个token
      }

			matcher->word_index++;

      if (matcher->word_index > matcher->index
					|| (vector_active(token->brackets) == 1))
        return MATCHER_OK;

			/******处理下一个token******/

			if (!cmd_matcher_words_left(matcher))
				return MATCHER_INCOMPLETE;

			brackets_match = no_match;

			word_token = vector_slot(token->brackets, 1);

			word = cmd_matcher_get_word(matcher);

			word_match = cmd_word_match(word_token, matcher->filter, word);

			if (word_match == no_match)
				return MATCHER_NO_MATCH;

		/*记录最后一个匹配，用于命令行联想(输入问号最后一个字符是null，用来匹配记录下一个token)*/
		 cmd_matcher_record_match(matcher, word_match, word_token);

#if 0
			/*第二个token肯帝是中括号内最后的token，中括号后面的一个token也要放入matcher->match用于联想*/
			if(matcher->match
		  	&& matcher->filter == FILTER_RELAXED
		  	&& matcher->word_index == matcher->index
		  	&& token_next)
		  {
		  printf("matcher->index in brackets:%d\n",matcher->index);
			  if (!word || !strlen(word))
			  	{
				  vector_set(*matcher->match, token_next);
			  	}
		  }
#endif

			//最后一个token肯定是参数
		if (TERMINAL_RECORD (word_token->terminal))
		{
		 if (push_argument(argc, argv, word))
			 return MATCHER_EXCEED_ARGC_MAX;
		}

		if (word_match > brackets_match)
			  brackets_match = word_match;

		 matcher->word_index++;

		 return MATCHER_OK;
}


static enum matcher_rv
cmd_matcher_match_terminal(struct cmd_matcher *matcher,
                           struct cmd_token *token,
                           int *argc, const char **argv)
{
  const char *word;
  enum match_type word_match;

  assert(token->type == TOKEN_TERMINAL);

  if (!cmd_matcher_words_left(matcher))
    {
      if (token->terminal == TERMINAL_OPTION)
        return MATCHER_OK; /* missing optional args are NOT pushed as NULL */
      else
        return MATCHER_INCOMPLETE;
    }

  word = cmd_matcher_get_word(matcher);
  word_match = cmd_word_match(token, matcher->filter, word);
  if (word_match == no_match)
    return MATCHER_NO_MATCH;

  /* We have to record the input word as argument if it matched
   * against a variable. */
  if (TERMINAL_RECORD (token->terminal))
    {
      if (push_argument(argc, argv, word))
        return MATCHER_EXCEED_ARGC_MAX;
    }

  cmd_matcher_record_match(matcher, word_match, token);

  matcher->word_index++;

  /* A vararg token should consume all left over words as arguments */
  if (token->terminal == TERMINAL_VARARG)
    while (cmd_matcher_words_left(matcher))
      {
        word = cmd_matcher_get_word(matcher);
        if (word && strlen(word))
          push_argument(argc, argv, word);
        matcher->word_index++;
      }

  return MATCHER_OK;
}

static enum matcher_rv
cmd_matcher_match_multiple(struct cmd_matcher *matcher,
                           struct cmd_token *token,
                           int *argc, const char **argv)
{
  enum match_type multiple_match;
  unsigned int multiple_index;
  const char *word;
  const char *arg = NULL;
  struct cmd_token *word_token;
  enum match_type word_match;
	struct cmd_token *best_token = NULL;
	int is_digit = 0;

  assert(token->type == TOKEN_MULTIPLE);


  multiple_match = no_match;

  if (!cmd_matcher_words_left(matcher))
    return MATCHER_INCOMPLETE;

  word = cmd_matcher_get_word(matcher);
  for (multiple_index = 0;
       multiple_index < vector_active(token->multiple);
       multiple_index++)
    {
      word_token = vector_slot(token->multiple, multiple_index);

      word_match = cmd_word_match(word_token, matcher->filter, word);
      if (word_match == no_match)
        continue;

      cmd_matcher_record_match(matcher, word_match, word_token);

      if (word_match > multiple_match)
        {
          multiple_match = word_match;
          arg = word;
					best_token = word_token;
        }
      /* To mimic the behavior of the old command implementation, we
       * tolerate any ambiguities here :/ */
    }

  matcher->word_index++;

  if (multiple_match == no_match)
    return MATCHER_NO_MATCH;

    if(word)
    {
    	is_digit = isdigit ((int) word[0]);
    }

    if(multiple_match == partly_match) //
    {
		if(!is_digit)
		{
	        if(push_argument(argc, argv, best_token->cmd))
	        {
	            return MATCHER_EXCEED_ARGC_MAX;
	        }
        }
        else
        {
			return MATCHER_NO_MATCH;
        }
    }
	else
	{
	  if (push_argument(argc, argv, arg))
	    return MATCHER_EXCEED_ARGC_MAX;
	}

  return MATCHER_OK;
}

static enum matcher_rv
cmd_matcher_read_keywords(struct cmd_matcher *matcher,
                          struct cmd_token *token,
                          vector args_vector,
                          struct cmd_token *token_next )
{
  unsigned int i;
  unsigned long keyword_mask;
  unsigned int keyword_found;
  enum match_type keyword_match;
  enum match_type word_match;
  vector keyword_vector;
  struct cmd_token *word_token;
  const char *word;
  int keyword_argc;
  const char **keyword_argv;
  enum matcher_rv rv = MATCHER_NO_MATCH;
  unsigned int keyword_token_counter = 0;

//  struct cmd_token *next_token = NULL;

  keyword_mask = 0;
  while (1)
    {
	  keyword_token_counter++;
	  if(keyword_token_counter > vector_active(token->keyword))
	  	return MATCHER_OK;

      if (!cmd_matcher_words_left(matcher))
      	{
        return MATCHER_OK;
      	}

      word = cmd_matcher_get_word(matcher);
      keyword_found = -1;
      keyword_match = no_match;
      for (i = 0; i < vector_active(token->keyword); i++)
        {
          if (keyword_mask & (1 << i))
            continue;

          keyword_vector = vector_slot(token->keyword, i);
          word_token = vector_slot(keyword_vector, 0);

          word_match = cmd_word_match(word_token, matcher->filter, word);
          if (word_match == no_match)
            continue;
          cmd_matcher_record_match(matcher, word_match, word_token);

          if (word_match > keyword_match)
            {
              keyword_match = word_match;
              keyword_found = i;
            }
          else if (word_match == keyword_match)
            {
              if (matcher->word_index != matcher->index || args_vector)
                return MATCHER_AMBIGUOUS;
            }
        }

	  if(matcher->match
	  	&& matcher->filter == FILTER_RELAXED
	  	&& matcher->word_index == matcher->index
	  	&& token_next)
	  {
	  if (!word || !strlen(word))
	  	{
		  vector_set(*matcher->match, token_next);
	  	}
	  }

      if (keyword_found == (unsigned int)-1)
      {
	      if(token_next != NULL)
	      	{
	     	return MATCHER_KEYWORD_UNMATCH;
	      	}
		  else
		  	{
				return MATCHER_NO_MATCH;
		  	}
	  }

      matcher->word_index++;

      if (matcher->word_index > matcher->index)
        return MATCHER_OK;


      keyword_mask |= (1 << keyword_found);

      if (args_vector)
        {
          keyword_argc = 0;
          keyword_argv = XMALLOC(MTYPE_TMP, (CMD_ARGC_MAX + 1) * sizeof(char*));
          /* We use -1 as a marker for unused fields as NULL might be a valid value */
          for (i = 0; i < CMD_ARGC_MAX + 1; i++)
            keyword_argv[i] = (void*)-1;
          vector_set_index(args_vector, keyword_found, keyword_argv);
        }
      else
        {
          keyword_argv = NULL;
        }

      keyword_vector = vector_slot(token->keyword, keyword_found);
      /* the keyword itself is at 0. We are only interested in the arguments,
       * so start counting at 1. */
      for (i = 1; i < vector_active(keyword_vector); i++)
        {
          word_token = vector_slot(keyword_vector, i);

          switch (word_token->type)
            {
            case TOKEN_TERMINAL:
              rv = cmd_matcher_match_terminal(matcher, word_token,
                                              &keyword_argc, keyword_argv);
              break;
            case TOKEN_MULTIPLE:
              rv = cmd_matcher_match_multiple(matcher, word_token,
                                              &keyword_argc, keyword_argv);
              break;
						case TOKEN_BRACKETS:
							for (uint32_t j = 0; j < vector_active(token->keyword); j++)
							{
								if( j == keyword_found)
									continue;

								struct cmd_token *record_token;
								vector record_vector;
								record_vector = vector_slot(token->keyword, j);
								record_token = vector_slot(record_vector, 0);

								if(vector_slot(matcher->vline, matcher->word_index) == 	NULL)
								{
								  if (matcher->word_index != matcher->index)
								    break;
								  if (!*matcher->match)
      					  	*matcher->match = vector_init(VECTOR_MIN_SIZE);
							 	 	vector_set(*matcher->match, record_token);
						  	}
							}
              rv = cmd_matcher_match_brackets(matcher, word_token,
                                              &keyword_argc, keyword_argv, token_next);
              break;
            case TOKEN_KEYWORD:
              assert(!"Keywords should never be nested.");
              break;
            }

          if (MATCHER_ERROR(rv))
            return rv;

          if (matcher->word_index > matcher->index)
            return MATCHER_OK;
        }
    }
  /* not reached */
}

static enum matcher_rv
cmd_matcher_build_keyword_args(struct cmd_matcher *matcher,
                               struct cmd_token *token,
                               int *argc, const char **argv,
                               vector keyword_args_vector)
{
  unsigned int i, j;
  const char **keyword_args;
  vector keyword_vector;
  struct cmd_token *word_token;
  const char *arg;
  enum matcher_rv rv;

  rv = MATCHER_OK;

  if (keyword_args_vector == NULL)
  	{
    return rv;
  	}

  for (i = 0; i < vector_active(token->keyword); i++)
    {
      keyword_vector = vector_slot(token->keyword, i);
      keyword_args = vector_lookup(keyword_args_vector, i);

      if (vector_active(keyword_vector) == 1)
        {
          /* this is a keyword without arguments */
          if (keyword_args)
            {
              word_token = vector_slot(keyword_vector, 0);
              arg = word_token->cmd;
            }
          else
            {
              arg = NULL;
            }

          if (push_argument(argc, argv, arg))
            rv = MATCHER_EXCEED_ARGC_MAX;
        }
      else
        {
          /* this is a keyword with arguments */
          if (keyword_args)
            {
              /* the keyword was present, so just fill in the arguments */
              for (j = 0; keyword_args[j] != (void*)-1; j++)
                if (push_argument(argc, argv, keyword_args[j]))
                  rv = MATCHER_EXCEED_ARGC_MAX;
              XFREE(MTYPE_TMP, keyword_args);
            }
          else
            {
              /* the keyword was not present, insert NULL for the arguments
               * the keyword would have taken. */
              for (j = 1; j < vector_active(keyword_vector); j++)
                {
                  word_token = vector_slot(keyword_vector, j);
                  if ((word_token->type == TOKEN_TERMINAL
                       && TERMINAL_RECORD (word_token->terminal))
                      || word_token->type == TOKEN_MULTIPLE)
                    {
                      if (push_argument(argc, argv, NULL))
                        rv = MATCHER_EXCEED_ARGC_MAX;
                    }
                }
            }
        }
    }
  vector_free(keyword_args_vector);
  return rv;
}

static enum matcher_rv
cmd_matcher_match_keyword(struct cmd_matcher *matcher,
                          struct cmd_token *token,
                          int *argc, const char **argv,
                          struct cmd_token *token_next )
{
  vector keyword_args_vector;
  enum matcher_rv reader_rv;
  enum matcher_rv builder_rv;

  assert(token->type == TOKEN_KEYWORD);

  if (argc && argv)
    keyword_args_vector = vector_init(VECTOR_MIN_SIZE);
  else
    keyword_args_vector = NULL;

  reader_rv = cmd_matcher_read_keywords(matcher, token, keyword_args_vector, token_next);
  builder_rv = cmd_matcher_build_keyword_args(matcher, token, argc,
                                              argv, keyword_args_vector);
  /* keyword_args_vector is consumed by cmd_matcher_build_keyword_args */

  if (!MATCHER_ERROR(reader_rv) && MATCHER_ERROR(builder_rv))
    return builder_rv;

  return reader_rv;
}

static void
cmd_matcher_init(struct cmd_matcher *matcher,
                 struct cmd_element *cmd,
                 enum filter_type filter,
                 vector vline,
                 unsigned int index,
                 enum match_type *match_type,
                 vector *match)
{
  memset(matcher, 0, sizeof(*matcher));

  matcher->cmd = cmd;
  matcher->filter = filter;
  matcher->vline = vline;
  matcher->index = index;

  matcher->match_type = match_type;
  if (matcher->match_type)
    *matcher->match_type = no_match;
  matcher->match = match;

  matcher->word_index = 0;
}

static enum matcher_rv
cmd_element_match(struct cmd_element *cmd_element,
                  enum filter_type filter,
                  vector vline,
                  unsigned int index,
                  enum match_type *match_type,
                  vector *match,
                  int *argc,
                  const char **argv)
{
  struct cmd_matcher matcher;
  unsigned int token_index;
  enum matcher_rv rv = MATCHER_NO_MATCH;

  cmd_matcher_init(&matcher, cmd_element, filter,
                   vline, index, match_type, match);

  if (argc != NULL)
    *argc = 0;

  for (token_index = 0;
       token_index < vector_active(cmd_element->tokens);
       token_index++)
    {
      struct cmd_token *token = vector_slot(cmd_element->tokens, token_index);
	  struct cmd_token *token_next = NULL;//vector_slot(cmd_element->tokens, token_index + 1);
	  if(token_index < vector_active(cmd_element->tokens) - 1)
	  	{
	  	token_next = vector_slot(cmd_element->tokens, token_index + 1);
	  	}
	  else
	  	{
	  	token_next = NULL;
	  	}

      switch (token->type)
        {
        case TOKEN_TERMINAL:
          rv = cmd_matcher_match_terminal(&matcher, token, argc, argv);
          break;
        case TOKEN_MULTIPLE:
          rv = cmd_matcher_match_multiple(&matcher, token, argc, argv);
          break;
        case TOKEN_KEYWORD:
          rv = cmd_matcher_match_keyword(&matcher, token, argc, argv, token_next);
					break;
				case TOKEN_BRACKETS:
					rv = cmd_matcher_match_brackets(&matcher, token, argc, argv, token_next);
					break;
        }

		if(rv == MATCHER_OK && validity_check.match_fail_cnt > 0)
		{
			memset (&validity_check, 0, sizeof(struct match_flag));
		}

		//if none keyword token match, continue match next word, because keyword is optional
	  if (rv == MATCHER_KEYWORD_UNMATCH || rv == MATCHER_OPTIONAL_UNMATCH)
	  	{
	  	continue;
	  	}

      if (MATCHER_ERROR(rv))
        return rv;

      if (matcher.word_index > index)
      	{
        return MATCHER_OK;
      	}
    }

  /* return MATCHER_COMPLETE if all words were consumed */
  if (matcher.word_index >= vector_active(vline))
    return MATCHER_COMPLETE;

  /* return MATCHER_COMPLETE also if only an empty word is left. */
  if (matcher.word_index == vector_active(vline) - 1
      && (!vector_slot(vline, matcher.word_index)
          || !strlen((char*)vector_slot(vline, matcher.word_index))))
    return MATCHER_COMPLETE;

  return MATCHER_NO_MATCH; /* command is too long to match */
}

/**
 * Filter a given vector of commands against a given commandline and
 * calculate possible completions.
 *
 * @param commands A vector of struct cmd_element*. Commands that don't
 *                 match against the given command line will be overwritten
 *                 with NULL in that vector.
 * @param filter Either FILTER_RELAXED or FILTER_STRICT. This basically
 *               determines how incomplete commands are handled, compare with
 *               cmd_word_match for details.
 * @param vline A vector of char* containing the tokenized commandline.
 * @param index Only match up to the given token of the commandline.
 * @param match_type Record the type of the best match here.
 * @param matches Record the matches here. For each cmd_element in the commands
 *                vector, a match vector will be created in the matches vector.
 *                That vector will contain all struct command_token* of the
 *                cmd_element which matched against the given vline at the given
 *                index.
 * @return A code specifying if an error occured. If all went right, it's
 *         CMD_SUCCESS.
 */
static int
cmd_vector_filter(vector commands,
                  enum filter_type filter,
                  vector vline,
                  unsigned int index,
                  enum match_type *match_type,
                  vector *matches)
{
  unsigned int i;
  struct cmd_element *cmd_element;
  enum match_type best_match;
  enum match_type element_match;
  enum matcher_rv matcher_rv;

  best_match = no_match;
  *matches = vector_init(VECTOR_MIN_SIZE);

  for (i = 0; i < vector_active (commands); i++)
    if ((cmd_element = vector_slot (commands, i)) != NULL)
      {
        vector_set_index(*matches, i, NULL);
        matcher_rv = cmd_element_match(cmd_element, filter,
                                       vline, index,
                                       &element_match,
                                       (vector*)&vector_slot(*matches, i),
                                       NULL, NULL);
        if (MATCHER_ERROR(matcher_rv))
          {
            vector_slot(commands, i) = NULL;
            if (matcher_rv == MATCHER_AMBIGUOUS)
              return CMD_ERR_AMBIGUOUS;
            if (matcher_rv == MATCHER_EXCEED_ARGC_MAX)
              return CMD_ERR_EXEED_ARGC_MAX;
          }
        else if (element_match > best_match)
          {
            best_match = element_match;
          }
      }
  *match_type = best_match;
  return CMD_SUCCESS;
}

/**
 * Check whether a given commandline is complete if used for a specific
 * cmd_element.
 *
 * @param cmd_element A cmd_element against which the commandline should be
 *                    checked.
 * @param vline The tokenized commandline.
 * @return 1 if the given commandline is complete, 0 otherwise.
 */
static int
cmd_is_complete(struct cmd_element *cmd_element,
                vector vline)
{
  enum matcher_rv rv;
  rv = cmd_element_match(cmd_element,
                         FILTER_RELAXED,
                         vline, -1,
                         NULL, NULL,
                         NULL, NULL);
  return (rv == MATCHER_COMPLETE);
}

/**
 * Parse a given commandline and construct a list of arguments for the
 * given command_element.
 *
 * @param cmd_element The cmd_element for which we want to construct arguments.
 * @param vline The tokenized commandline.
 * @param argc Where to store the argument count.
 * @param argv Where to store the argument list. Should be at least
 *             CMD_ARGC_MAX elements long.
 * @return CMD_SUCCESS if everything went alright, an error otherwise.
 */
static int
cmd_parse(struct cmd_element *cmd_element,
          vector vline,
          int *argc, const char **argv)
{
  enum matcher_rv rv = cmd_element_match(cmd_element,
                                         FILTER_RELAXED,
                                         vline, -1,
                                         NULL, NULL,
                                         argc, argv);

  switch (rv)
    {
    case MATCHER_COMPLETE:
      return CMD_SUCCESS;

    case MATCHER_NO_MATCH:
      return CMD_ERR_NO_MATCH;

    case MATCHER_AMBIGUOUS:
      return CMD_ERR_AMBIGUOUS;

    case MATCHER_EXCEED_ARGC_MAX:
      return CMD_ERR_EXEED_ARGC_MAX;

    default:
      return CMD_ERR_INCOMPLETE;
    }
}

/* Check ambiguous match */
static int
is_cmd_ambiguous (vector cmd_vector,
                  const char *command,
                  vector matches,
                  enum match_type type)
{
  unsigned int i;
  unsigned int j;
  const char *str = NULL;
  const char *matched = NULL;
  vector match_vector;
  struct cmd_token *cmd_token;

  if (command == NULL)
    command = "";

  for (i = 0; i < vector_active (matches); i++)
    if ((match_vector = vector_slot (matches, i)) != NULL)
      {
	int match = 0;

	for (j = 0; j < vector_active (match_vector); j++)
	  if ((cmd_token = vector_slot (match_vector, j)) != NULL)
	    {
	      enum match_type ret;

	      assert(cmd_token->type == TOKEN_TERMINAL);
	      if (cmd_token->type != TOKEN_TERMINAL)
		continue;

	      str = cmd_token->cmd;

	      switch (type)
		{
		case exact_match:
		  if (!TERMINAL_RECORD (cmd_token->terminal)
		      && strcmp (command, str) == 0)
		    match++;
		  break;
		case partly_match:
		  if (!TERMINAL_RECORD (cmd_token->terminal)
		      && strncmp (command, str, strlen (command)) == 0)
		    {
		      if (matched && strcmp (matched, str) != 0)
			return 1;	/* There is ambiguous match. */
		      else
			matched = str;
		      match++;
		    }
		  break;
		case range_match:
		  if (cmd_range_match (str, command))
		    {
		      if (matched && strcmp (matched, str) != 0)
			return 1;
		      else
			matched = str;
		      match++;
		    }
		  break;
#ifdef HAVE_IPV6
		case ipv6_match:
		  if (cmd_token->terminal == TERMINAL_IPV6)
		    match++;
		  break;
		case ipv6_prefix_match:
		  if ((ret = cmd_ipv6_prefix_match (command)) != no_match)
		    {
		      if (ret == partly_match)
			return 2;	/* There is incomplete match. */

		      match++;
		    }
		  break;
#endif /* HAVE_IPV6 */
		case ipv4_match:
		  if (cmd_token->terminal == TERMINAL_IPV4)
		    match++;
		  break;
		case ipv4_prefix_match:
		  if ((ret = cmd_ipv4_prefix_match (command)) != no_match)
		    {
		      if (ret == partly_match)
			return 2;	/* There is incomplete match. */

		      match++;
		    }
		  break;
		case ifname_match:
		  if (cmd_token->terminal == TERMINAL_USP)
		    match++;
		  break;
		case mac_match:
			if (cmd_token->terminal == TERMINAL_MAC)
				match++;
			break;

		case extend_match:
		  if (TERMINAL_RECORD (cmd_token->terminal))
		    match++;
		  break;
		case no_match:
		default:
		  break;
		}
	    }
	if (!match)
	  vector_slot (cmd_vector, i) = NULL;
      }
  return 0;
}

/* If src matches dst return dst string, otherwise return NULL */
static const char *
cmd_entry_function (const char *src, struct cmd_token *token)
{
  const char *dst = token->cmd;

  /* Skip variable arguments. */
  if (TERMINAL_RECORD (token->terminal))
    return NULL;

  /* In case of 'command \t', given src is NULL string. */
  if (src == NULL)
    return dst;

  /* Matched with input string. */
  if (strncmp (src, dst, strlen (src)) == 0)
    return dst;

  return NULL;
}

/* If src matches dst return dst string, otherwise return NULL */
/* This version will return the dst string always if it is
   CMD_VARIABLE for '?' key processing */
static const char *
cmd_entry_function_desc (const char *src, struct cmd_token *token)
{
  const char *dst = token->cmd;

  switch (token->terminal)
    {
      case TERMINAL_VARARG:
        return dst;

      case TERMINAL_RANGE:
        if (cmd_range_match (dst, src))
          return dst;
        else
          return NULL;
#ifdef HAVE_IPV6
      case TERMINAL_IPV6:
        if (cmd_ipv6_match (src))
          return dst;
        else
          return NULL;
      case TERMINAL_IPV6_PREFIX:
        if (cmd_ipv6_prefix_match (src))
          return dst;
        else
          return NULL;
#endif
      case TERMINAL_IPV4:
        if (cmd_ipv4_match (src))
          return dst;
        else
          return NULL;

			case TERMINAL_MAC:
				if (cmd_mac_match (src))
					return dst;
				else
					return NULL;

			case TERMINAL_USP:
				if (cmd_ifname_match (src))
					return dst;
				else
					return NULL;

      case TERMINAL_IPV4_PREFIX:
        if (cmd_ipv4_prefix_match (src))
          return dst;
        else
          return NULL;

      /* Optional or variable commands always match on '?' */
      case TERMINAL_OPTION:
      case TERMINAL_VARIABLE:
			case TERMINAL_NAME:
        return dst;

      case TERMINAL_LITERAL:
        /* In case of 'command \t', given src is NULL string. */
        if (src == NULL)
          return dst;

        if (strncmp (src, dst, strlen (src)) == 0)
          return dst;
        else
          return NULL;

      default:
        return NULL;
    }
}

/**
 * Check whether a string is already present in a vector of strings.
 * @param v A vector of char*.
 * @param str A char*.
 * @return 0 if str is already present in the vector, 1 otherwise.
 */
static int
cmd_unique_string (vector v, const char *str)
{
  unsigned int i;
  char *match;

  for (i = 0; i < vector_active (v); i++)
    if ((match = vector_slot (v, i)) != NULL)
      if (strcmp (match, str) == 0)
	return 0;
  return 1;
}

/**
 * Check whether a struct cmd_token matching a given string is already
 * present in a vector of struct cmd_token.
 * @param v A vector of struct cmd_token*.
 * @param str A char* which should be searched for.
 * @return 0 if there is a struct cmd_token* with its cmd matching str,
 *         1 otherwise.
 */
static int
desc_unique_string (vector v, const char *str)
{
  unsigned int i;
  struct cmd_token *token;

  for (i = 0; i < vector_active (v); i++)
    if ((token = vector_slot (v, i)) != NULL)
      if (strcmp (token->cmd, str) == 0)
	return 0;
  return 1;
}

static int
cmd_try_do_shortcut (enum node_type node, char* first_word) {
  if ( first_word != NULL &&
       node != AUTH_NODE &&
       node != USER_NODE &&
       node != CONFIG_NODE &&
       0 == strcmp( "do", first_word ) )
    return 1;
  return 0;
}

static void
cmd_matches_free(vector *matches)
{
  unsigned int i;
  vector cmd_matches;

  for (i = 0; i < vector_active(*matches); i++)
    if ((cmd_matches = vector_slot(*matches, i)) != NULL)
      vector_free(cmd_matches);
  vector_free(*matches);
  *matches = NULL;
}

static int
cmd_describe_cmp(const void *a, const void *b)
{
  const struct cmd_token *first = *(struct cmd_token * const *)a;
  const struct cmd_token *second = *(struct cmd_token * const *)b;

  return strcmp(first->cmd, second->cmd);
}

static void
cmd_describe_sort(vector matchvec)
{
  qsort(matchvec->index, vector_active(matchvec),
        sizeof(void*), cmd_describe_cmp);
}

/* '?' describe command support. */
static vector
cmd_describe_command_real (vector vline, struct vty *vty, int *status)
{
  unsigned int i;
  vector cmd_vector;
#define INIT_MATCHVEC_SIZE 10
  vector matchvec;
  struct cmd_element *cmd_element;
  unsigned int index;
  int ret;
  enum match_type match;
  char *command;
  vector matches = NULL;
  vector match_vector;
  uint32_t command_found = 0;
  const char *last_word;

  /* Set index. */
  if (vector_active (vline) == 0)
    {
      *status = CMD_ERR_NO_MATCH;
      return NULL;
    }

  index = vector_active (vline) - 1;
  /* Make copy vector of current node's command vector. */
  cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

  /* Prepare match vector */
  matchvec = vector_init (INIT_MATCHVEC_SIZE);

  /* Filter commands and build a list how they could possibly continue. */
  for (i = 0; i <= index; i++)
    {
      command = vector_slot (vline, i);

      if (matches)
	cmd_matches_free(&matches);
      ret = cmd_vector_filter(cmd_vector,
	                      FILTER_RELAXED,
	                      vline, i,
	                      &match,
	                      &matches);

      if (ret != CMD_SUCCESS)
	{
	  vector_free (cmd_vector);
	  vector_free (matchvec);
	  cmd_matches_free(&matches);
	  *status = ret;
	  return NULL;
	}

      /* The last match may well be ambigious, so break here */
      if (i == index)
	break;

      if (match == vararg_match)
	{
	  /* We found a vararg match - so we can throw out the current matches here
	   * and don't need to continue checking the command input */
	  unsigned int j, k;

	  for (j = 0; j < vector_active (matches); j++)
	    if ((match_vector = vector_slot (matches, j)) != NULL)
	      for (k = 0; k < vector_active (match_vector); k++)
	        {
	          struct cmd_token *token = vector_slot (match_vector, k);
	          vector_set (matchvec, token);
	        }

	  *status = CMD_SUCCESS;
	  vector_set(matchvec, &token_cr);
	  vector_free (cmd_vector);
	  cmd_matches_free(&matches);
	  cmd_describe_sort(matchvec);
	  return matchvec;
	}

      ret = is_cmd_ambiguous(cmd_vector, command, matches, match);
      if (ret == 1) //ambiguous match
	{
	  vector_free (cmd_vector);
	  vector_free (matchvec);
	  cmd_matches_free(&matches);
	  *status = CMD_ERR_AMBIGUOUS;
	  return NULL;
	}
      else if (ret == 2)  //incomplete match
	{
	  vector_free (cmd_vector);
	  vector_free (matchvec);
	  cmd_matches_free(&matches);
	  *status = CMD_ERR_NO_MATCH;
	  return NULL;
	}
    }

  /* Make description vector. */
  for (i = 0; i < vector_active (matches); i++) //matches is the vector of matched tokens of all the cmd elements
    {
    //how many cmd elements left can match the part of command we have put
      if ((cmd_element = vector_slot (cmd_vector, i)) != NULL)
	{
	  unsigned int j;
	  vector vline_trimmed;

	  command_found++;
	  last_word = vector_slot(vline, vector_active(vline) - 1);
	  if (last_word == NULL || !strlen(last_word))
	    {
	      vline_trimmed = vector_copy(vline);
	      vector_unset(vline_trimmed, vector_active(vline_trimmed) - 1);

	      if (cmd_is_complete(cmd_element, vline_trimmed)
		  && desc_unique_string(matchvec, command_cr))
		{
		  if (match != vararg_match)
		    vector_set(matchvec, &token_cr);
		}
	      vector_free(vline_trimmed);
	    }

	  match_vector = vector_slot (matches, i);
	  if (match_vector)
	    {
	      for (j = 0; j < vector_active(match_vector); j++)
		{
		  struct cmd_token *token = vector_slot(match_vector, j);
		  const char *string;

		  string = cmd_entry_function_desc(command, token);
		  if (string && desc_unique_string(matchvec, string))
		    vector_set(matchvec, token);
		}
	    }
	}
    }

  /*
   * We can get into this situation when the command is complete
   * but the last part of the command is an optional piece of
   * the cli.
   */
#if 0
  last_word = vector_slot(vline, vector_active(vline) - 1);
  if (command_found == 0 && (last_word == NULL || !strlen(last_word)))
    vector_set(matchvec, &token_cr);
#endif

  vector_free (cmd_vector);
  cmd_matches_free(&matches);

  if (vector_slot (matchvec, 0) == NULL)
    {
      vector_free (matchvec);
      *status = CMD_ERR_NO_MATCH;
      return NULL;
    }

  *status = CMD_SUCCESS;
  cmd_describe_sort(matchvec);
  return matchvec;
}

vector
cmd_describe_command (vector vline, struct vty *vty, int *status)
{
  vector ret;

  if ( cmd_try_do_shortcut(vty->node, vector_slot(vline, 0) ) )
    {
      enum node_type onode;
      vector shifted_vline;
      unsigned int index;

      onode = vty->node;
      vty->node = CONFIG_NODE;
      /* We can try it on enable node, cos' the vty is authenticated */

      shifted_vline = vector_init (vector_count(vline));
      /* use memcpy? */
      for (index = 1; index < vector_active (vline); index++)
	{
	  vector_set_index (shifted_vline, index-1, vector_lookup(vline, index));
	}

      ret = cmd_describe_command_real (shifted_vline, vty, status);

      vector_free(shifted_vline);
      vty->node = onode;
      return ret;
  }


  return cmd_describe_command_real (vline, vty, status);
}


/* Check LCD of matched command. */
static int
cmd_lcd (char **matched)
{
  int i;
  int j;
  int lcd = -1;
  char *s1, *s2;
  char c1, c2;

  if (matched[0] == NULL || matched[1] == NULL)
    return 0;

  for (i = 1; matched[i] != NULL; i++)
    {
      s1 = matched[i - 1];
      s2 = matched[i];

      for (j = 0; (c1 = s1[j]) && (c2 = s2[j]); j++)
	if (c1 != c2)
	  break;

      if (lcd < 0)
	lcd = j;
      else
	{
	  if (lcd > j)
	    lcd = j;
	}
    }
  return lcd;
}

static int
cmd_complete_cmp(const void *a, const void *b)
{
  const char *first = *(char * const *)a;
  const char *second = *(char * const *)b;

  if (!first)
    {
      if (!second)
        return 0;
      return 1;
    }
  if (!second)
    return -1;

  return strcmp(first, second);
}

static void
cmd_complete_sort(vector matchvec)
{
  qsort(matchvec->index, vector_active(matchvec),
        sizeof(void*), cmd_complete_cmp);
}

/* Command line completion support. */
static char **
cmd_complete_command_real (vector vline, struct vty *vty, int *status, int islib)
{
  unsigned int i;
  vector cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));
#define INIT_MATCHVEC_SIZE 10
  vector matchvec;
  unsigned int index;
  char **match_str;
  struct cmd_token *token;
  char *command;
  int lcd;
  vector matches = NULL;
  vector match_vector;

  if (vector_active (vline) == 0)
    {
      vector_free (cmd_vector);
      *status = CMD_ERR_NO_MATCH;
      return NULL;
    }
  else
    index = vector_active (vline) - 1;

  /* First, filter by command string */
  for (i = 0; i <= index; i++)
    {
      command = vector_slot (vline, i);
      enum match_type match;
      int ret;

      if (matches)
        cmd_matches_free(&matches);

      /* First try completion match, if there is exactly match return 1 */
      ret = cmd_vector_filter(cmd_vector,
	                      FILTER_RELAXED,
	                      vline, i,
	                      &match,
	                      &matches);

      if (ret != CMD_SUCCESS)
	{
	  vector_free(cmd_vector);
	  cmd_matches_free(&matches);
	  *status = ret;
	  return NULL;
	}

      /* Break here - the completion mustn't be checked to be non-ambiguous */
      if (i == index)
	break;

      /* If there is exact match then filter ambiguous match else check
	 ambiguousness. */
      ret = is_cmd_ambiguous (cmd_vector, command, matches, match);
      if (ret == 1)
	{
	  vector_free (cmd_vector);
	  cmd_matches_free(&matches);
	  *status = CMD_ERR_AMBIGUOUS;
	  return NULL;
	}
      /*
	   else if (ret == 2)
	   {
	   vector_free (cmd_vector);
	   cmd_matches_free(&matches);
	   *status = CMD_ERR_NO_MATCH;
	   return NULL;
	   }
	 */
    }

  /* Prepare match vector. */
  matchvec = vector_init (INIT_MATCHVEC_SIZE);

  /* Build the possible list of continuations into a list of completions */
  for (i = 0; i < vector_active (matches); i++)
    if ((match_vector = vector_slot (matches, i)))
      {
	const char *string;
	unsigned int j;

	for (j = 0; j < vector_active (match_vector); j++)
	  if ((token = vector_slot (match_vector, j)))
            {
              string = cmd_entry_function (vector_slot (vline, index), token);
              if (string && cmd_unique_string (matchvec, string))
                vector_set (matchvec, (islib != 0 ?
                                      XSTRDUP (MTYPE_TMP, string) :
                                      strdup (string) /* rl freed */));
            }
      }

  /* We don't need cmd_vector any more. */
  vector_free (cmd_vector);
  cmd_matches_free(&matches);

  /* No matched command */
  if (vector_slot (matchvec, 0) == NULL)
    {
      vector_free (matchvec);

      /* In case of 'command \t' pattern.  Do you need '?' command at
         the end of the line. */
      if (vector_slot (vline, index) == '\0')
	*status = CMD_ERR_NOTHING_TODO;
      else
	*status = CMD_ERR_NO_MATCH;
      return NULL;
    }

  /* Only one matched */
  if (vector_slot (matchvec, 1) == NULL)
    {
      match_str = (char **) matchvec->index;
      vector_only_wrapper_free (matchvec);
      *status = CMD_COMPLETE_FULL_MATCH;
      return match_str;
    }
  /* Make it sure last element is NULL. */
  vector_set (matchvec, NULL);

  /* Check LCD of matched strings. */
  if (vector_slot (vline, index) != NULL)
    {
      lcd = cmd_lcd ((char **) matchvec->index);

      if (lcd)
	{
	  int len = strlen (vector_slot (vline, index));

	  if (len < lcd)
	    {
	      char *lcdstr;

	      lcdstr = (islib != 0 ?
                        XMALLOC (MTYPE_TMP, lcd + 1) :
                        malloc(lcd + 1));
	      memcpy (lcdstr, matchvec->index[0], lcd);
	      lcdstr[lcd] = '\0';

	      /* match_str = (char **) &lcdstr; */

	      /* Free matchvec. */
	      for (i = 0; i < vector_active (matchvec); i++)
                {
                  if (vector_slot (matchvec, i))
                    {
                      if (islib != 0)
                        XFREE (MTYPE_TMP, vector_slot (matchvec, i));
                      else
                        free (vector_slot (matchvec, i));
                    }
                }
	      vector_free (matchvec);

	      /* Make new matchvec. */
	      matchvec = vector_init (INIT_MATCHVEC_SIZE);
	      vector_set (matchvec, lcdstr);
	      match_str = (char **) matchvec->index;
	      vector_only_wrapper_free (matchvec);

	      *status = CMD_COMPLETE_MATCH;
	      return match_str;
	    }
	}
    }

  match_str = (char **) matchvec->index;
  cmd_complete_sort(matchvec);
  vector_only_wrapper_free (matchvec);
  *status = CMD_COMPLETE_LIST_MATCH;
  return match_str;
}

char **
cmd_complete_command_lib (vector vline, struct vty *vty, int *status, int islib)
{
  char **ret;

  if ( cmd_try_do_shortcut(vty->node, vector_slot(vline, 0) ) )
    {
      enum node_type onode;
      vector shifted_vline;
      unsigned int index;

      onode = vty->node;
      vty->node = CONFIG_NODE;
      /* We can try it on enable node, cos' the vty is authenticated */

      shifted_vline = vector_init (vector_count(vline));
      /* use memcpy? */
      for (index = 1; index < vector_active (vline); index++)
	{
	  vector_set_index (shifted_vline, index-1, vector_lookup(vline, index));
	}

      ret = cmd_complete_command_real (shifted_vline, vty, status, islib);

      vector_free(shifted_vline);
      vty->node = onode;
      return ret;
  }

  return cmd_complete_command_real (vline, vty, status, islib);
}

char **
cmd_complete_command (vector vline, struct vty *vty, int *status)
{
  return cmd_complete_command_lib (vline, vty, status, 0);
}

/* return parent node */
/* MUST eventually converge on CONFIG_NODE */
enum node_type
node_parent ( enum node_type node )
{
  enum node_type ret;

  assert (node > CONFIG_NODE);

  switch (node)
    {
    case NEIGHBOR_NODE:
	case NEIGHBOR_NODE_IPV6:
	case BGP_VPN_INSTANCE_NODE:
	case BGP_VPNV4_NODE:
	case BGP_IPV6_NODE:
       ret = BGP_NODE;
       break;
//    case KEYCHAIN_KEY_NODE:
//      ret = KEYCHAIN_NODE;
//      break;
    case AREA_NODE:
		ret = OSPF_NODE;
		break;
	case VSI_NODE:
    case L3VPN_NODE:
	case PW_NODE:
	case APS_NODE:
	case MPLSTP_OAM_SESS_NODE:
	case MPLSTP_OAM_MEG_NODE:
		ret = MPLS_NODE;
		break;
	case CFM_MA_NODE:
		ret = CFM_MD_NODE;
		break;
    case CONNECTION_NODE:
        ret = XCONNECT_GROUP_NODE;
        break;
    case PW_MASTER_NODE:
        ret = CONNECTION_NODE;
        break;
    case PW_BACKUP_NODE:
        ret = PW_MASTER_NODE;
        break;
    case SERVICE_INSTANCE_NODE:
        ret = PHYSICAL_IF_NODE;
        break;
    default:
      ret = CONFIG_NODE;
    }

  return ret;
}

/* Execute command by argument vline vector. */
static int
cmd_execute_command_real (vector vline,
			  enum filter_type filter,
			  struct vty *vty,
			  struct cmd_element **cmd)
{
  unsigned int i;
  unsigned int index;
  vector cmd_vector;
  struct cmd_element *cmd_element;
  struct cmd_element *matched_element;
  unsigned int matched_count, incomplete_count;
  int argc;
  const char *argv[CMD_ARGC_MAX] = {NULL};
  enum match_type match = 0;
  char *command;
  int ret;
  vector matches;
  int cmd_left = 0;


  /* Make copy of command elements. */
  cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

  for (index = 0; index < vector_active (vline); index++)
    {
      command = vector_slot (vline, index);
      ret = cmd_vector_filter(cmd_vector,
			      filter,
			      vline, index,
			      &match,
			      &matches);

	//if we have left more than one cmd string in cmd_vector; check unmatch state is still
	//unknow
	for (i = 0; i < vector_active (cmd_vector); i++)
		if ((cmd_element = vector_slot (cmd_vector, i)) != NULL)
			cmd_left++;

	if(cmd_left >= 1 || validity_check.match_fail_cnt > 1)
	{
		cmd_left = 0;
		memset(&validity_check, 0, sizeof(struct match_flag));
	}


      if (ret != CMD_SUCCESS)
	{
	  cmd_matches_free(&matches);
	  return ret;
	}

      if (match == vararg_match)
	{
	  cmd_matches_free(&matches);
	  break;
	}

      ret = is_cmd_ambiguous (cmd_vector, command, matches, match);
      cmd_matches_free(&matches);

      if (ret == 1)
	{
	  vector_free(cmd_vector);
	  return CMD_ERR_AMBIGUOUS;
	}
      else if (ret == 2)
	{
	  vector_free(cmd_vector);
		zlog_debug(0, "%s: CMD_ERR_NO_MATCH ret %d, cmd:%s, node %d\n",__FUNCTION__, 
	                            ret, vty->buf, vty->node);	  
	  return CMD_ERR_NO_MATCH;
	}
    }

  /* Check matched count. */
  matched_element = NULL;
  matched_count = 0;
  incomplete_count = 0;

  for (i = 0; i < vector_active (cmd_vector); i++)
    if ((cmd_element = vector_slot (cmd_vector, i)))
      {
	if (cmd_is_complete(cmd_element, vline))
	  {
	    matched_element = cmd_element;
	    matched_count++;
	  }
	else
	  {
	    incomplete_count++;
	  }
      }

  /* Finish of using cmd_vector. */
  vector_free (cmd_vector);

  /* To execute command, matched_count must be 1. */
  if (matched_count == 0)
    {
      if (incomplete_count)
	return CMD_ERR_INCOMPLETE;
      else
	  {
			zlog_debug(0, "%s: CMD_ERR_NO_MATCH2 ret %d, lastcmd: %s, node %d\n",__FUNCTION__, ret, vty->buf, vty->node);
	return CMD_ERR_NO_MATCH;
	  }
    }

  if (matched_count > 1)
    return CMD_ERR_AMBIGUOUS;

  ret = cmd_parse(matched_element, vline, &argc, argv);
  zlog_debug(0, "%s: cmd_parse ret %d, daemon %llx\n",__FUNCTION__, ret, matched_element->daemon);
  if (ret != CMD_SUCCESS)
    return ret;

  /* For vtysh execution. */
  if (cmd)
    *cmd = matched_element;

  if (matched_element->daemon)
    return CMD_SUCCESS_DAEMON;

	/* if vty is not type of daemon, then it is in vtysh, return before exec cmd func. */
	if(NULL != matched_element->func
		&& (vty->type == VTY_SSH
		|| vty->type == VTY_TERM
		|| vty->type == VTY_SHELL))
	{
		if(vty->privilege < matched_element->level )
		{
			vty_out(vty, "Permission deny! Cmd need level %d%s", matched_element->level, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
    zlog_debug(0, "%s: exec cmd\n",__FUNCTION__);
  /* Execute matched command. */
    ret = (*matched_element->func) (matched_element, vty, argc, argv);  
    zlog_debug(0, "%s: exec cmd ret %d\n",__FUNCTION__, ret);
    return ret;
}

/**
 * Execute a given command, handling things like "do ..." and checking
 * whether the given command might apply at a parent node if doesn't
 * apply for the current node.
 *
 * @param vline Command line input, vector of char* where each element is
 *              one input token.
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element of the matched command
 *            will be stored, if any. May be set to NULL if this info is
 *            not needed.
 * @param vtysh If set != 0, don't lookup the command at parent nodes.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int
cmd_execute_command (vector vline, struct vty *vty, struct cmd_element **cmd,
		     int vtysh) {
  int ret, saved_ret, tried = 0;
  enum node_type onode, try_node;
	//int privilege;

  onode = try_node = vty->node;
#if 0
  if ( cmd_try_do_shortcut(vty->node, vector_slot(vline, 0) ) )
    {
      vector shifted_vline;
      unsigned int index;

      vty->node = CONFIG_NODE;
      /* We can try it on enable node, cos' the vty is authenticated */

      shifted_vline = vector_init (vector_count(vline));
      /* use memcpy? */
      for (index = 1; index < vector_active (vline); index++)
	{
	  vector_set_index (shifted_vline, index-1, vector_lookup(vline, index));
	}

      ret = cmd_execute_command_real (shifted_vline, FILTER_RELAXED, vty, cmd);

      vector_free(shifted_vline);
      vty->node = onode;
      return ret;
  }
#endif

	//if(vty->type == VTY_FILE)
	//	privilege = ROOT_LEVE;
	//else
	//	privilege = vty->privilege;

  saved_ret = ret = cmd_execute_command_real (vline, FILTER_RELAXED, vty, cmd);
    zlog_debug(0, "%s: cmd_execute_command_real ret %d, vtysh %d\n",__FUNCTION__, ret, vtysh);
  if (vtysh)
    return saved_ret;

  /* This assumes all nodes above CONFIG_NODE are childs of CONFIG_NODE */
  while ( ret != CMD_SUCCESS && ret != CMD_WARNING  && ret != CMD_CONTINUE
		&& ret != CMD_REPLAY && ret != CMD_WAIT 
	  && vty->node > CONFIG_NODE )
    {
      try_node = node_parent(try_node);
      vty->node = try_node;
      ret = cmd_execute_command_real (vline, FILTER_RELAXED, vty, cmd);
      zlog_debug(0, "%s: NODE %d, cmd_execute_command_real ret %d\n",__FUNCTION__, try_node, ret);
      tried = 1;
      if (ret == CMD_SUCCESS || ret == CMD_WARNING)
	{
	  /* succesfull command, leave the node as is */
	  return ret;
	}
    }
  /* no command succeeded, reset the vty to the original node and
     return the error for this node */
  if ( tried )
    vty->node = onode;
  return saved_ret;
}

/**
 * Execute a given command, matching it strictly against the current node.
 * This mode is used when reading config files.
 *
 * @param vline Command line input, vector of char* where each element is
 *              one input token.
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element* of the matched command
 *            will be stored, if any. May be set to NULL if this info is
 *            not needed.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int
cmd_execute_command_strict (vector vline, struct vty *vty,
			    struct cmd_element **cmd)
{
	//int privilege;

	///*vty have highest privilege while reading config file*/
	//if(vty->type == VTY_FILE)
	//	privilege = ROOT_LEVE;
	//else
	//	privilege = vty->privilege;

  return cmd_execute_command_real(vline, FILTER_STRICT, vty, cmd);
}

/**
 * Parse one line of config, walking up the parse tree attempting to find a match
 *
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element* of the match command
 *            will be stored, if any.  May be set to NULL if this info is
 *            not needed.
 * @param use_daemon Boolean to control whether or not we match on CMD_SUCCESS_DAEMON
 *                   or not.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int
command_config_read_one_line (struct vty *vty, struct cmd_element **cmd, int use_daemon)
{
  vector vline;
  int saved_node;
  int ret;

  vline = cmd_make_strvec (vty->buf);

  /* In case of comment line */
  if (vline == NULL)
    return CMD_SUCCESS;

  /* Execute configuration command : this is strict match */
  ret = cmd_execute_command_strict (vline, vty, cmd);

  saved_node = vty->node;

  while (!(use_daemon && ret == CMD_SUCCESS_DAEMON) &&
	 ret != CMD_SUCCESS && ret != CMD_WARNING &&
	 ret != CMD_ERR_NOTHING_TODO && vty->node != CONFIG_NODE) {
    vty->node = node_parent(vty->node);
    ret = cmd_execute_command_strict (vline, vty, cmd);
  }

  // If climbing the tree did not work then ignore the command and
  // stay at the same node
  if (!(use_daemon && ret == CMD_SUCCESS_DAEMON) &&
      ret != CMD_SUCCESS && ret != CMD_WARNING &&
      ret != CMD_ERR_NOTHING_TODO)
    {
      vty->node = saved_node;
    }

  cmd_free_strvec (vline);

  return ret;
}

/* Configration make from file. */
int
config_from_file (struct vty *vty, FILE *fp, unsigned int *line_num)
{
  int ret;
  *line_num = 0;

  while (fgets (vty->buf, VTY_BUFSIZ, fp))
    {
      ++(*line_num);

      ret = command_config_read_one_line (vty, NULL, 0);

      if (ret != CMD_SUCCESS && ret != CMD_WARNING
	  && ret != CMD_ERR_NOTHING_TODO)
	return ret;
    }
  return CMD_SUCCESS;
}

/* Configration from terminal */
DEFUN (vty_sysrun,
		vty_sysrun_cmd,
		"sysrun",
		"system operation\n")
{
    vty->node = SYSRUN_NODE;
  return CMD_SUCCESS;
}

/* Configration from terminal */
DEFUN (config_terminal,
       config_terminal_cmd,
       "configure terminal",
       "Configuration from vty interface\n"
       "Configuration terminal\n")
{
    vty->node = CONFIG_NODE;
  return CMD_SUCCESS;
}
#if 0
/* Enable command */
DEFUN (enable,
       config_enable_cmd,
       "enable",
       "Turn on privileged mode command\n")
{
    vty->node = ENABLE_NODE;

  return CMD_SUCCESS;
}
#endif
/* Down vty node level. */
DEFUN (config_exit,
       config_exit_cmd,
       "exit",
       "Exit current mode and down to previous mode\n")
{
  switch (vty->node)
    {
    case USER_NODE:
	  vty->node = LOGIN_NODE;
	  break;
    case CONFIG_NODE:
      vty->node = CONFIG_NODE;
//      vty_config_unlock (vty);
      break;
	case VSI_NODE:
    case L3VPN_NODE:
	case PW_NODE:
	case APS_NODE:
	case MPLSTP_OAM_SESS_NODE:
	case MPLSTP_OAM_MEG_NODE:
		vty->node = MPLS_NODE;
		break;
    case INTERFACE_NODE:
    case PHYSICAL_IF_NODE:
    case PHYSICAL_SUBIF_NODE:
	case TDM_IF_NODE:
	case TDM_SUBIF_NODE:
	case STM_IF_NODE:
	case STM_SUBIF_NODE:
    case LOOPBACK_IF_NODE:
    case TUNNEL_IF_NODE:
    case TRUNK_IF_NODE:
	case TRUNK_SUBIF_NODE:
    case VLANIF_NODE:
    case CLOCK_NODE:
	case VCG_NODE:
	case E1_IF_NODE:
    case BGP_NODE:
    case RIP_NODE:
	case RIPNG_NODE:
	case MPLS_NODE:
	case ROUTE_NODE:
    case ARP_NODE:
	case ACL_NODE:
    case OSPF_NODE:
	case OSPF_DCN_NODE:
	case OSPF6_NODE:
    case ISIS_NODE:
//    case VTY_NODE:
    case SYSLOG_NODE:
	case VLAN_NODE:
	case DEVM_NODE:
	case POOL_NODE:
	case QOS_DOMAIN_NODE:
	case QOS_PHB_NODE:
	case QOS_CAR_NODE:
	case HQOS_WRED_NODE:
	case HQOS_QUEUE_NODE:
	case AAA_NODE:
	case AAA_LOCAL_USER_NODE:
    case ALARM_NODE:
	case STATIS_NODE:
	case BFD_SESSION_NODE:
	case BFD_TEMPLATE_NODE:
	case SDH_DXC_NODE:
	case CFM_MD_NODE:
	case CFM_SESSION_NODE:
	case SNMPD_NODE:
	case SLA_SESSION_NODE:

	case ELPS_SESSION_NODE:
    case ERPS_SESSION_NODE:
	case OPENFLOW_NODE:

	case IGMP_NODE:
	case TCP_NODE:
	case DHCPV6_NODE:
	case SYSRUN_NODE:
    case PW_CLASS_NODE:
    case XCONNECT_GROUP_NODE:
	case TRUNK_GLOABLE_NODE:
    case FILEM_NODE:
	case USER_LINE_NODE:
      vty->node = CONFIG_NODE;
      break;
	case AREA_NODE:
	  vty->node = OSPF_NODE;
	  break;
	case CFM_MA_NODE:
	  vty->node = CFM_MD_NODE;
	  break;
	case NEIGHBOR_NODE:
	case NEIGHBOR_NODE_IPV6:
	case BGP_VPN_INSTANCE_NODE:
	case BGP_VPNV4_NODE:
	case BGP_IPV6_NODE:
	  vty->node = BGP_NODE;
	  break;
    case CONNECTION_NODE:
        vty->node = XCONNECT_GROUP_NODE;
        break;
    case PW_MASTER_NODE:
        vty->node = CONNECTION_NODE;
        break;
    case PW_BACKUP_NODE:
        vty->node = PW_MASTER_NODE;
        break;
    case SERVICE_INSTANCE_NODE:
        vty->node = PHYSICAL_IF_NODE;
        break;
	case ISIS_FAMILY_V4_NODE:
	case ISIS_FAMILY_V6_NODE:
		vty->node = ISIS_NODE;
		break;
    default:
      break;
    }
  return CMD_SUCCESS;
}

/* quit is alias of exit. */
ALIAS (config_exit,
       config_quit_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

/* End of configuration. */
DEFUN (config_end,
       config_end_cmd,
       "end",
       "End current mode and change to enable mode.")
{
  switch (vty->node)
    {
    case CONFIG_NODE:
      /* Nothing to do. */
      break;
    case USER_NODE:
	case VLAN_NODE:
	case DEVM_NODE:
    case INTERFACE_NODE:
    case PHYSICAL_IF_NODE:
    case PHYSICAL_SUBIF_NODE:
	case TDM_IF_NODE:
	case TDM_SUBIF_NODE:
	case STM_IF_NODE:
	case STM_SUBIF_NODE:
    case LOOPBACK_IF_NODE:
    case TUNNEL_IF_NODE:
    case TRUNK_IF_NODE:
	case TRUNK_SUBIF_NODE:
    case VLANIF_NODE:
	case CLOCK_NODE:
	case VCG_NODE:
	case E1_IF_NODE:
    case RIP_NODE:
	case RIPNG_NODE:
	case MPLS_NODE:
	case VSI_NODE:
    case L3VPN_NODE:
	case PW_NODE:
	case MPLSTP_OAM_SESS_NODE:
	case MPLSTP_OAM_MEG_NODE:
	case ROUTE_NODE:
	case ACL_NODE:
    case BGP_NODE:
    case OSPF_NODE:
	case OSPF_DCN_NODE:
	case OSPF6_NODE:
    case ISIS_NODE:
//    case VTY_NODE:
    case SYSLOG_NODE:
	case POOL_NODE:
	case AREA_NODE:
	case QOS_DOMAIN_NODE:
	case QOS_PHB_NODE:
	case QOS_CAR_NODE:
	case HQOS_WRED_NODE:
	case HQOS_QUEUE_NODE:
	case AAA_NODE:
	case AAA_LOCAL_USER_NODE:
	case ALARM_NODE:
	case STATIS_NODE:
	case BFD_SESSION_NODE:
	case BFD_TEMPLATE_NODE:
	case SDH_DXC_NODE:
    case SNMPD_NODE:
	case NEIGHBOR_NODE:
	case NEIGHBOR_NODE_IPV6:
	case SLA_SESSION_NODE:
	case OPENFLOW_NODE:
	case PIM_INSTANCE_NODE:
	case TRUNK_GLOABLE_NODE:
	case IGMP_NODE:
	case APS_NODE:
	case BGP_VPN_INSTANCE_NODE:
	case BGP_VPNV4_NODE:
    case BGP_IPV6_NODE:
	case TCP_NODE:
	case DHCPV6_NODE:
	case SYSRUN_NODE:
    case PW_CLASS_NODE:
    case SERVICE_INSTANCE_NODE:
    case XCONNECT_GROUP_NODE:
    case CONNECTION_NODE:
    case PW_MASTER_NODE:
    case PW_BACKUP_NODE:
	case ISIS_FAMILY_V4_NODE:
    case FILEM_NODE:
//      vty_config_unlock (vty);
      vty->node = CONFIG_NODE;
      break;
    default:
      break;
    }
  return CMD_SUCCESS;
}

ALIAS (config_end,
       config_return_cmd,
       "return",
        "Return current mode and down to previous mode\n")
       

#if 0
/* Show version. */
DEFUN (show_version,
       show_version_cmd,
       "show version",
       SHOW_STR
       "Displays app version\n")
{
	vty_out (vty, "Version   : %s %s", IPRAN_VERSION, VTY_NEWLINE);
    vty_out (vty, "Build date: %s%s", BUILDDATE, VTY_NEWLINE);

  return CMD_SUCCESS;
}
#endif

/* Help display function for all node. */
DEFUN (config_help,
       config_help_cmd,
       "help",
       "Description of the interactive help system\n")
{
  vty_out (vty,
	   "Quagga VTY provides advanced help feature.  When you need help,%s\
anytime at the command line please press '?'.%s\
%s\
If nothing matches, the help list will be empty and you must backup%s\
 until entering a '?' shows the available options.%s\
Two styles of help are provided:%s\
1. Full help is available when you are ready to enter a%s\
command argument (e.g. 'show ?') and describes each possible%s\
argument.%s\
2. Partial help is provided when an abbreviated argument is entered%s\
   and you want to know what arguments match the input%s\
   (e.g. 'show me?'.)%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE,
	   VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE,
	   VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);
  return CMD_SUCCESS;
}

/* Help display function for all node. */
DEFUN (config_list,
       config_list_cmd,
       "list",
       "Print command list\n")
{
  unsigned int i;
  struct cmd_node *cnode = vector_slot (cmdvec, vty->node);
  struct cmd_element *cmd;

  for (i = 0; i < vector_active (cnode->cmd_vector); i++)
    if ((cmd = vector_slot (cnode->cmd_vector, i)) != NULL
        && !(cmd->attr == CMD_ATTR_DEPRECATED
             || cmd->attr == CMD_ATTR_HIDDEN))
      vty_out (vty, "  %s%s", cmd->string,
	       VTY_NEWLINE);
  return CMD_SUCCESS;
}

/* Write current configuration into the terminal. */
DEFUN (config_write_terminal,
       config_write_terminal_cmd,
       "write terminal",
       "Write running configuration to memory, network, or terminal\n"
       "Write to terminal\n")
{
  unsigned int i;
  struct cmd_node *node;

  if (vty->type == VTY_SHELL_SERV)
    {
      for (i = 0; i < vector_active (cmdvec); i++)
	if ((node = vector_slot (cmdvec, i)) && node->func && node->vtysh)
	  {
	    if ((*node->func) (vty))
	      vty_out (vty, "!%s", VTY_NEWLINE);
	  }
    }
  else
    {
      //vty_out (vty, "%sCurrent configuration:%s", VTY_NEWLINE, VTY_NEWLINE);
      vty_out (vty, "!%s", VTY_NEWLINE);

      for (i = 0; i < vector_active (cmdvec); i++)
	if ((node = vector_slot (cmdvec, i)) && node->func)
	  {
	    if ((*node->func) (vty))
	      vty_out (vty, "!%s", VTY_NEWLINE);
	  }
//      vty_out (vty, "end%s",VTY_NEWLINE);
    }
  return CMD_SUCCESS;
}


/* Write current configuration into the terminal. */
DEFUN (config_write_factory_terminal,
	  config_write_factory_terminal_cmd,
	  "write factory terminal",
	  "Write running configuration to memory, network, or terminal\n"
	  "Factory configuration"
	  "Write to terminal\n")
{
	unsigned int i;
	struct cmd_node *node;

	if (vty->type == VTY_SHELL_SERV)
	{
		for (i = 0; i < vector_active (cmdvec); i++)
			if ((node = vector_slot (cmdvec, i)) && node->func && node->vtysh)
			{
				if ((node->func_factory) && (*node->func_factory) (vty))
					vty_out (vty, "!%s", VTY_NEWLINE);
			}
	}
	else
	{
		vty_out (vty, "%sCurrent configuration:%s", VTY_NEWLINE, VTY_NEWLINE);
		vty_out (vty, "!%s", VTY_NEWLINE);

		for (i = 0; i < vector_active (cmdvec); i++)
			if ((node = vector_slot (cmdvec, i)) && node->func)
			{
				if ((node->func_factory) && (*node->func_factory) (vty))
					vty_out (vty, "!%s", VTY_NEWLINE);
			}
		//	   vty_out (vty, "end%s",VTY_NEWLINE);
	}
	return CMD_SUCCESS;
}


	   

/* Write current configuration into the terminal. */
ALIAS (config_write_terminal,
       show_running_config_cmd,
       "show running-config",
       SHOW_STR
       "running configuration\n")


DEFUN (config_terminal_length, config_terminal_length_cmd,
       "terminal length <80-512>",
       "Set terminal line parameters\n"
       "Set number of lines on a screen\n"
       "Number of lines on screen (0 for no pausing)\n")
{
  int lines;
  char *endptr = NULL;

  lines = strtol (argv[0], &endptr, 10);
  if (lines < 80 || lines > 512 || *endptr != '\0')
    {
      vty_out (vty, "length is malformed%s", VTY_NEWLINE);
      return CMD_WARNING;
    }
  vty->lines = lines;

  return CMD_SUCCESS;
}

DEFUN (config_terminal_no_length, config_terminal_no_length_cmd,
       "terminal no length",
       "Set terminal line parameters\n"
       NO_STR
       "Set number of lines on a screen\n")
{
  vty->lines = -1;
  return CMD_SUCCESS;
}

DEFUN_HIDDEN (do_echo,
	      echo_cmd,
	      "echo .MESSAGE",
	      "Echo a message back to the vty\n"
	      "The message to echo\n")
{
  char *message;

  vty_out (vty, "%s%s", ((message = argv_concat(argv, argc, 0)) ? message : ""),
	   VTY_NEWLINE);
  if (message)
    XFREE(MTYPE_TMP, message);
  return CMD_SUCCESS;
}

DEFUN (show_commandtree,
       show_commandtree_cmd,
       "show commandtree",
       NO_STR
       "Show command tree\n")
{
  /* TBD */
  vector cmd_vector;
  unsigned int i;

  vty_out (vty, "Current node id: %d%s", vty->node, VTY_NEWLINE);

  /* vector of all commands installed at this node */
  cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

  /* loop over all commands at this node */
  for (i = 0; i < vector_active(cmd_vector); ++i)
    {
      struct cmd_element *cmd_element;

      /* A cmd_element (seems to be) is an individual command */
      if ((cmd_element = vector_slot (cmd_vector, i)) == NULL)
        continue;

      vty_out (vty, "    %s%s", cmd_element->string, VTY_NEWLINE);
    }

  vector_free (cmd_vector);
  return CMD_SUCCESS;
}

DEFUN (show_daemon,
       show_daemon_cmd,
       "show daemon",
       SHOW_STR
       "Tell vtysh whether this process is alive\n")
{
	return CMD_SUCCESS;
}


void
install_default (enum node_type node)
{
  install_element (node, &config_exit_cmd, CMD_SYNC);
  install_element (node, &config_quit_cmd, CMD_SYNC);
  install_element (node, &config_end_cmd, CMD_SYNC);  
  install_element (node, &config_return_cmd, CMD_SYNC);
  
  install_element (node, &config_help_cmd, CMD_LOCAL);
  install_element (node, &config_list_cmd, CMD_LOCAL);

  install_element (node, &config_write_factory_terminal_cmd, CMD_SYNC);
  install_element (node, &config_write_terminal_cmd, CMD_SYNC);
//  install_element (node, &config_write_file_cmd);
//  install_element (node, &config_write_memory_cmd);
//  install_element (node, &config_write_cmd);
  install_element (node, &show_running_config_cmd, CMD_LOCAL);
	install_element (node, &show_daemon_cmd, CMD_LOCAL);


}

extern struct cmd_element vtysh_log_level_ctl_cmd;
extern struct cmd_element vtysh_show_log_level_ctl_cmd;


/* Initialize command interface. Install basic nodes and commands. */
void
cmd_init (int terminal)
{
  command_cr = XSTRDUP(MTYPE_CMD_TOKENS, "<cr>");
  token_cr.type = TOKEN_TERMINAL;
  token_cr.terminal = TERMINAL_LITERAL;
  token_cr.cmd = command_cr;
  token_cr.desc = XSTRDUP(MTYPE_CMD_TOKENS, "");

  /* Allocate initial top vector of commands. */
  cmdvec = vector_init (VECTOR_MIN_SIZE);

  /* Install top nodes. */
  install_node (&auth_node, NULL);
  install_node (&config_node, NULL);
  install_node (&sysrun_node, NULL);

  if (terminal)
  {
	  install_default (SYSRUN_NODE);
	  install_element (CONFIG_NODE, &vty_sysrun_cmd, CMD_SYNC);

  }

//  install_element (CONFIG_NODE, &show_version_cmd);
  install_element (CONFIG_NODE, &show_commandtree_cmd, CMD_LOCAL);

  if (terminal)
  {
	install_element (CONFIG_NODE, &echo_cmd, CMD_LOCAL);
	install_default (CONFIG_NODE);
//	log_cmd_init();
  }

//  install_element (CONFIG_NODE, &vtysh_log_level_ctl_cmd, CMD_LOCAL);
//  install_element (CONFIG_NODE, &vtysh_show_log_level_ctl_cmd, CMD_LOCAL);

  srandom(time(NULL));
}

#if 0
static void
cmd_terminate_token(struct cmd_token *token)
{
  unsigned int i, j;
  vector keyword_vect;

  if (token->multiple)
    {
      for (i = 0; i < vector_active(token->multiple); i++)
        cmd_terminate_token(vector_slot(token->multiple, i));
      vector_free(token->multiple);
      token->multiple = NULL;
    }

  if (token->keyword)
    {
      for (i = 0; i < vector_active(token->keyword); i++)
        {
          keyword_vect = vector_slot(token->keyword, i);
          for (j = 0; j < vector_active(keyword_vect); j++)
            cmd_terminate_token(vector_slot(keyword_vect, j));
          vector_free(keyword_vect);
        }
      vector_free(token->keyword);
      token->keyword = NULL;
    }

  XFREE(MTYPE_CMD_TOKENS, token->cmd);
  XFREE(MTYPE_CMD_TOKENS, token->desc);

  XFREE(MTYPE_CMD_TOKENS, token);
}

static void
cmd_terminate_element(struct cmd_element *cmd)
{
  unsigned int i;

  if (cmd->tokens == NULL)
    return;

  for (i = 0; i < vector_active(cmd->tokens); i++)
    cmd_terminate_token(vector_slot(cmd->tokens, i));

  vector_free(cmd->tokens);
  cmd->tokens = NULL;
}


void
cmd_terminate ()
{
  unsigned int i, j;
  struct cmd_node *cmd_node;
  struct cmd_element *cmd_element;
  vector cmd_node_v;

  if (cmdvec)
    {
      for (i = 0; i < vector_active (cmdvec); i++)
        if ((cmd_node = vector_slot (cmdvec, i)) != NULL)
          {
            cmd_node_v = cmd_node->cmd_vector;

            for (j = 0; j < vector_active (cmd_node_v); j++)
              if ((cmd_element = vector_slot (cmd_node_v, j)) != NULL)
                cmd_terminate_element(cmd_element);

            vector_free (cmd_node_v);
          }

      vector_free (cmdvec);
      cmdvec = NULL;
    }

  if (command_cr)
    XFREE(MTYPE_CMD_TOKENS, command_cr);
  if (token_cr.desc)
    XFREE(MTYPE_CMD_TOKENS, token_cr.desc);
  if (host.name)
    XFREE (MTYPE_HOST, host.name);
  if (host.password)
    XFREE (MTYPE_HOST, host.password);
  if (host.password_encrypt)
    XFREE (MTYPE_HOST, host.password_encrypt);
  if (host.enable)
    XFREE (MTYPE_HOST, host.enable);
  if (host.enable_encrypt)
    XFREE (MTYPE_HOST, host.enable_encrypt);
  if (host.logfile)
    XFREE (MTYPE_HOST, host.logfile);
  if (host.motdfile)
    XFREE (MTYPE_HOST, host.motdfile);
  if (host.config)
    XFREE (MTYPE_HOST, host.config);
}
#endif

