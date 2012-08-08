/* spmfilter - mail filtering framework
 * Copyright (C) 2009-2012 Axel Steiner and SpaceNet AG
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

#include <stdio.h>
#include <assert.h>

#include "../src/smf_envelope.h"
#include "../src/smf_email_address.h"
#include "../src/smf_message.h"
#include "test.h"

int foreach_rc = -1;

static void print_rcpt_func(SMFEmailAddress_T *ea, void *data) {
    char *s = NULL;
    s = smf_email_address_to_string(ea);
    
    if ((strcmp(s,test_addr) != 0) && 
            (strcmp(s,test_addr2) != 0)) {
        foreach_rc = -1;
    } else {
        foreach_rc = 0;
    }
    free(s);
}


int main (int argc, char const *argv[]) {
    SMFEnvelope_T *env = NULL;
    SMFEmailAddress_T *ea = NULL;
    SMFMessage_T *msg = NULL;
    char *s = NULL;

    printf("Start SMFEnvelope_T tests...\n");
    printf("* testing smf_envelope_new()...\t\t\t\t");
    env = smf_envelope_new();
    assert(env);
    printf("passed\n");

    printf("* testing smf_envelope_set_sender()...\t\t\t");
    smf_envelope_set_sender(env,test_addr);
    printf("passed\n");

    printf("* testing smf_envelope_get_sender_string()...\t\t");
    s = smf_envelope_get_sender_string(env);
    if (strcmp(test_addr,s) != 0) {
        printf("failed\n");
        return -1;
    } 
    free(s);
    printf("passed\n");

    printf("* testing smf_envelope_get_sender()...\t\t\t");
    ea = smf_envelope_get_sender(env);
    if ((strcmp(test_name2,smf_email_address_get_name(ea))!=0) || (strcmp(test_email2,smf_email_address_get_email(ea))!=0)) {
        printf("failed\n");
        return -1;
    }
    printf("passed\n");
   
    printf("* testing smf_envelope_add_rcpt()...\t\t\t");
    smf_envelope_add_rcpt(env,test_addr);
    smf_envelope_add_rcpt(env,test_addr2);

    if (env->recipients->size != 2) {
        printf("failed\n");
        return -1;
    } else
        printf("passed\n");
     
    printf("* testing smf_envelope_foreach_rcpt()...\t\t");
    smf_envelope_foreach_rcpt(env, print_rcpt_func, NULL);
    if (foreach_rc != 0) {
        printf("failed\n");
        return -1;
    } else
        printf("passed\n");

    printf("* testing smf_envelope_set_message_file()...\t\t");
    smf_envelope_set_message_file(env,test_path);
    printf("passed\n");

    printf("* testing smf_envelope_get_message_file()...\t\t");
    if (strcmp(test_path,smf_envelope_get_message_file(env)) != 0) {
        printf("failed\n");
        return -1;
    } else
        printf("passed\n");
    

    printf("* testing smf_envelope_set_auth_user()...\t\t");
    smf_envelope_set_auth_user(env,test_auth_user);
    printf("passed\n");

    printf("* testing smf_envelope_get_auth_user()...\t\t");
    if (strcmp(test_auth_user,smf_envelope_get_auth_user(env)) != 0) {
        printf("failed\n");
        return -1;
    } else 
        printf("passed\n");

    printf("* testing smf_envelope_set_auth_pass()...\t\t");
    smf_envelope_set_auth_pass(env,test_auth_pass);
    printf("passed\n");

    printf("* testing smf_envelope_get_auth_pass()...\t\t");
    if (strcmp(test_auth_pass,smf_envelope_get_auth_pass(env)) != 0) {
        printf("failed\n");
        return -1;
    } else
        printf("passed\n");

    printf("* testing smf_envelope_set_nexthop()...\t\t\t");
    smf_envelope_set_nexthop(env,test_nexthop);
    printf("passed\n");

    printf("* testing smf_envelope_get_nexthop()...\t\t\t");
    if (strcmp(test_nexthop,smf_envelope_get_nexthop(env)) != 0) {
        printf("failed\n");
        return -1;
    } else 
        printf("passed\n");

    printf("* testing smf_envelope_set_message()...\t\t\t");
    msg = smf_message_create_skeleton(test_addr, test_addr2, test_string);
    smf_envelope_set_message(env,msg);
    printf("passed\n");

    printf("* testing smf_envelope_get_message()...\t\t\t");
    msg = smf_envelope_get_message(env);
    assert(msg);
    printf("passed\n");

    printf("* testing smf_envelope_free()...\t\t\t");
    smf_envelope_free(env);
    printf("passed\n");
    return 0;
}
