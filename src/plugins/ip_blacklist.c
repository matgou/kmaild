/******************************************************************************
*   Copyright (C) 2011 Mathieu Goulin <mathieu.goulin@gadz.org>               *
*                                                                             *
*   This program is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Lesser General Public                *
*   License as published by the Free Software Foundation; either              *
*   version 2.1 of the License, or (at your option) any later version.        *
*                                                                             *
*   The GNU C Library is distributed in the hope that it will be useful,      *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
*   Lesser General Public License for more details.                           *
*                                                                             *
*   You should have received a copy of the GNU Lesser General Public          *
*   License along with the GNU C Library; if not, write to the Free           *
*   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA         *
*   02111-1307 USA.                                                           *
******************************************************************************/
/**
 * \file ip_blacklist.c
 * \brief plugin pour blacklister les ips 
 * \author Goulin.M
 * \version 0.5
 * \date 20 avril 2011
 *
 */

#define BLACKLIST_FILE "etc/ip_blacklist"

#include <stdio.h>
#include <stdlib.h>
#include <plugins.h>
#include <string.h>
#include <ctype.h>

/**
 * \fn int clean_io(char *str)
 * \brief clean a string command line to remove r or n for exemple
 * \param str the param to clean
 */
int
clean_io (char *str)
{
  int i;
  int len = strlen (str);
  for (i = 0; i < len; i++)
    {
      if (str[i] == '\n' || str[i] == '\r' || str[i] <= 0)
	{
	  str[i] = '\0';
	}
      str[i] = tolower (str[i]);
    }
  return 1;
}

int
pre_sayWelcome ()
{

  char *ip = getenv ("TCPREMOTEIP");
  if (ip == NULL)
    {
      printf ("No env TCPREMOTEIP, are you in Debug mode \r\n");
      return 1;
    }
  char path[] = BLACKLIST_FILE;
  size_t len;
  ssize_t read;
  char *line = NULL;
  FILE *input = fopen (path, "r+");
  if (input == NULL)
    {
      return 1;
    }
  else
    {
      while ((read = getline (&line, &len, input)) != -1)
	{
	  clean_io (line);
	  if (strcmp (line, ip) == 0)
	    {
	      printf ("220 Helo\r\n");
	      printf ("450 Your ip adress was blacklisted\r\n");
	      free (line);
	      fclose (input);
	      return -1;
	    }
	  free (line);
	}
    }
  fclose (input);
  return 1;
}

plugin_t
plugin_main ()
{
  plugin_t me = malloc (sizeof (struct kplugins));
  memset (me, 0, sizeof (struct kplugins));
  me->name = "IP_Blacklist";
  
  int i = 0;
  for (i = 0; i < MAX_COMMAND; i++)
    me->hook_table[i] = NULL;
  me->hook_table[PRE_WELCOME] = &pre_sayWelcome;
  return me;
}
