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

void pump(int in_fd, int out_fd) {
    char buf[4096];
    ssize_t sz, t;
    for (;;) {
        sz = TEMP_FAILURE_RETRY(read(in_fd, buf, sizeof(buf)));
        if (sz <= 0) return;
        while (sz) {
            t = TEMP_FAILURE_RETRY(write(out_fd, buf, sz));
            if (t <= 0) return;
            sz -= t;
        }
    }
}

int p_std_in[2], p_std_out[2], p_std_err[2];

void *pump_stdin(void *ignore) {
    pump(STDIN_FILENO, p_std_in[1]);
    close(p_std_in[1]);
    return NULL;
}

void *pump_stdout(void *ignore) {
    pump(p_std_out[0], STDOUT_FILENO);
    close(p_std_out[0]);
    return NULL;
}

void *pump_stderr(void *ignore) {
    pump(p_std_err[0], STDERR_FILENO);
    close(p_std_err[0]);
    return NULL;
}

void replace_fd(int fd, int target_fd) {
    if (dup2(fd, target_fd) == -1) err(EXIT_FAILURE, "dup");
    close(fd);
    if (fcntl(target_fd, F_SETFD, fcntl(target_fd, F_GETFD) & ~FD_CLOEXEC) == -1)
        err(EXIT_FAILURE, "replace_fd");
}

int main(int argc, char **argv) {
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

        pthread_t t_stdin;
        pthread_create(&t_stdin, NULL, pump_stdin, NULL);
        pthread_detach(t_stdin);

        pthread_t t_stdout;
        pthread_create(&t_stdout, NULL, pump_stdout, NULL);

        pthread_t t_stderr;
        pthread_create(&t_stderr, NULL, pump_stderr, NULL);

        int status;
        if (TEMP_FAILURE_RETRY(waitpid(pid, &status, 0)) < 0) err(EXIT_FAILURE, "wait");

        pthread_join(t_stdout, NULL);
        pthread_join(t_stderr, NULL);

        if (WIFEXITED(status))
            exit(WEXITSTATUS(status));
        else
            exit(EXIT_FAILURE);
    } else {
        replace_fd(p_std_in[0], STDIN_FILENO);
        replace_fd(p_std_out[1], STDOUT_FILENO);
        replace_fd(p_std_err[1], STDERR_FILENO);

        execv("/system/bin/cmd", argv);
        err(EXIT_FAILURE, "exec");
    }
}
