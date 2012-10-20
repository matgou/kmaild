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
 * \file emaildb.c
 * \brief lib pour manipuler et verifier les addresses emails
 * \author Goulin.M
 * \version 0.2
 * \date 20 avril 2011
 *
 */
#include <stdio.h>
#include <emaildb.h>
#include <stdlib.h>
#include <string.h>
#include <config.h>

/* for mailbox verification implementation*/
#include <kmaildb_file.h>
#include <kmaildb_kdb.h>
/**
 * \fn int verify(char *email)
 * \brief verifie qu'un email est dans la base
 */
int
verify (char *email)
{
  char **mailbox;
  mailbox = separate (email);
  if (mailbox == NULL)
    {
      return -1;
    }
  if (verify_domain (mailbox[1]) <= 0)
    {
      freeMailbox (mailbox);
      return EMAIL_DOMAIN_UNKNOW;
    }

  if (verify_mailbox (mailbox[0]) <= 0)
    {
      freeMailbox (mailbox);
      return EMAIL_MAILBOX_UNKNOW;
    }
  freeMailbox (mailbox);
  return EMAIL_OK;
}

void
freeMailbox (char **mailbox)
{
  free (mailbox);
}

/**
 * \fn char ** separate(char *email)
 * \brief separe une addresse email en 2 partie : mailbox et domain
 */
char **
separate (char *email)
{
  char (**mailbox);
  mailbox = (char **) malloc (sizeof (char *) * 2);
  mailbox[0] = email;
  mailbox[1] = strchr (mailbox[0], '@');
  if (mailbox[1] != NULL)
    {
      mailbox[1]++;
      return mailbox;
    }
  return NULL;
}

/**
 * \fn int verify_domain(char *domain)
 * \brief verifie le domain de l'email 
 */
int
verify_domain (char *domain)
{
  struct param_string *actual_dom;
  actual_dom = config->rcpt_host;
  while (actual_dom != NULL)
    {
      if (strcmp (domain, actual_dom->param) == 0)
	{
	  return 1;
	}
      actual_dom = actual_dom->pNext;
    }

  return -1;
}

/**
 * \fn int verify_mailbox(char *mailbox)
 * \brief 
 */
int
verify_mailbox (char *mailbox)
{
  switch (config->mailbox_verif)
    {
    case STATIC_FILE:
      return verif_mailbox_file (config->mailbox_db, mailbox);
      break;
    case KCH:
      return verif_mailbox_kch (config->mailbox_kch_file, mailbox);
      break;
    default:
      return verif_mailbox_file (config->mailbox_db, mailbox);
      break;
    }
  return -1;
}
