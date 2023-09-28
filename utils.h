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
#ifndef __UTILS_H__
#define __UTILS_H__

#define ERROR(x...) do { fprintf(stderr, x); usage(); } while(0)

extern void usage(void);

FILE *parse_options_and_open_outfile(const char **argv, int argc,
    const char *open_mode, int *offset);
FILE *parse_options_and_open_infile(const char **argv, int argc,
    const char *open_mode, int *offset);

#endif

