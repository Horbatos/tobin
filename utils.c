/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Some utility functions
 *
 * $Id:$
 *
 ********************************************************************/
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

static int FILE_to_fd(FILE *f)
{
  int fd = fileno(f);

  if (fd < 0)
    ERROR("Cannot get file descriptor\n\n");

  return fd;
}

void lock_file(FILE *f, struct flock *lck)
{
  int fd = FILE_to_fd(f);

  /* See http://www.ecst.csuchico.edu/~beej/guide/ipc/flock.html */
  lck->l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
  lck->l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
  lck->l_start  = 0;        /* Offset from l_whence         */
  lck->l_len    = 0;        /* length, 0 = to EOF           */
  lck->l_pid    = getpid(); /* our PID                      */

  if (fcntl(fd, F_SETLKW, lck) < 0)
    ERROR("Locking file failed: %d\n\n", errno);
}

static FILE *parse_options_and_open_file(FILE *def, const char **argv, int argc,
    const char *file_arg, const char *seek_arg, const char *open_mode, int *offset)
{
  static struct flock lck;
  int fmt_offset = *offset;
  FILE *out = def;

  /* Handle arguments */
  if (strcmp(argv[fmt_offset], file_arg) == 0)
    {
      const char *name = argv[fmt_offset + 1];

      if (argc < fmt_offset + 2)
        ERROR("Too few arguments\n");
      out = fopen(name, open_mode);
      if ( !out )
      {
         ERROR("Cannot open %s\n\n", name);
         return out;
      }
      fmt_offset += 2;
    }
  lock_file(out, &lck);

  if (strcmp(argv[fmt_offset], seek_arg) == 0)
    {
      unsigned long seek_offs;
      const char *arg = argv[fmt_offset + 1];
      char *p;

      if (argc < fmt_offset + 2)
        ERROR("Too few arguments\n");
      /* -s is only valid is not stdout */
      if (out == def)
        ERROR("-s is only valid if the file is not stdout/stdin\n\n");

      seek_offs = strtoul(arg, &p, 0);
      if (p == arg)
        ERROR("Please provide a number to seek to. Cannot parse %s\n\n", arg);

      if (fseek(out, seek_offs, SEEK_SET) < 0)
        ERROR("Cannot seek to %ld in file\n\n", seek_offs);

      fmt_offset += 2;
    }

  *offset = fmt_offset;

  return out;
}


FILE *parse_options_and_open_outfile(const char **argv, int argc,
    const char *open_mode, int *offset)
{
  return parse_options_and_open_file(stdout, argv, argc,
      "-of", "-os", open_mode, offset);
}

FILE *parse_options_and_open_infile(const char **argv, int argc,
    const char *open_mode, int *offset)
{
  return parse_options_and_open_file(stdin, argv, argc,
      "-if", "-is", open_mode, offset);
}
