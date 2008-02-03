/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      struct.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Exported stuff
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <stdio.h>

typedef enum
{
  end_LITTLE_ENDIAN = 0,
  end_BIG_ENDIAN = 1,
} endianness_t;

typedef struct
{
  endianness_t endian;
  endianness_t host_endian;
  FILE *out, *in;
  const char *fmt;
  int p;
} struct_t;


extern const char *struct_fmt_options;

extern void init_struct_pack(struct_t *s, FILE *out, const char *fmt);
extern void init_struct_unpack(struct_t *s, FILE *in, FILE *out, const char *fmt);
extern void run_struct_pack(struct_t *s, int argc, const char *args[]);
extern void run_struct_unpack(struct_t *s);

#endif /* !__STRUCT_H__ */
