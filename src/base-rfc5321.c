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
 * \file base-rfc5321.c
 * \brief defini et manipule les commandes SMTP de base
 * \author Goulin.M
 * \version 0.1
 * \date 13 avril 2011
 *
 */
#include <smtpd-command.h>
#include <base-rfc5321.h>
#include <smtpd.h>
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <ioutils.h>
#include <emaildb.h>

/** 
 * \fn quit(char *c)
 * \brief send goodbye message and close the quit the process
 */
int
quit (char *c)
{
  clearData ();
  sendTxt ("221 Service closing transmission channel\r\n");
  free_commands ();
  freeconfig ();
  freeAllPlugin ();
  exit (0);
  return 221;
}

/**
 * \fn void exitTalkEarlier()
 * \brief exit when the client don't wait the 220 greeting message
 */
void
exitTalkEarlier ()
{
  sendTxt ("451 you must wait 220 code\r\n");
  if (config->exitTalkEarlier)
    {
      clearData ();
      sendTxt ("451 you must wait 220 code\r\n");
      free_commands ();
      freeconfig ();
      exit (0);
    }
}

/**
 * \fn int exittimeout()
 * \brief send timeout and exit propely
 */
int
exittimeout ()
{
  clearData ();
  sendTxt ("451 timeout\r\n");
  free_commands ();
  freeconfig ();
  exit (0);
  return 451;
}

/**
 * \fn int ehlo(char* c)
 * \brief init the sequence with the client on extended hello
 */
int
ehlo (char *c)
{
  initData ();
  char prefix[255], domain[512];
  sscanf (c, "%s %s", prefix, domain);
  sendTxt ("250-%s\r\n250-PIPELINING\r\n250-STARTTLS\r\n250 8BITMIME\r\n", config->fdqn);
  say_ehlo = 1;
  return 250;
}

/** 
 * \fn int rcpt_to(char *c)
 * \brief handler for rcpt to command
 */
int
rcpt_to (char *c)
{
  if (!say_ehlo || strlen (data_email->from_email) < 3)
    {
      return smtp_proto_violation ();
    }
  char prefix[255];
  int i = 0;
  int j = 0;
  int t = 0;
  char parameters[512];
  while (c[i] != ':' && c[i] != '\n' && c[i] != '\0' && i < 255)
    {
      prefix[i] = tolower (c[i]);
      i++;
    }
  prefix[i] = '\0';
  if (strcmp (prefix, "rcpt to") != 0)
    {
      return doNothing (c);
    }
  i++;
  while (c[i + j] != '\n' && c[i + j] != '\0' && j < 512)
    {
      if (c[i + j] != '<' && c[i + j] != '>')
	{
	  parameters[t] = c[i + j];
	  t++;
	}
      j++;
    }
  parameters[t] = '\0';
  short int v = verify (parameters);
  if (v == EMAIL_OK)
    {
      strcpy (data_email->rcpt_email, parameters);
    }
  else if (v == EMAIL_DOMAIN_UNKNOW)
    {
      sendTxt ("553 relay denied\r\n");
      return 553;
    }
  else if (v == EMAIL_MAILBOX_UNKNOW)
    {
      sendTxt ("550 unknow mailbox\r\n");
      return 550;
    }
  else
    {
      internal_error ();
      return 450;
    }
  sendTxt ("250 OK msg to : %s\r\n", parameters);
  return 250;
}

/**
 * \fn int data(char *c)
 * \brief handler for data command 
 */
int
data (char *c)
{
  if (!say_ehlo || (data_email->from_email)[0] == '\0'
      || (data_email->rcpt_email)[0] == '\0')
    {
      return smtp_proto_violation ();
    }
  char ch[2] = "\0";
  unsigned long int i = 0;
  unsigned long int part = 0;
  int eod = 0;
  char *data_txt = data_email->data;
  sendTxt ("354 Start Input\r\n");
  while (!eod)
    {
      while (fgets_timeout (ch, 2, stdin, TIMEOUT_DATA) != NULL && !eod)
	{
	  *(data_txt + i) = ch[0];
	  if (i != 0 && ((i / DATA_PART_SIZE) - 1) == part)
	    {
	      part++;
	      /*increment size of data area */
	      data_txt =
		(char *) realloc (data_txt,
				  (part +
				   1) * sizeof (char *) * DATA_PART_SIZE);
	    }
	  if (i >= config->maxsize)
	    {
	      sendTxt ("550 message to long\r\n");
	      clearData ();
	      return 550;
	    }
	  /* end of data detection */
	  if (i >= 4)
	    {
	      if (data_txt[i] == '\n' &&
		  data_txt[i - 1] == '\r' &&
		  data_txt[i - 2] == '.' &&
		  data_txt[i - 3] == '\n' && data_txt[i - 4] == '\r')
		{
		  data_txt[i - 4] = '\0';
		  data_email->data = data_txt;
		  data_email->count = i;
		  char *qmailTxt = sendmail_message ();
		  switch (qmailTxt[0])
		    {
		    case '\0':
		      sendTxt ("250 OK msg queuing\r\n");
		      return 250;
		      break;
		    case 'D':
		      sendTxt ("550 %s\r\n", &(qmailTxt[1]));
		      return 550;
		      break;
		    case 'Z':
		      sendTxt ("450 %s\r\n", &(qmailTxt[1]));
		      return 450;
		      break;
		    default:
		      sendTxt ("550 unknow error\r\n");
		      return 550;
		      break;
		    }
		}
	    }
	  i++;
	}
    }
  return 550;
}

/** 
 * \fn int mail_from(char *c)
 * \brief handler for mail from command
 */
int
mail_from (char *c)
{
  if (!say_ehlo)
    {
      return smtp_proto_violation ();
    }
  if (strlen (data_email->from_email) > 0)
    {
      clearData ();
    }
  char prefix[255] = "";
  int i = 0;
  int j = 0;
  int t = 0;
  char parameters[512] = "";
  while (c[i] != ':' && c[i] != '\n' && c[i] != '\0' && i < 255)
    {
      prefix[i] = tolower (c[i]);
      i++;
    }
  prefix[i] = '\0';
  if (strcmp (prefix, "mail from") != 0)
    {
      return doNothing (c);
    }
  i++;
  while (c[i + j] != '\n' && c[i + j] != '\0' && j < 512)
    {
      if (c[i + j] != '<' && c[i + j] != '>')
	{
	  parameters[t] = c[i + j];
	  t++;
	}
      j++;
    }
  parameters[t] = '\0';
  strcpy (data_email->from_email, parameters);
  sendTxt ("250 OK new mail from : %s\r\n", parameters);
  return 250;
}

/**
 * \fn int vrfy(char *c)
 * \brief handler vrfy : disable for security
 */
int
vrfy (char *c)
{
  sendTxt ("252 for security reasons\n\r");
  return 252;
}

/**
 * \fn int expn(char *c)
 * \brief handler expn : disable for security
 */
int
expn (char *c)
{
  sendTxt ("252 for security reasons\n\r");
  return 252;
}

/**
 * \fn int rset(char *c)
 * \brief handler for rset command: reset connexion data
 */
int
rset (char *c)
{
  if (!say_ehlo)
    {
      return smtp_proto_violation ();
    }
  clearData ();			/*free memory buffer */
  sendTxt ("250 OK\r\n");
  return 250;
}

/**
 * \fn int noop(char *c)
 * \brief handler for noop command : no effect just print 250 OK
 */
int
noop (char *c)
{
  sendTxt ("250 OK\r\n");
  return 250;
}
