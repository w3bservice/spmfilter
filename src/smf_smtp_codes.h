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

#ifndef _SMF_SMTP_CODES_H
#define	_SMF_SMTP_CODES_H

/** Free smtp codes */
void smtp_code_free(void);

/** Add smtp return code to list
 *
 * \param code smtp code
 * \param msg smtp return message
 */
void smtp_code_insert(int code, char *msg);

/** Get smtp return code message of given code
 *
 * \param code to look for
 *
 * \returns smtp return message for given code
 */
char *smtp_code_get(int code);

#endif	/* _SMF_SMTP_CODES_H */
