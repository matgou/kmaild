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
 * \file ioutils.c
 * \brief Lib pour manipuler les I/O 
 * \author Goulin.M
 * \version 0.1
 * \date 20 avril 2011
 *
 */
#include <ioutils.h>
#include <base-rfc5321.h>
#include <stdio.h>
#include <sys/select.h>
#include <string.h>
#include <ctype.h>
/**
 * \fn int fgets_timeout(char *string, int count, FILE * stream, int timeout)
 * \brief quit program if timeout
 * \param string the pointer to put the input string
 * \param count the size to read
 * \param stream the input stream to read
 * \param timeout the duration in second to wait
 */
int
fgets_timeout (char *string, int count, FILE * stream, int timeout)
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_ZERO (&fds);
  FD_SET (fileno (stream), &fds);

  select (fileno (stream) + 1, &fds, NULL, NULL, &tv);
  if (FD_ISSET (fileno (stream), &fds))
    {
      fgets (string, count, stream);
      return 1;
    }
  else
    {
      exittimeout ();
    }
  return -1000;
}

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
