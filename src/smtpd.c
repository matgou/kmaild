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
 * \file smtpd.c
 * \brief Programme de demarage du process smtp.
 * \author Goulin.M
 * \version 0.1
 * \date 11 avril 2011
 *
 */
#include <smtpd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <base-rfc5321.h>
#include <ioutils.h>
#include <plugins.h>
/**
 * \fn void sendTxt(const char *format, ...)
 * \brief Function to send data
 * \param format format string 
 */
void
sendTxt (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stdout, format, args);
  fflush (stdout);
  va_end (args);
}

/**
 * \fn void sayWelcome(int code)
 * \brief Function to say the welcome banner
 *
 * \param code to send.
 */
void
sayWelcome (int code)
{
  if (run_plugin_dispacher (-1, "sayWelcome", "") <= 0)
    quit ("pre_helo");
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = config->greetingDelay;
  tv.tv_usec = 0;

  FD_ZERO (&fds);
  FD_SET (fileno (stdin), &fds);

  select (fileno (stdin) + 1, &fds, NULL, NULL, &tv);
  if (FD_ISSET (fileno (stdin), &fds))
    {
      exitTalkEarlier ();
    }
  else
    {
      fprintf (stdout, "%d %s kmaild ESMTP\r\n", code, config->fdqn);
      fflush (stdout);

    }
  if (run_plugin_dispacher (1, "sayWelcome", "") <= 0)
    quit ("pre_helo");
}

/**
 * \fn void mainLoop ()
 * \brief mainLoop of smtp transaction
 */
void
mainLoop ()
{
  char client_commands[COMMAND_BUFFER_SIZE];
  for (;;)
    {
      while (fgets_timeout
	     (client_commands, COMMAND_BUFFER_SIZE - 1, stdin,
	      TIMEOUT_COMMAND) != 0)
	{
	  exec_command (client_commands);
	}
    }
}

/**
 * \fn void segfault_sigaction()
 * \brief handler segfault
 */
void
segfault_sigaction ()
{
  quit ("quit");
}

/**
 * \fn int main ()
 * \brief entry of the smtpd process
 */
int
main ()
{

  struct sigaction sa;

  memset (&sa, 0, sizeof (sigaction));
  sigemptyset (&sa.sa_mask);
  sa.sa_sigaction = segfault_sigaction;
  sa.sa_flags = SA_SIGINFO;
//  sigaction (SIGSEGV, &sa, NULL);

  setvbuf (stdin, NULL, _IONBF, 0);
  loadConfig ();
  init_commands ();
  plugins_table = NULL;
  loadAllPlugin ("etc/plugins");
  sayWelcome (220);
  mainLoop ();
  return 0;
}
