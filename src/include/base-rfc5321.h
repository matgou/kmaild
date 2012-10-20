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

#ifndef base_rfc5321_h
#define base_rfc5321_h
#define TIMEOUT_DATA 30
void internal_error ();
int smtp_proto_violation ();
int doNothing (char *c);
int quit (char *c);
int ehlo (char *c);
int rcpt_to (char *c);
int data (char *c);
int mail_from (char *c);
int vrfy (char *c);
int expn (char *c);
int rset (char *c);
int noop (char *c);
int exittimeout ();
#endif
