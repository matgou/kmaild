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

/* Test Kyotocabinet
 Insertion d'une valeur a partir d'une clé passés en parametre
 Mathieu Goulin Kapable 100me208

 le 06/11/2010
*/

#include <kclangc.h>
#include <stdio.h>
#include <time.h>

/* main routine */
int
main (int argc, char **argv)
{
  if (argc != 4)
    {
      printf ("Usage setVam KCH_FILE KEY VALUE\n");
      return 100;
    }

  KCDB *db;
  int retour = 0;
  db = kcdbnew ();

  /* open the database */
  if (!kcdbopen (db, argv[1], KCOWRITER | KCOCREATE))
    {
      fprintf (stderr, "open error: %s\n", kcecodename (kcdbecode (db)));
      return 100;
    }

  if (!kcdbset (db, argv[2], strlen (argv[2]), argv[3], strlen (argv[3])))
    {
      fprintf (stderr, "set error: %s\n", kcecodename (kcdbecode (db)));
      retour = 1;
    }

  /* delete the database object */
  kcdbdel (db);

  return retour;
}
