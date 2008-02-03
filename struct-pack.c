/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      struct-pack.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Pack a struct
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>

#include "struct.h"

static void usage(void)
{
  fprintf(stderr,
          "Usage: tobin fmt ...\n\n"
          "Produces binary output to stdout from a format string (similar to the\n"
          "Python struct module)\n"
          "\n"
          "Where format is\n"
          "%s"
          "Example:\n"
          "    tobin \">bH6s\" 4 0x70ff hejsan\n"
          "Produces binary output with a byte, a half-word and a string in big-endian\n"
          "order\n\n"
          "Report bugs to simon.kagstrom@gmail.com\n",
          struct_fmt_options);
  exit(1);
}

int main(int argc, const char *argv[])
{
  struct_t s;
  FILE *out = stdout;

  if (argc < 3)
    usage();

  init_struct_pack(&s, out, argv[1]);
  run_struct_pack(&s, argc - 2, &argv[2]);

  return 0;
}
