/*
 *
 * Copyright (C) 1998 David Mazieres (dm@uun.org)
 * Copyright (C) 2010 Sardes Project INRIA France
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 */
/**
 * Dirty C++ hacks for comparing structs easily. Not worth the look.
 * Changelog:
 * - 2010: code is now GCC4 compliant.
 */
#ifndef _KEYFUNC_H_
#define _KEYFUNC_H_ 1
#include <string.h>
#include <netinet/in.h>


template<class T> struct unref_t {
  typedef T base_type;
  typedef T unref_type;
  typedef const T &ref_type;
  typedef T &ncref_type;
};
template<class T> struct unref_t<const T> {
  typedef T base_type;
  typedef const T unref_type;
  typedef const T &ref_type;
  typedef T &ncref_type;
};
template<class T> struct unref_t<T &> {
  typedef T base_type;
  typedef T unref_type;
  typedef const T &ref_type;
  typedef T &ncref_type;
};
template<class T> struct unref_t<const T &> {
  typedef T base_type;
  typedef const T unref_type;
  typedef const T &ref_type;
  typedef T &ncref_type;
};
#define CREF(T) unref_t<T>::ref_type
#define UNREF(T) unref_t<T>::unref_type
#define UNCREF(T) unref_t<T>::base_type
#define NCREF(T) unref_t<T>::ncref_type

#define HASHSEED 5381
inline u_int
hash_bytes (const void *_key, int len, u_int seed = HASHSEED)
{
  const u_char *key = (const u_char *) _key;
  const u_char *end;

  for (end = key + len; key < end; key++)
    seed = ((seed << 5) + seed) ^ *key;
  return seed;
}

inline u_int
hash_string (const void *_p, u_int v = HASHSEED)
{
  const char *p = (const char *) _p;
  while (*p)
    v = (33 * v) ^ (unsigned char) *p++;
  return v;
}

inline u_int
hash_rotate (u_int val, u_int rot)
{
  rot %= 8 * sizeof (val);
  return (val << rot) | (val >> (8 * sizeof (val) - rot));
}

class hash_t {
  u_int val;
public:
  hash_t () {}
  hash_t (u_int v) : val (v) {}
  operator u_int() const { return val; }
};

template<class T>
struct compare {
  compare () {}
  int operator() (typename CREF (T) a, typename CREF (T) b) const
    { return a < b ? -1 : b < a; }
};
template<class T>
struct compare<const T> : compare<T>
{
  compare () {}
};

template<class T>
struct equals {
  equals () {}
  bool operator() (typename CREF (T) a, typename CREF (T) b) const
    { return a == b; }
};
template<class T>
struct equals<const T> : equals<T>
{
  equals () {}
};

template<class T>
struct hashfn {
  hashfn () {}
  hash_t operator() (typename CREF (T) a) const
    { return a; }
};
template<class T>
struct hashfn<const T> : hashfn<T>
{
  hashfn () {}
};

template<class T1, class T2>
struct hash2fn {
  hashfn<T1> h1;
  hashfn<T2> h2;
  hash2fn () {}
  hash_t operator() (typename CREF (T1) t1, typename CREF (T2) t2) const
    { return h1 (t1) ^ h2 (t2);  }
};

/* Don't compare pointers by accident */
template<class T> struct compare<T *>;
template<class T> struct hashfn<T *>;

/* Specializations for (char *) */
#define _CHAR_PTR(T, u)						\
template<>							\
struct compare<T> {						\
  compare () {}							\
  int operator() (const u char *a, const u char *b) const	\
    { return strcmp ((char *) a, (char *) b); }			\
};								\
template<>							\
struct equals<T> {						\
  equals () {}							\
  bool operator() (const u char *a, const u char *b) const	\
    { return !strcmp ((char *) a, (char *) b); }		\
};								\
template<>							\
struct hashfn<T> {						\
  hashfn () {}							\
  hash_t operator() (const u char *a) const			\
    { return hash_string (a); }					\
};
#define CHAR_PTR(T, u) _CHAR_PTR(T, u) _CHAR_PTR(T const, u)
CHAR_PTR(char *,)
CHAR_PTR(const char *,)
CHAR_PTR(signed char *, signed)
CHAR_PTR(const signed char *, signed)
CHAR_PTR(unsigned char *, unsigned)
CHAR_PTR(const unsigned char *, unsigned)

template<class R, class V, class K, K V::*key, class F>
class keyfunc_1 {
public:
  const F kf;
  keyfunc_1 () {}
  keyfunc_1 (const F &f) : kf (f) {}
  R operator() (typename CREF (V) a) const
    { return kf (a.*key); }
};

template<class R, class V, class K, K V::*key, class F>
class keyfunc_2 {
public:
  const F kf;
  keyfunc_2 () {}
  keyfunc_2 (const F &f) : kf (f) {}
  R operator() (typename CREF (V) a, typename CREF (V) b) const
    { return kf (a.*key, b.*key); }
};


/*
 * Various comparison operators for timespec
 */
inline bool
operator== (const timespec &a, const timespec &b)
{
  return a.tv_nsec == b.tv_nsec && a.tv_sec == b.tv_sec;
}
inline bool
operator!= (const timespec &a, const timespec &b)
{
  return a.tv_nsec != b.tv_nsec || a.tv_sec != b.tv_sec;
}
inline int
tscmp (const timespec &a, const timespec &b)
{
  if (a.tv_sec < b.tv_sec)
    return -1;
  if (b.tv_sec < a.tv_sec)
    return 1;
  if (a.tv_nsec < b.tv_nsec)
    return -1;
  return b.tv_nsec < a.tv_nsec;
};
inline bool
operator< (const timespec &a, const timespec &b)
{
  return tscmp (a, b) < 0;
}
inline bool
operator<= (const timespec &a, const timespec &b)
{
  return tscmp (a, b) <= 0;
}
inline bool
operator> (const timespec &a, const timespec &b)
{
  return tscmp (a, b) > 0;
}
inline bool
operator>= (const timespec &a, const timespec &b)
{
  return tscmp (a, b) >= 0;
}

inline timespec
operator+ (const timespec &a, const timespec &b)
{
  timespec ts;
  ts.tv_sec = a.tv_sec + b.tv_sec;
  if ((ts.tv_nsec = a.tv_nsec + b.tv_nsec) > 1000000000) {
    ts.tv_nsec -= 1000000000;
    ++ts.tv_sec;
  }
  return ts;
}
inline timespec
operator- (const timespec &a, const timespec &b)
{
  timespec ts;
  ts.tv_sec = a.tv_sec - b.tv_sec;
  if (a.tv_nsec > b.tv_nsec)
    ts.tv_nsec = a.tv_nsec - b.tv_nsec;
  else {
    ts.tv_nsec = a.tv_nsec + 1000000000 - b.tv_nsec;
    --ts.tv_sec;
  }
  return ts;
}
#endif /* !KEYFUNC_H_ */
