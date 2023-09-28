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
#include "utils.h"

void usage(void)
{
  fprintf(stderr,
          "Usage: binto [-if infile] [-is seek] [-of outfile] [-os seek] fmt ...\n\n"
          "Produces human-readable output to stdout from a format string and\n"
          "binary input on stdin (similar to the Python struct module)\n"
          "  -if outfile  specifies the infile name (default stdin)\n"
          "  -is seek     specifies how far to seek into the infile (default 0)\n"
          "  -of outfile  specifies the outfile name (default stdout)\n"
          "  -os seek     specifies how far to seek into the outfile (default 0)\n"
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
  int fmt_offset = 1;

  if (argc < 2)
    usage();
  /* Open and lock the outfile */
  if (argc > 2)
    {
      in = parse_options_and_open_infile(argv, argc, "r+", &fmt_offset);
      if (fmt_offset < argc - 1)
        out = parse_options_and_open_outfile(argv, argc, "w+", &fmt_offset);
    }

  init_struct_unpack(&s, in, out, argv[fmt_offset]);
  run_struct_unpack(&s);

  return 0;
}
