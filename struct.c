/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      struct.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Python-style struct implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "struct.h"

static void set_endianness(struct_t *s)
{
  switch(s->fmt[s->p])
    {
    case '@':
    case '=':
      s->endian = s->host_endian;
      break;
    case '>':
    case '!':
      s->endian = end_BIG_ENDIAN;
      break;
    case '<':
      s->endian = end_LITTLE_ENDIAN;
      break;
    default:
      s->endian = s->host_endian;
      return;
    }
  s->p++;
}

static uint16_t swap16(uint64_t x)
{
  uint16_t out;

  out = ((x & 0xffull) << 8 ) |
    ((x & 0xff00ull) >> 8) << 0;

  return out;
}

static uint32_t swap32(uint64_t x)
{
  uint32_t out;


  out = ( (x & 0xffull) << 24 ) |
    ( ((x & 0xff00ull) >> 8) << 16) |
    ( ((x & 0xff0000ull) >> 16) << 8) |
    ( ((x & 0xff000000ull) >> 24) << 0);

      return out;
}

static uint64_t swap64(uint64_t x)
{
  uint64_t out;

  out = ( ((x & 0xffull) << 56 ) |
          ( ((x & 0xff00ull) >> 8) << 48 ) |
          ( ((x & 0xff0000ull) >> 16) << 40 ) |
          ( ((x & 0xff000000ull) >> 24) << 32 ) |
          ( ((x & 0xff00000000ull) >> 32) << 24 ) |
          ( ((x & 0xff0000000000ull) >> 40)  << 16 ) |
          ( ((x & 0xff000000000000ull) >> 48) << 8 ) |
          ( ((x & 0xff00000000000000ull) >> 56) << 0 ) );

return out;
}

static uint16_t maybe_convert_uint16_t(struct_t *s, uint16_t v)
{
  if ( s->endian != s->host_endian )
    return swap16(v);
  return v;
}
#define maybe_convert_int16_t maybe_convert_uint16_t

static uint32_t maybe_convert_uint32_t(struct_t *s, uint32_t v)
{
  if ( s->endian != s->host_endian )
    return swap32(v);
  return v;
}
#define maybe_convert_int32_t maybe_convert_uint32_t

static uint64_t maybe_convert_uint64_t(struct_t *s, uint64_t v)
{
  if ( s->endian != s->host_endian )
    return swap64(v);
  return v;
}
#define maybe_convert_int64_t maybe_convert_uint64_t

/* No need to byteswap */
#define maybe_convert_int8_t(a,b) (b)
#define maybe_convert_uint8_t(a,b) (b)
#define maybe_convert_float(a,b) (b)
#define maybe_convert_double(a,b) (b)

static int64_t convert_int(struct_t *s, const char *arg)
{
  char *tail;
  int base = 0;
  int64_t out;

  /* Binary */
  if ( strncmp(arg, "0b", 2) == 0 )
    {
      base = 2;
      arg += 2;
    }

  out = strtoll(arg, &tail, base);
  if (errno != 0)
    {
      fprintf(stderr, "Cannot convert %s to an integer\n", arg);
      exit(1);
    }

  return out;
}


static uint64_t convert_uint(struct_t *s, const char *arg)
{
  char *tail;
  int base = 0;
  uint64_t out;

  /* Binary */
  if ( strncmp(arg, "0b", 2) == 0 )
    {
      base = 2;
      arg += 2;
    }

  out = strtoull(arg, &tail, base);
  if (errno != 0)
    {
      fprintf(stderr, "Cannot convert %s to an integer\n", arg);
      exit(1);
    }

  return out;
}


static float convert_float(struct_t *s, const char *arg)
{
  char *tail;
  float out;

  out = (float)strtod(arg, &tail); /* strtof not standard */
  if (errno != 0)
    {
      fprintf(stderr, "Cannot convert %s to a float\n", arg);
      exit(1);
    }

  return out;
}

static float convert_double(struct_t *s, const char *arg)
{
  char *tail;
  double out;

  out = strtod(arg, &tail);
  if (errno != 0)
    {
      fprintf(stderr, "Cannot convert %s to a double\n", arg);
      exit(1);
    }

  return out;
}


/* Macros!! */
#define convert_type(sign, type) do {         \
        type v = (type)sign(s, arg);          \
        v = maybe_convert_##type(s,v);        \
        fwrite(&v, sizeof(type), 1, s->out);  \
      } while(0)
#define convert_signed(type) convert_type(convert_int, type)
#define convert_unsigned(type) convert_type(convert_uint, type)
#define convert_float() convert_type(convert_float, float)
#define convert_double() convert_type(convert_double, double)

static int do_one_pack(struct_t *s, const char *arg)
{
  static int nr = 0;
  static int nr_mult = 1;
  const char cur = s->fmt[s->p];
  int out = 1;

  if (cur >= '0' && cur <= '9')
    {
      nr = (nr * nr_mult) + (cur - '0');
      nr_mult *= 10;
      return 0;
    }
  else
    nr_mult = 1;

  switch (cur)
    {
    case 'x': /* Pad byte */
      {
        int n = ( nr == 0 ) ? 1 : nr; /* 0 will not work */
        int i;

        out = 0;
        for (i = 0; i < n; i++)
          fputc(0, s->out);
      } break;
    case 'c': /* char (string of length 1) */
      fputc(arg[0], s->out);
      break;
    case 's':
    case 'p': /* char [] (string) */
      {
        int len = strlen(arg);

        if (nr == 0)
          fwrite(arg, 1, 1, s->out);
        else
          {
            if (len > nr)
              fwrite(arg, nr, 1, s->out);
            else /* Pad with zeros */
              {
                int i;

                fwrite(arg, len, 1, s->out);
                for (i = 0; i < nr - len; i++)
                  fputc(0, s->out);
              }
          }
        nr = 1;
      }break;
    case 'b': /* int8_t */
      convert_signed(int8_t); break;
    case 'B': /* uint8_t */
      convert_unsigned(uint8_t); break;
    case 'h': /* int16_t */
      convert_signed(int16_t); break;
    case 'H': /* uint16_t */
      convert_unsigned(int16_t); break;
    case 'i':
    case 'l': /* int32_t */
      convert_signed(int32_t); break;
    case 'I':
    case 'L': /* uint32_t */
      convert_unsigned(int32_t); break;
    case 'q': /* int64_t */
      convert_signed(int64_t); break;
    case 'Q': /* uint64_t */
      convert_unsigned(uint64_t); break;
    case 'P':
      if (sizeof(void*) == 2)
        convert_unsigned(uint16_t); /* OK, maybe not that likely... */
      else if (sizeof(void*) == 4)
        convert_unsigned(uint32_t);
      else if (sizeof(void*) == 8)
        convert_unsigned(uint64_t);
      break;
    case 'f': /* float */
      convert_float(); break;
    case 'd': /* double */
      convert_double(); break;
    default:
      fprintf(stderr, "Unknown format %c\n", cur);
      exit(1);
    }

  return out;
}

static void get_bytes(struct_t *s, void *p, size_t size, FILE *f)
{
  size_t out = fread(p, size, 1, f);

  if (out != 1)
    {
      fprintf(stderr, "File ends prematurely while parsing %c\n",
              s->fmt[s->p]);
      exit(1);
    }
}

/* Macros again!! */
#define convert_type_out(type, fmt, tc) do {    \
        type v;                                 \
        get_bytes(s, &v, sizeof(type), s->in);  \
        v = maybe_convert_##type(s,v);          \
        fprintf(s->out, fmt" ", (tc)v);         \
      } while(0)
#define signed_out(type) convert_type_out(type, "%lld", long long int)
#define unsigned_out8(type) convert_type_out(type, "%u", uint8_t)
#define unsigned_out16(type) convert_type_out(type, "%u", uint16_t)
#define unsigned_out32(type) convert_type_out(type, "%u", uint32_t)
#define unsigned_out64(type) convert_type_out(type, "%llu", uint64_t)
#define float_out() convert_type_out(float, "%f", float)
#define double_out() convert_type_out(double, "%f", double)

static int do_one_unpack(struct_t *s)
{
  static int nr = 0;
  static int nr_mult = 1;
  const char cur = s->fmt[s->p];
  int out = 1;

  if (cur >= '0' && cur <= '9')
    {
      nr = (nr * nr_mult) + (cur - '0');
      nr_mult *= 10;
      return 0;
    }
  else
    nr_mult = 1;

  switch (cur)
    {
    case 'x': /* Pad byte */
      {
        uint8_t dummy;
        int n = ( nr == 0 ) ? 1 : nr;
        int i;

        out = 0;
        for (i = 0; i < n; i++)
          get_bytes(s, &dummy, sizeof(uint8_t), s->in);
      }
      break;
    case 'c': /* char (string of length 1) */
      fputc( fgetc(s->in) , s->out );
      break;
    case 's':
    case 'p': /* char [] (string) */
      {
        char *buf;
        int n = ( nr == 0 ) ? 1 : nr;

        buf = malloc(n);
        if (!buf)
          {
            perror("malloc");
            exit(1);
          }
        get_bytes(s, buf, n, s->in);
        fwrite(buf, 1, n, s->out);
        free(buf);
      } break;
    case 'b': /* int8_t */
      signed_out(int8_t); break;
    case 'B': /* uint8_t */
      unsigned_out8(uint8_t); break;
    case 'h': /* int16_t */
      signed_out(int16_t); break;
    case 'H': /* uint16_t */
      unsigned_out16(int16_t); break;
    case 'i':
    case 'l': /* int32_t */
      signed_out(int32_t); break;
    case 'I':
    case 'L': /* uint32_t */
      unsigned_out32(int32_t); break;
    case 'q': /* int64_t */
      signed_out(int64_t); break;
    case 'Q': /* uint64_t */
      unsigned_out64(uint64_t); break;
    case 'P':
      if (sizeof(void*) == 2)
        unsigned_out16(uint16_t); /* OK, maybe not that likely... */
      else if (sizeof(void*) == 4)
        unsigned_out32(uint32_t);
      else if (sizeof(void*) == 8)
        unsigned_out64(uint64_t);
      break;
    case 'f': /* float */
      float_out(); break;
    case 'd': /* double */
      double_out(); break;
    default:
      fprintf(stderr, "Unknown format %c\n", cur);
      exit(1);
    }

  return out;
}


void run_struct_pack(struct_t *s, int argc, const char *args[])
{
  int first = s->p;
  int count = 0;

  for ( ;
        s->fmt[s->p] != '\0';
        s->p++)
    {
      if (count > argc)
        {
          fprintf(stderr, "Too few arguments for format\n");
          exit(1);
        }
      count += do_one_pack(s, args[count] );
    }

  if (s->p - first  < argc)
    {
      fprintf(stderr, "Too many arguments for format\n");
      exit(1);
    }
}


void run_struct_unpack(struct_t *s)
{
  int count = 0;

  for ( ;
        s->fmt[s->p] != '\0';
        s->p++)
    {
      count += do_one_unpack(s);
    }
  fprintf(s->out, "\n");
}


/* from http://unixpapa.com/incnote/byteorder.html */
static int am_big_endian(void)
{
  uint32_t one= 1;

  return !( *((char *)(&one)) );
}

static void init_struct(struct_t *s, FILE *in, FILE *out, const char *fmt)
{
  memset(s, 0, sizeof(struct_t));
  s->fmt = fmt;
  s->p = 0;
  s->out = out;
  s->in = in;

  if (am_big_endian())
    s->host_endian = end_BIG_ENDIAN;
  else
    s->host_endian = end_LITTLE_ENDIAN;

  set_endianness(s);
}

void init_struct_pack(struct_t *s, FILE *out, const char *fmt)
{
  init_struct(s, NULL, out, fmt);
}


void init_struct_unpack(struct_t *s, FILE *in, FILE *out, const char *fmt)
{
  init_struct(s, in, out, fmt);
}

const char *struct_fmt_options = "   x  pad byte     q   int64_t\n"
"   c      char     Q  uint64_t\n"
"   b    int8_t     f     float\n"
"   B   uint8_t     d    double\n"
"   h   int16_t   [nn]s  string (without NULL-termination)\n"
"   H  uint16_t   [nn]p  string (without NULL-termination)\n"
"   i   int32_t     P    void *\n"
"   I  uint32_t\n"
"   l   int32_t\n"
"   L  uint32_t\n"
"\n"
"[nn] is an optional number which specifies the number of bytes to write\n"
"for strings. Numbers use native byteorder by default. Alternatively, the\n"
"first character in the format string can be\n"
"\n"
"   @|= native byteorder\n"
"   <   little endian\n"
"   >!  big endian (network)\n"
"\n";

