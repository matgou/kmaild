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
 * \file config.c
 * \brief Function to manipulate configuration.
 * \author Goulin.M
 * \version 0.1
 * \date 11 avril 2011
 *
 * One file by parameter
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ioutils.h>

#include <kmaildb_file.h>
#include <kmaildb_kdb.h>

/**
 * \fn freeconfig();
 * \brief clear configuration
 */
void
freeconfig ()
{
  free (config->fdqn);
  free (config->mailbox_kch_file);
  free (config->mailbox_db);

  freeParamString (config->rcpt_host);
  free (config);
}

/**
 * \fn int_loadFromFile(char *fileName, int defaultValue)
 * \brief load an integer from a config file
 * \param fileName the config file name (in CONFIG_DIR directory)
 * \param defaultValue the default value
 * \return the default value if config file doesn't exist the config value else
 */
int
int_loadFromFile (char *fileName, int defaultValue)
{
  int returnVal;
  char *path = malloc (sizeof (CONFIG_DIR));
  strcpy (path, CONFIG_DIR);
  path = (char *) realloc (path, strlen (path) + strlen (fileName) + 1);
  strcat (path, fileName);
  FILE *input = fopen (path, "r+");
  if (input == NULL)
    {
      free (path);
      return defaultValue;
    }
  else
    {
      fscanf (input, "%d", &returnVal);
    }
  if (input != NULL)
    fclose (input);
  free (path);
  return returnVal;
}

/**
 * \fn void freeParamString(struct param_string *)
 * \brief free memory of param_string structure
 */
void
freeParamString (struct param_string *param)
{
  struct param_string *actual_dom;
  struct param_string *next;
  actual_dom = param;
  while (actual_dom != NULL)
    {
      free (actual_dom->param);
      next = actual_dom->pNext;
      free (actual_dom);
      actual_dom = next;
    }

}

/**
 * \fn param_string_loadFromFile(char *fileName, const char* defaultValue)
 * \brief load an param_string_loadFromFile list string from a config file
 * \param fileName the config file name (in CONFIG_DIR directory)
 * \param defaultValue the default value
 * \return the default value if config file doesn't exist the config value else
 */
struct param_string *
param_string_loadFromFile (char *fileName, const char *defaultValue)
{
  char *path = malloc (sizeof (CONFIG_DIR));
  strcpy (path, CONFIG_DIR);

  path = realloc (path, strlen (path) + strlen (fileName) + 1);
  strcat (path, fileName);
  struct param_string *param = malloc (sizeof (struct param_string));
  param->param = NULL;
  param->pNext = NULL;
  size_t len;
  ssize_t read;
  char *line = NULL;
  struct param_string *actual = NULL;
  FILE *input = fopen (path, "r+");
  if (input == NULL)
    {
      param->param = malloc (sizeof (char) * strlen (defaultValue));
      strcpy (param->param, defaultValue);
      param->pNext = NULL;
    }
  else
    {
      actual = param;
      while ((read = getline (&line, &len, input)) != -1)
	{
	  if (param->param == NULL)
	    {
	      param->param = malloc (sizeof (char) * len);
	      param->pNext = NULL;
	      clean_io (line);
	      strcpy (param->param, line);
	    }
	  else if (line != NULL)
	    {
	      struct param_string *next =
		malloc (sizeof (struct param_string));
	      clean_io (line);
	      next->param = line;
	      next->pNext = NULL;
	      actual->pNext = next;
	      actual = next;
	    }
	}
    }
  if (input != NULL)
    fclose (input);
  free (path);
  return param;
}


/**
 * \fn string_loadFromFile(char *fileName, const char* defaultValue)
 * \brief load an char* string from a config file
 * \param fileName the config file name (in CONFIG_DIR directory)
 * \param defaultValue the default value
 * \return the default value if config file doesn't exist the config value else
 */
char *
string_loadFromFile (char *fileName, const char *defaultValue)
{
  char returnVal[255];
  char *path = malloc (sizeof (CONFIG_DIR));
  strcpy (path, CONFIG_DIR);

  path = realloc (path, strlen (path) + strlen (fileName) + 1);
  strcat (path, fileName);
  FILE *input = fopen (path, "r+");
  if (input == NULL)
    {
      strcpy (returnVal, defaultValue);
    }
  else
    {
      fscanf (input, "%s", returnVal);
    }
  char *globalVal = malloc (255);
  strcpy (globalVal, returnVal);
  if (input != NULL)
    fclose (input);
  free (path);
  return globalVal;
}

/**
 * \fn int loadConfig()
 * \brief load all config file in config structure
 * \return 1 in case of success
 */
int
loadConfig ()
{
  config = (struct kmailConfig *) malloc (sizeof (struct kmailConfig));
  /* Syntax config->value = type_loadFromFile(FILENAME,default_value); */
  config->greetingDelay = int_loadFromFile ("smtp_greetingDelay", 0);
  config->fdqn = string_loadFromFile ("fdqn", "localhost.localdomain");
  config->maxsize = int_loadFromFile ("maxsize", 20000000);
  config->rcpt_host =
    param_string_loadFromFile ("rcpthost", "localhost.localdomain");
  config->exitTalkEarlier = int_loadFromFile ("exitTalkEarlier", 0);
  config->mailbox_verif = int_loadFromFile ("DBtype", STATIC_FILE);
  config->mailbox_kch_file = string_loadFromFile ("db_file", "etc/db.kch");
  config->mailbox_db = string_loadFromFile ("db_file", "etc/db.conf");
  return 1;
}
