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
#ifndef command_h
#define command_h

#define COMMAND_BUFFER_SIZE 1024

struct Command
{
  char *name;
  int (*f) (char *);
  struct Command *pNext;
};

struct Data
{
  char *data;			/*data */
  char *from_email;		/* from email */
  char *rcpt_email;		/* dest email */
  unsigned long int count;	/* data size */
};

typedef int (*PtrComFonct) (char *);

struct Command *commands;
int say_ehlo;
#define EMAIL_SIZE 512
#define DATA_PART_SIZE 8000
struct Data *data_email;

int init_commands ();
char *sendmail_message ();
void exec_command (char *client_commands);
int doNothing (char *client_command);
int smtp_proto_violation ();
void clearData ();
void initData ();
void free_commands ();
void exitTalkEarlier ();
#endif
