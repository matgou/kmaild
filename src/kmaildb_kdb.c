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
 * \file kmaildb_kdb.c
 * \brief Function to manipulate kyoto_cabinet database of email
 * \author Goulin.M
 * \version 0.2
 * \date 20 avril 2011
 */
#include <kclangc.h>
#include <stdio.h>
#include <time.h>

int
verif_mailbox_kch (char *mailbox_kch_file, char *mailbox)
{
  KCDB *db;
  char *vbuf;
  size_t vsiz;
  int retour = -1;
  db = kcdbnew ();

  /* open the database */
  if (!kcdbopen (db, mailbox_kch_file, KCOREADER))
    {
      return -100;
    }

  vbuf = kcdbget (db, mailbox, strlen (mailbox), &vsiz);
  if (vbuf)
    {
      kcfree (vbuf);
      retour = 1;
    }
  /* delete the database object */
  kcdbdel (db);

  return retour;
}
