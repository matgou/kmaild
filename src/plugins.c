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
 * \file plugins.c
 * \brief Lib pour manipuler les plugins
 * \author Goulin.M
 * \version 0.5
 * \date 20 avril 2011
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <plugins.h>
#include <ioutils.h>

/**
 * \fn int register_plugins(const char *plugin_name);
 * \brief enregister un plugin qui se trouve dans plugins/filename.so
 * \param plugin_name : nom (sans le .so du plugins) 
 */
int
register_plugins (const char *plugin_name)
{
  void *handle;
  char *filename = malloc (sizeof (char) * (strlen (plugin_name) + 200));
  strcat (filename, "plugins/");
  strcat (filename, plugin_name);
  strcat (filename, ".so");
  handle = dlopen (filename, RTLD_LAZY);
  if (!handle)
    {
      printf ("unable to load %s : %s\n", filename, dlerror ());
    }
  plugin_t (*plugin_main) ();
  plugin_main = dlsym (handle, "plugin_main");
  if (plugins_table != NULL)
    {
      plugins_table->pNext = (*plugin_main) ();
      plugins_table->pNext->handle = handle;
    }
  else
    {
      plugins_table = (*plugin_main) ();
      plugins_table->handle = handle;
    }
  free (filename);

  return 1;
}

/**
 * \fn void freePlugin(plugin_t plugin)
 * \brief free the plugin passe in param
 */
void
freePlugin (plugin_t plugin)
{
//  free (plugin->name);
//TODO  free (plugin->pre_sayWelcome);
  dlclose (plugin->handle);
  free (plugin);
}

/**
 * \fn void freeAllPlugin()
 * \brief free the plugin_table 
 */
void
freeAllPlugin ()
{
  plugin_t cursor = plugins_table;
  plugin_t next = NULL;
  while (cursor != NULL)
    {
      next = cursor->pNext;
      freePlugin (cursor);
      cursor = next;
    }
}

/**
 * \fn int loadAllPlugin(const char *filename)
 * \brief load all plugin listed in filename
 */
int
loadAllPlugin (const char *filename)
{
  size_t len;
  ssize_t read;
  char *line = NULL;
  FILE *input = fopen (filename, "r+");
  if (input == NULL)
    {
      return 1;
    }
  else
    {
      while ((read = getline (&line, &len, input)) != -1)
	{
	  clean_io (line);
	  register_plugins (line);
	  free (line);
	}
    }
  fclose (input);
  return 1;
}

/**
 * \fn int run_plugin_dispacher(int instant, const char *prefix,char *str)
 * \brief run the correct plugin
 * \param prefix identifier of plugin handler
 * \param str command parameter
 */
int
run_plugin_dispacher (int instant, const char *prefix, char *str)
{
#define REGISTER_(x,y); if (strcmp (fnct, x) == 0) { id=y; }

  char *fnct = malloc (sizeof (char) * 250);
  int retour = 1;
  int id = -1;
  memset (fnct, 0, 250);
  if (instant < 0)
    strcpy (fnct, "pre_");
  else if (instant > 0)
    strcpy (fnct, "post_");

  strcat (fnct, prefix);
  REGISTER_ ("pre_sayWelcome", PRE_WELCOME);
  REGISTER_ ("post_sayWelcome",POST_WELCOME);
  
  if (id >= 0)
    {
      plugin_t cursor = plugins_table;
      while (cursor != NULL)
	{
	  if ((cursor->hook_table[id]) != NULL)
	    {
	      int res = (*(cursor->hook_table[id])) (str);
	      if (res < retour)
		retour = res;
	    }
	  cursor = cursor->pNext;
	}
    }
  free (fnct);
  return retour;
}
