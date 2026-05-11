#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        char *cmd_str = argv[i];

        /* Split the command string into tokens */
        char *copy = strdup(cmd_str);
        int n = 0;
        char *tok = strtok(copy, " ");
        while (tok) { n++; tok = strtok(NULL, " "); }

        char **args = malloc((n + 1) * sizeof(char *));
        free(copy);

        copy = strdup(cmd_str);
        tok = strtok(copy, " ");
        for (int j = 0; j < n; j++) {
            args[j] = tok;
            tok = strtok(NULL, " ");
        }
        args[n] = NULL;

        /* Create pipe between command and wc */
        int pipefd[2];
        if (pipe(pipefd) == -1) { perror("pipe"); exit(1); }

        /* Fork command process */
        pid_t pid1 = fork();
        if (pid1 == -1) { perror("fork"); exit(1); }

        if (pid1 == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        }

        /* Fork wc process */
        pid_t pid2 = fork();
        if (pid2 == -1) { perror("fork"); exit(1); }

        if (pid2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            execlp("wc", "wc", "-c", NULL);
            perror("execlp");
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

        free(copy);
        free(args);
    }

    return 0;
}
