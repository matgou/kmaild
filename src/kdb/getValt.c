
/* Test Kyotocabinet
 Recuperation d'une valeur a partir d'une clé passé en parametre
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
  KCDB *db;
  char *vbuf;
  size_t vsiz;
  /* Variable for time mesure */
  clock_t start, finish;
  int duration;
  start = clock ();

  db = kcdbnew ();

  /* open the database */
  if (!kcdbopen (db, argv[1], KCOWRITER | KCOCREATE))
    {
      fprintf (stderr, "open error: %s\n", kcecodename (kcdbecode (db)));
      return 100;
    }

  vbuf = kcdbget (db, argv[2], strlen (argv[2]), &vsiz);
  if (vbuf)
    {
      printf ("%s\n", vbuf);
      kcfree (vbuf);
    }
  /* delete the database object */
  kcdbdel (db);

  finish = clock ();
  duration = (finish - start);
  printf ("\nduration : %iclock (%i clock/s) \n", duration, CLOCKS_PER_SEC);

  return 0;
}
