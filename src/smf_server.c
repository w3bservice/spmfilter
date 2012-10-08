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

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "smf_settings.h"
#include "smf_trace.h"
#include "smf_server.h"

#define THIS_MODULE "server"

int min_childs = 5;
int max_childs = 25;
int num_procs = 0;
int daemon_exit = 0;
int child[] = {};

void smf_server_sig_handler(int sig) {
    pid_t pid;
    int i;

    switch(sig) {
        case SIGTERM:
            daemon_exit = 1;
            break;
        case SIGCHLD:
            while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
                for (i=0; i < num_procs; i++)
                    child[i] = 0; /* remove process id */
            }
                
            break;
        default:
            break;
    }

    return;
}

void smf_server_init(SMFSettings_T *settings, int sd) {
    pid_t pid;
    FILE *pidfile;
    int i, sigs[] = { SIGHUP, SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOU };
    struct sigaction action_handler;
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    struct sigaction action;

    action_handler.sa_handler = smf_server_sig_handler;
    sigemptyset(&action_handler.sa_mask);
    action_handler.sa_flags = 0;

    if (sigaction(SIGTERM, &action_handler, NULL) < 0) {
        TRACE(TRACE_ERR,"sigaction (SIGTERM) failed: %s",strerror(errno));
        close(sd);
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGCHLD, &action_handler, NULL) < 0) {
        TRACE(TRACE_ERR,"sigaction (SIGCHLD) failed: %s",strerror(errno));
        close(sd);
        exit(EXIT_FAILURE);
    }

    /* switch to background */
    if (settings->foreground == 0) { 
        action.sa_handler = SIG_IGN;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

        for (i = 0; i < sizeof(sigs) / sizeof(int); i++) {
            if (sigaction(sigs[i],&action, NULL) < 0) {
                TRACE(TRACE_ERR,"sigaction failed: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        switch( pid = fork()) {
            case -1:
                TRACE(TRACE_ERR,"fork failed: %s",strerror(errno));
                break;
            case 0:
                break;
            default:
                exit(EXIT_SUCCESS);
                break;
        }

        if (setsid() < 0) {
            TRACE(TRACE_ERR,"setsid failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        switch (pid = fork()) {
            case -1:
                TRACE(TRACE_ERR,"fork failed: %s",strerror(errno));
                exit(EXIT_FAILURE);
                break;
            case 0:
                break;
            default:
                exit(EXIT_SUCCESS);
                break;
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        chdir(settings->queue_dir);
        umask(0);
    }

    /* switch user */
    if ((settings->user != NULL) && (settings->group != NULL)) {
        TRACE(TRACE_DEBUG,"switching to user %s:%s",settings->user,settings->group);
        grp = getgrnam(settings->group);

        if (grp == NULL) {
            TRACE(TRACE_ERR, "could not find group %s", settings->group);
            exit(EXIT_FAILURE);
        }

        pwd = getpwnam(settings->user);
        if (pwd == NULL) {
            TRACE(TRACE_ERR, "could not find user %s", settings->user);
            exit(EXIT_FAILURE);
        }

        if (setgid(grp->gr_gid) != 0) {
            TRACE(TRACE_ERR, "could not set gid to %s", settings->group);
            exit(EXIT_FAILURE);
        }

        if (setuid(pwd->pw_uid) != 0) {
            TRACE(TRACE_ERR, "could not set uid to %s", settings->user);
            exit(EXIT_FAILURE);
        }
    }

    if( settings->pid_file != NULL ) {
        pidfile = fopen(settings->pid_file, "w+" );
        if( pidfile == NULL ) {
            TRACE(TRACE_ERR, "can't open PID file %s: %s",settings->pid_file, strerror(errno));
        } else {
            fprintf(pidfile, "%d\n", getpid());
            fclose(pidfile);
        }
    }

    min_childs = smf_settings_get_min_childs(settings);
}

int smf_server_listen(SMFSettings_T *settings) {
    int sd, reuseaddr;
    int status = -1;
    struct addrinfo hints, *ai, *aptr;
    char *srvname = NULL;

    assert(settings);

    memset(&hints,0,sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    TRACE(TRACE_INFO,"binding to %s:%d",settings->bind_ip,settings->bind_port);

    asprintf(&srvname,"%d",settings->bind_port);
    if ((status == getaddrinfo(settings->bind_ip,srvname,&hints,&ai)) == 0) {
        for (aptr = ai; aptr != NULL; aptr = aptr->ai_next) {
            if ((sd = socket(aptr->ai_family,aptr->ai_socktype, aptr->ai_protocol)) < 0)
                continue;

            reuseaddr = 1;
            setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));

            if (bind(sd,aptr->ai_addr,aptr->ai_addrlen) == 0) {
                if (listen(sd, settings->listen_backlog) >= 0)
                    break;
            }
            close(sd);
        }

        freeaddrinfo(ai);

        if (aptr == NULL) {
            TRACE(TRACE_ERR,"can't listen on port %s: %s", srvname, strerror(errno));
            return -1;
        }
    } else {
        TRACE(TRACE_ERR,"getaddrinfo failed: %s",gai_strerror(status));
        return -1;
    }
    free(srvname);

    return sd;
}

void smf_server_prefork(SMFSettings_T *settings,int sd,
        void (*handle_client_func)(SMFSettings_T *settings,int client)) {
    int i;
    for (i = 0; i < min_childs; i++) {
        switch(child[i] = fork()) {
            case -1:
                TRACE(TRACE_ERR,"fork() failed: %s",strerror(errno));
                break;
            case 0:
                smf_server_accept_handler(settings,sd,handle_client_func);
                exit(EXIT_SUCCESS); /* quit child process */
                break;
            default: /* parent process: go on with accept */
                printf("NUM [%d]\n",++num_procs);
                break;
        }
    }

    close(sd);

    for (;;) {
        pause(); 
        if (daemon_exit)
            break;
    }

    for (i = 0; i < num_procs; i++)
        if (child[i] > 0)
            kill(child[i],SIGTERM);
    while(wait(NULL) > 0)
        ;

    unlink(settings->pid_file);
}

void smf_server_accept_handler(SMFSettings_T *settings, int sd, void (*handle_client_func)(SMFSettings_T *settings,int client)) {
    int client;
    socklen_t slen;
    struct sockaddr_storage sa;


    /* process incoming connection in an infinite loop */
    for (;;) {
        slen = sizeof(sa);

        /* accept new connection */
        if ((client = accept(sd, (struct sockaddr *)&sa, &slen)) < 0) {
            if (daemon_exit)
                break;

            if (errno != EINTR) {
                TRACE(TRACE_ERR,"accept failed: %s",strerror(errno));
            }
            continue;
        }
        handle_client_func(settings,client);
        close(client);
    }

}

