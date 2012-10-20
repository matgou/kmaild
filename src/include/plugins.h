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
#ifndef plugins_h
#define plugins_h

/* define de constant*/
#define PRE_WELCOME 0
#define POST_WELCOME 1

#define MAX_COMMAND 2
typedef int (*hook) (char *);
typedef struct kplugins
{
  char *name;
  hook hook_table[MAX_COMMAND];
  struct kplugins *pNext;
  void *handle;
} *plugin_t;

plugin_t plugins_table;
int register_plugins (const char *plugin_name);
int run_pre_sayWelcome ();
int loadAllPlugin (const char *filename);
void freePlugin (plugin_t plugin);
void freeAllPlugin ();
int run_plugin_dispacher (int instant, const char *prefix, char *str);
#endif
