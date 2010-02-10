/* spmfilter - mail filtering framework
 * Copyright (C) 2009-2010 Axel Steiner and SpaceNet AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SMF_LOOKUP_H
#define	_SMF_LOOKUP_H

#include "spmfilter.h"

/** expands placeholders in a user querystring
 *
 * \param format format string to use as input
 * \param addr email address to use for replacements
 * \buf pointer to unallocated buffer for expanded format string, needs to
 *      free'd by caller if not required anymore
 *
 * \returns the number of replacements made or -1 in case of error
 */
int expand_query(char *format, char *addr, char **buf);

/** Establish database/ldap connection
 *
 * \returns 0 on success or -1 in case of error
 */
int lookup_connect(void);

/** Destroy database/ldap connection
 *
 * \returns 0 on success or -1 in case of error
 */
int lookup_disconnect(void);

/** Allocates memory for LookupResult_T
 *
 * \returns newly allocated LookupResult_T
 */
LookupResult_T *lookup_result_new(void);

/** Adds a new element on to the end of the list.
 *  The return value is the new start of the list,
 *  which may have changed, so make sure you store the new value.
 *
 * \param *l pointer to LookupResult_T
 * \param *elem_data pointer to LookupElement_T
 */
void lookup_result_add(LookupResult_T *l, LookupElement_T *elem_data);

/** Creates a new element for LookupResult_T
 *
 * \returns pointer to new allocated LookupRow_T
 */
LookupElement_T *lookup_element_new(void);

/** Inserts a new key and value into LookupElement_T
 *  If the key already exists in LookupElement_T its current value is
 *  replaced with the new value.
 *
 * \param *e pointer to LookupElement_T
 * \param *key a key to insert
 * \param *value the value to associate with the key
 */
void lookup_element_add(LookupElement_T *e, char *key, void *value);

#ifdef HAVE_ZDB
/** Connect to sql server
 *
 * \returns 0 on success or -1 in case of error
 */
int sql_connect(void);

void sql_disconnect(void);

/** Check if given user exists in database
 *
 * \param addr email adress of user
 *
 * \return 1 if the user exists, otherwise 0
 */
int sql_user_exists(char *addr);

LookupResult_T *sql_query(const char *q, ...);
#endif

#ifdef HAVE_LDAP
/** Connect to ldap server
 *
 * \returns 0 on success or -1 in case of error
 */
int ldap_connect(void);

void ldap_disconnect(void);

/** Check if given user exists in ldap directory
 *
 * \param addr email adress of user
 *
 * \return 1 if the user exists, otherwise 0
 */
int ldap_user_exists(char *addr);

LookupResult_T *ldap_query(const char *q, ...);
#endif

#endif	/* _SMF_LOOKUP_H */
