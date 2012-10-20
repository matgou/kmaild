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
 Generation d'une base KCH a partir d'une requete MySQL
 Mathieu Goulin Kapable 100me208

 le 06/11/2010
*/

#include <stdio.h>
#include <mysql/mysql.h>
#include <time.h>
#include <kclangc.h>



int
main (int argc, char **argv)
{
  printf ("GenDB: dump des données de mysql pour l'insersion dans KC\n");

  if (argc != 7)
    {
      printf
	("Usage genDB MySQL_USER MySQL_Passwd MySQL_HOST MySQL_DB SQL KCH_FILE\n");
      return 100;
    }
  /* Variable for time mesure */
  clock_t start, finish;
  double duration;
  start = clock ();
  /* Variable for mysql */
  MYSQL mysql;
  mysql_init (&mysql);
  mysql_options (&mysql, MYSQL_READ_DEFAULT_GROUP, "option");
  /* Variables for kyotoCabinet */
  KCDB *db;

  /* MySQL Connect ! */
  if (!mysql_real_connect
      (&mysql, argv[1], argv[2], argv[3], argv[4], 0, NULL, 0))
    {
      fprintf (stderr, "MYSQL : open error\n");
      return 101;
    }

  db = kcdbnew ();
  /* open the database */
  if (!kcdbopen (db, argv[5], KCOWRITER | KCOCREATE))
    {
      fprintf (stderr, "open error: %s\n", kcecodename (kcdbecode (db)));
      mysql_close (&mysql);
      return 102;
    }

  /* DUMP du SQL */

  mysql_query (&mysql, argv[6]);

  MYSQL_RES *result = NULL;
  MYSQL_ROW *row = NULL;

  unsigned int num_champs = 0;
  result = mysql_store_result (&mysql);
  num_champs = mysql_num_fields (result);

  while ((row = mysql_fetch_row (result)))
    {
      unsigned long *lengths;
      lengths = mysql_fetch_lengths (result);
      if (!kcdbset (db, row[0], strlen (row[0]), row[1], strlen (row[1])))
	{
	  fprintf (stderr, "set error: %s\n", kcecodename (kcdbecode (db)));
	  break;
	}
    }

  mysql_free_result (result);
  mysql_close (&mysql);
  /* delete the database object */
  kcdbdel (db);


  finish = clock ();
  duration = (double) (finish - start) / CLOCKS_PER_SEC * 1000;
  printf ("\nduration : %2.8f ms\n", duration);
  return 0;
}
