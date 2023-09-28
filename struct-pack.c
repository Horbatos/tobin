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
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "struct.h"
#include "utils.h"

void usage(void)
{
  fprintf(stderr,
          "Usage: tobin [-of outfile] [-os seek] fmt ...\n\n"
          "Produces binary output to stdout from a format string (similar to the\n"
          "Python struct module)\n\n"
          "  -of outfile  specifies the outfile name (default stdout)\n"
          "  -os seek     specifies how far to seek into the outfile (default 0)\n"
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
  FILE *out;
  int fmt_offset = 1;

  if (argc < 3)
    usage();

  /* Open and lock the outfile */
  out = parse_options_and_open_outfile(argv, argc, "r+", &fmt_offset); 

  init_struct_pack(&s, out, argv[fmt_offset]);
  run_struct_pack(&s, argc - (fmt_offset + 1), &argv[fmt_offset + 1]);

  /* File is closed and unlocked at process exit */

  return 0;
}
