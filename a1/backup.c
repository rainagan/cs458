/*
 * A very simple backup/restore program - use at your own risk ;)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#define CMD_BACKUP 0
#define CMD_RESTORE 1

#define BACKUP_DIRECTORY "/usr/share/backup"
#define FORBIDDEN_DIRECTORY ".ssh"

static
int copyFile(char* src, char* dst)
{
  unsigned int i, len;
  char buffer[2048]; /* 2K ought to be enough for anyone*/
  FILE *source, *dest;
  int c;

  source = fopen(src, "r");
  if (source == NULL) {
    fprintf(stderr, "Failed to open source file\n");
    return -1;
  }

  i = 0;
  c = fgetc(source);
  while (c != EOF) {
    buffer[i]  = (unsigned char) c;
    c = fgetc(source);
    i++;
  }

  len = i;
  fclose(source);
  dest = fopen(dst, "w");
  if (dest == NULL) {
    fprintf(stderr, "Failed to open destination file\n");
    perror("Error");
    return -1;
  }

  for(i = 0; i < len; i++)
    fputc(buffer[i], dest);

  fclose(dest);

  return 0;
}

static
int run_cmd(char * cmd, ... ) {
  va_list vl;
  char * arg[8];
  pid_t pid;
  int i, status;
  i = 1;
  va_start(vl, cmd);
  while(i < 7 && (arg[i++] = va_arg(vl, char*)));
    va_end(vl);
    arg[7] = NULL;
    arg[0] = rindex(cmd, '/');
    if (arg[0] == NULL)
      arg[0] = cmd;
    else
      arg[0]++;

  pid = fork();
  // error
  if (pid < 0) {
    fprintf(stderr, "Fork failed\n");
    return 1;
  }
  if (pid > 0) {
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) < 0)
      return 1;
  }
  else {
    execvp(cmd, arg);
    return 1;
  }
  return 0;
}


static
int restorePermissions(char* target)
{
  pid_t pid;
  int status;
  char *user, *userid, *ptr;
  FILE *file;
  char buffer[64];
  mode_t mode;

  // execute "chown" to assign file ownership to user
  pid = fork();

  // error
  if (pid < 0) {
    fprintf(stderr, "Fork failed\n");
    return -1;
  }

  // parent
  if (pid > 0) {
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) < 0)
      return -1;
  }
  else {

    // child
    // retrieve username
    user = getenv("USER");
    // retrieve corresponding userid
    file = fopen("/etc/passwd", "r");
    if (file == NULL) {
      fprintf(stderr, "Failed to open password file\n");
      return -1;
    }
    userid = NULL;
    while (!feof(file)) {
      if (fgets(buffer, sizeof(buffer), file) != NULL) {
        ptr = strtok(buffer, ":");
        if (strcmp(ptr, user) == 0) {
          strtok(NULL, ":"); // password
          userid = strtok(NULL, ":"); // userid
          ptr = strtok(NULL, ":"); // group
          *ptr = '\0';
          break;
        }
      }
    }

    if (userid != NULL)
      execlp("/bin/chown", "/bin/chown", userid, target, NULL);

    // reached only in case of error
    return -1;
  }

  mode = S_IRUSR | S_IWUSR | S_IEXEC;
  chmod(target, mode);

  return 0;
}

int show_files() {
  run_cmd("ls", "-la", BACKUP_DIRECTORY, NULL);
  return 0;
}

static
void usage(char* parameter)
{
  char newline = '\n';
  char output[128];
  char buffer[128];

  snprintf(buffer, sizeof(buffer),
        "Usage: %.60s backup|restore|ls pathname%c", parameter, newline);

  sprintf(output, buffer);
  printf(output);
}

int main(int argc, char* argv[])
{
  int cmd;
  char *path, *ptr;
  char *forbidden = FORBIDDEN_DIRECTORY;
  char *src, *dst;
  char buffer[200];
  struct stat buf;


  if (strcmp("backup", argv[1]) == 0) {
    if (argc != 3) {
      usage(argv[0]);
      return 1;
    }
    cmd = CMD_BACKUP;
  }
  else if (strcmp("restore", argv[1]) == 0) {
    if (argc != 3) {
      usage(argv[0]);
      return 1;
    }
    cmd = CMD_RESTORE;
  }
  else if (strcmp("ls", argv[1]) == 0) {
    if (argc != 2) {
      usage(argv[0]);
      return 1;
    }
    show_files();
    return 0;
  } else {
    usage(argv[0]);
    return 1;
  }

  path = argv[2];

  // prevent access to forbidden directory
  ptr = realpath(path, NULL);
  if (ptr != NULL && strstr(ptr, forbidden) == ptr) {
    fprintf(stderr, "Not allowed to access target/source %s\n", path);
    return 1;
  }

  // set up paths for copy operation
  if (cmd == CMD_BACKUP) {
      src = path;

    dst = buffer;
    strcpy(dst, BACKUP_DIRECTORY);
    strcat(dst, "/");
    strcat(dst, path);
  }
  else if (cmd == CMD_RESTORE) {
    src = buffer;
    strcpy(src, BACKUP_DIRECTORY);
    strcat(src, "/");
    strcat(src, path);

    dst = path;

    // don't overwrite existing file if we don't own it
    if (stat(dst, &buf) == 0 && buf.st_uid != getuid()) {
      fprintf(stderr, "Not your file: %s\n", dst);
      return 1;
    }
  }

  // perform actual backup/restore operation
    if (copyFile(src, dst) < 0)
    return 1;

  // grant user access to restored file
  if (cmd == CMD_RESTORE) {
    if (restorePermissions(path) < 0)
      return 1;
  }

  return 0;
}

