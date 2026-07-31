/* cmd.c
Copyright (C) 2024 5ec1cff
This file is part of termux-tools.
termux-tools is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.
termux-tools is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public License
along with termux-tools.  If not, see
<https://www.gnu.org/licenses/>.  */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)                                         \
  (__extension__({                                                             \
    long int __result;                                                         \
    do                                                                         \
      __result = (long int)(expression);                                       \
    while (__result == -1L && errno == EINTR);                                 \
    __result;                                                                  \
  }))
#endif

static void pump(int in_fd, int out_fd) {
    char buf[4096];
    for (;;) {
        ssize_t sz = TEMP_FAILURE_RETRY(read(in_fd, buf, sizeof(buf)));
        if (sz <= 0) return;
        char *p = buf;
        while (sz) {
            ssize_t t = TEMP_FAILURE_RETRY(write(out_fd, p, sz));
            if (t <= 0) return;
            sz -= t;
            p += t;
        }
    }
}

static int p_std_in[2], p_std_out[2], p_std_err[2];

static void *pump_stdin(void *ignore) {
    (void)ignore;
    pump(STDIN_FILENO, p_std_in[1]);
    close(p_std_in[1]);
    return NULL;
}

static void *pump_stdout(void *ignore) {
    (void)ignore;
    pump(p_std_out[0], STDOUT_FILENO);
    close(p_std_out[0]);
    return NULL;
}

static void *pump_stderr(void *ignore) {
    (void)ignore;
    pump(p_std_err[0], STDERR_FILENO);
    close(p_std_err[0]);
    return NULL;
}

static void replace_fd(int fd, int target_fd) {
    int flags;
    if (dup2(fd, target_fd) == -1) err(EXIT_FAILURE, "dup");
    close(fd);
    flags = fcntl(target_fd, F_GETFD);
    if (flags == -1) err(EXIT_FAILURE, "replace_fd F_GETFD");
    if (fcntl(target_fd, F_SETFD, flags & ~FD_CLOEXEC) == -1)
        err(EXIT_FAILURE, "replace_fd F_SETFD");
}

int main(int argc, char **argv) {
    (void)argc;
    if (pipe(p_std_in) == -1) err(EXIT_FAILURE, "pipe");
    if (pipe(p_std_out) == -1) err(EXIT_FAILURE, "pipe");
    if (pipe(p_std_err) == -1) err(EXIT_FAILURE, "pipe");

    pid_t pid = fork();

    if (pid < 0) {
        err(EXIT_FAILURE, "fork");
    } else if (pid > 0) {
        close(p_std_in[0]);
        close(p_std_out[1]);
        close(p_std_err[1]);

        signal(SIGPIPE, SIG_IGN);

        pthread_t t_stdin, t_stdout, t_stderr;
        if (pthread_create(&t_stdin, NULL, pump_stdin, NULL) != 0)
            err(EXIT_FAILURE, "pthread_create stdin");
        pthread_detach(t_stdin);
        if (pthread_create(&t_stdout, NULL, pump_stdout, NULL) != 0)
            err(EXIT_FAILURE, "pthread_create stdout");
        if (pthread_create(&t_stderr, NULL, pump_stderr, NULL) != 0)
            err(EXIT_FAILURE, "pthread_create stderr");

        int status;
        if (TEMP_FAILURE_RETRY(waitpid(pid, &status, 0)) < 0) err(EXIT_FAILURE, "wait");

        close(p_std_in[1]);
        if (pthread_join(t_stdout, NULL) != 0)
            err(EXIT_FAILURE, "pthread_join stdout");
        if (pthread_join(t_stderr, NULL) != 0)
            err(EXIT_FAILURE, "pthread_join stderr");

        if (WIFEXITED(status))
            exit(WEXITSTATUS(status));
        else
            exit(EXIT_FAILURE);
    } else {
        close(p_std_in[1]);
        close(p_std_out[0]);
        close(p_std_err[0]);
        replace_fd(p_std_in[0], STDIN_FILENO);
        replace_fd(p_std_out[1], STDOUT_FILENO);
        replace_fd(p_std_err[1], STDERR_FILENO);

        execv("/system/bin/cmd", argv);
        err(EXIT_FAILURE, "exec");
    }
}
