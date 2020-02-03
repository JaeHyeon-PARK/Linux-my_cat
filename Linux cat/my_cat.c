#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAXSIZE 1024

int read_file_func(char *fName);
void three_len_option(char *option, char *fName);
void append_option(char *option, char *fName1, char *fName2);
void combine_option(char *option, char *fName1, char *fName2, char *fName3);
void sort_option(char *fName);

int main(int argc, char *argv[]) {
	int fd, pid, child, status;
	char buffer[1024];
	ssize_t read_byte;

	if (argc == 1) {
		while (1) {
			scanf("%s", buffer);
			printf("%s\n", buffer);
		}
	}
	else if (argc == 2)
		fd = read_file_func(argv[1]);
	else if (!strcmp(argv[1], "-sort")) {
		sort_option(argv[2]);
		return 1;
	}

	pid = fork();
	if (pid == 0) {
		if (argc == 2) {
			while ((read_byte = read(fd, buffer, MAXSIZE)) > 0)
				write(1, buffer, read_byte);
			exit(0);
		}
		else if (argc == 3)
			three_len_option(argv[1], argv[2]);
		else if (argc == 4)
			append_option(argv[1], argv[2], argv[3]);
		else if (argc == 5)
			combine_option(argv[1], argv[2], argv[3], argv[4]);
	}
	else
		child = wait(&status);

	return 0;
}

int read_file_func(char *fName) {
	int return_fd;

	if ((return_fd = open(fName, O_RDONLY)) == -1)
		printf("File Error: [%s] Can't Open!!!\n", fName);

	return return_fd;
}

void three_len_option(char *option, char *fName) {
	FILE *read_fd = NULL;
	int i, str_len, write_fd;
	char buffer[MAXSIZE], *ch;
	ssize_t read_byte;

	if (!strcmp(option, "-b")) {
		i = 1;

		if ((read_fd = fopen(fName, "r")) == 0)
			printf("File Error: [%s] Can't Open!!!\n", fName);
		while (fgets(buffer, MAXSIZE, read_fd) != NULL) {
			if (!strcmp(buffer, "\n")) {
				printf("%s", buffer);
				continue;
			}
			printf("%6d\t%s", i, buffer);
			i++;
		}
	}
	else if (!strcmp(option, "-E")) {
		if ((read_fd = fopen(fName, "r")) == 0)
			printf("File Error: [%s] Can't Open!!!\n", fName);
		while (fgets(buffer, MAXSIZE, read_fd) != NULL) {
			str_len = strlen(buffer);
			if (buffer[str_len - 1] == '\n')
				buffer[str_len - 1] = '$';
			if (buffer[str_len - 1] == '$')
				printf("%s\n", buffer);
			else
				printf("%s", buffer);
		}
	}
	else if (!strcmp(option, "-n")) {
		i = 1;

		if ((read_fd = fopen(fName, "r")) == 0)
			printf("File Error: [%s] Can't Open!!!\n", fName);
		while (fgets(buffer, MAXSIZE, read_fd) != NULL) {
			printf("%6d\t%s", i, buffer);
			i++;
		}
	}
	else if (!strcmp(option, "-wr")) {
		if ((write_fd = open(fName, O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName);
		while (1) {
			fgets(buffer, MAXSIZE, stdin);
			write(write_fd, buffer, strlen(buffer));
		}
	}
}

void append_option(char *option, char *fName1, char *fName2) {
	int read_fd, write_fd;
	char buffer[1024];
	ssize_t read_byte;

	if (!strcmp(option, "-ap")) {
		if ((read_fd = open(fName1, O_RDONLY)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName1);
		if ((write_fd = open(fName2, O_CREAT | O_APPEND | O_WRONLY, 0644)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName2);

		dup2(write_fd, 1);
		while ((read_byte = read(read_fd, buffer, MAXSIZE)) > 0)
			write(1, buffer, read_byte);
	}
}

void combine_option(char *option, char *fName1, char *fName2, char *fName3) {
	int read_fd1, read_fd2, write_fd;
	char buffer[1024];
	ssize_t read_byte;

	if (!strcmp(option, "-comb")) {
		if ((read_fd1 = open(fName1, O_RDONLY)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName1);
		if ((read_fd2 = open(fName2, O_RDONLY)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName2);
		if ((write_fd = open(fName3, O_CREAT | O_APPEND | O_WRONLY, 0644)) == -1)
			printf("File Error: [%s] Can't Open!!!\n", fName3);

		dup2(write_fd, 1);
		while ((read_byte = read(read_fd1, buffer, MAXSIZE)) > 0)
			write(1, buffer, read_byte);
		while ((read_byte = read(read_fd2, buffer, MAXSIZE)) > 0)
			write(1, buffer, read_byte);
	}
}

void sort_option(char *fName) {
	int read_fd;
	int fd[2];
	char buffer[MAXSIZE];
	char *command = "sort";
	ssize_t read_byte;

	pipe(fd);
	if (fork() == 0) {
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
		read_fd = read_file_func(fName);

		while ((read_byte = read(read_fd, buffer, MAXSIZE)) > 0)
			write(1, buffer, read_byte);
		exit(0);
	}
	else {
		close(fd[1]);
		dup2(fd[0], 0);
		close(fd[0]);
		execlp(command, command, NULL);
		perror("pipe");
	}
}
