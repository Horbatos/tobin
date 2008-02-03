/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      struct-unpack.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Unpack a struct
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>

#include "struct.h"

static void usage(void)
{
  fprintf(stderr,
          "Usage: binto fmt ...\n\n"
          "Produces human-readable output to stdout from a format string and\n"
          "binary input on stdin (similar to the Python struct module)\n"
          "\n"
          "Where format is\n"
          "%s"
          "Example:\n"
          "    cat /tmp/a | binto \">bH6s\"\n"
          "Produces human-readable output with a byte, a half-word and a string in\n"
          "big-endian order\n\n"
          "Report bugs to simon.kagstrom@gmail.com\n",
          struct_fmt_options);
  exit(1);
}

int main(int argc, const char *argv[])
{
  struct_t s;
  FILE *out = stdout;
  FILE *in = stdin;

  if (argc != 2)
    usage();

  init_struct_unpack(&s, in, out, argv[1]);
  run_struct_unpack(&s);

  return 0;
}
