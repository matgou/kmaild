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
 * \file smtpd-command.c
 * \brief defini et manipule les commandes SMTP
 * \author Goulin.M
 * \version 0.1
 * \date 11 avril 2011
 *
 */
#include <smtpd-command.h>
#include <smtpd.h>
#include <config.h>
#include <base-rfc5321.h>
#include <stdlib.h>
#include <ioutils.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <plugins.h>
#include <tls.h>
/**
 * \fn void _command_fnct(char *prefix)
 * \brief Return a pointer on the function coresponding to the command prefix 
 * \param prefix the first word of commands send by client
 */
PtrComFonct
_command_fnct (char *prefix)
{
  struct Command *item = commands;
  /* the first item is nothing so we don't need to check it */
  while (item->pNext != NULL)
    {
      item = item->pNext;
      if (strcmp (item->name, prefix) == 0)
	{
	  return item->f;
	}
    }
  return &doNothing;
}

/**
 * \fn void internal_error()
 * \brief display a error message when handler an error
 */
void
internal_error ()
{
  sendTxt ("451 Requested action aborted: error in processing\r\n");
  return;
}

/**
 * \fn void exec_command(char *client_commands)
 * \brief Function to analyse an call a process when client send command
 * \param client_commands command send by the client
 */
void
exec_command (char *client_commands)
{
  int i = 0;
  char prefix[256];
  while (client_commands[i] != '\r' && client_commands[i] != '\n'
	 && client_commands[i] != ' ' && i < 255)
    {
      prefix[i] = tolower (client_commands[i]);
      i++;
    }
  prefix[i] = '\0';

  clean_io (client_commands);
  run_plugin_dispacher (-1, prefix, client_commands);
  if ((*_command_fnct (prefix)) (client_commands) < 0)
    {
      internal_error ();
    }
  run_plugin_dispacher (1, prefix, client_commands);

  return;
}

/**
 * \fn int add_command (char name[255], PtrComFonct fnc)
 * \brief Function to add a command to commands chain
 * \param name name to handler the command
 * \param fnc to the handler function of the command
 */
int
add_command (char name[255], PtrComFonct fnc)
{
  struct Command *c = malloc (sizeof (struct Command));
  c->name = name;
  c->f = fnc;
  c->pNext = NULL;
  struct Command *item = commands;
  while (item->pNext != NULL)
    {
      item = item->pNext;
    }
  item->pNext = c;
  return 1;
}

/**
 * \fn void free_commands()
 * \brief Function to free memory place of commands
 *
 */
void
free_commands ()
{
  struct Command *item = commands;
  struct Command *item2;
  while (item->pNext != NULL)
    {
      item2 = item->pNext;
      free (item);
      item = item2;
    }
  free (item);
}

/**
 * \fn int smtp_proto_violation()
 * \brief display error message when smtp protocole is violated
 */
int
smtp_proto_violation ()
{
  sendTxt ("503 Bad sequence of commands\r\n");
  return 503;
}

/**
 * \fn int doNothing(char *c)
 * \brief an empty function
 * \param c not in use
 */
int
doNothing (char *c)
{
  sendTxt ("502 Command not implemented : %s\r\n", c);
  return 502;
}

/**
 * \fn void display_ext()
 * \brief display all extention 
 */
void
display_ext ()
{
  return;
}

/**
 * \fn char *sendmail_message()
 * \brief put message in qmail queue
 *
 */
char *
sendmail_message ()
{
  FILE *psendmail;
  int returnCode = -1;		/* return code */
  char *cmd_string = malloc (512);
  memset (cmd_string, 0, 512);
  sprintf (cmd_string, "/usr/sbin/sendmail %s", data_email->rcpt_email);
  if ((psendmail = popen (cmd_string, "w")) == NULL)
    {
      perror ("popen");
      returnCode = 1000;
      char *returnMsg;
      returnMsg = malloc (30);
      memset (returnMsg, 0, 30);
      free (cmd_string);
      sprintf (returnMsg, "DQueuing error %d", returnCode);
      return returnMsg;

    }
  /* put data into fd[0] */
  char *datatxt = data_email->data;
  fwrite (datatxt, 1, data_email->count - 4, psendmail);
  pclose (psendmail);
  free (cmd_string);
  return "";
}

/**
 * \fn int init_commands()
 * \brief load all commands from plugins and core 
 *
 */
int
init_commands ()
{
  commands = (struct Command *) malloc (sizeof (struct Command));
  commands->name = "0";
  commands->f = &doNothing;
  commands->pNext = NULL;
  add_command ("quit", &quit);
  add_command ("ehlo", &ehlo);
  say_ehlo = 0;
  add_command ("helo", &ehlo);
  add_command ("mail", &mail_from);
  add_command ("rcpt", &rcpt_to);
  add_command ("data", &data);
  add_command ("vrfy", &vrfy);
  add_command ("expn", &expn);
  add_command ("rset", &rset);
  add_command ("noop", &noop);
  add_command ("starttls",starttls);
  return 1;
}

/**
 * \fn void clearData()
 * \brief clear transaction
 */
void
clearData ()
{
  if (data_email != NULL)
    {
      free (data_email->rcpt_email);
      free (data_email->from_email);
      free (data_email->data);
      free (data_email);
    }
}

/**
 * \fn void initData()
 * \brief init data structure mail_data
 */
void
initData ()
{
  clearData ();
  data_email = malloc (sizeof (struct Data));
  data_email->rcpt_email = (char *) malloc (EMAIL_SIZE * sizeof (char));
  memset (data_email->rcpt_email, 0, EMAIL_SIZE);
  data_email->from_email = (char *) malloc (EMAIL_SIZE * sizeof (char));
  memset (data_email->from_email, 0, EMAIL_SIZE);
  data_email->data = (char *) malloc (DATA_PART_SIZE * sizeof (char));
  data_email->count = 0;
}
