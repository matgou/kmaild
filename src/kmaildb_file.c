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
 * \file kmaildb_file.c
 * \brief Manipulate flat txt database of email
 * \author Goulin.M
 * \version 0.2
 * \date 20 avril 2011
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <kmaildb_file.h>
#include <ioutils.h>

/**
 * \fn int verif_mailbox_file (char *filename, char *mailbox);
 * \brief return 1 if the mailbox is in the file <=0
 * \param filename the filename of DB mailbox
 * \param mailbox the mailbox to search
 */
int
verif_mailbox_file (char *filename, char *mailbox)
{
  size_t len;
  ssize_t read;
  char *line = NULL;
  FILE *input = fopen (filename, "r+");
  if (input == NULL)
    {
      return -2;
    }
  else
    {
      while ((read = getline (&line, &len, input)) != -1)
	{
	  clean_io (line);
	  if (strcmp (mailbox, line) == 0)
	    {
	      free (line);
	      fclose (input);
	      return 1;
	    }
	  free (line);
	}
    }
  fclose (input);
  return -1;
}
