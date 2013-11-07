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
#ifndef _CALLBACK_H_INCLUDED_
#define _CALLBACK_H_INCLUDED_ 1

#include <stdio.h>
#define TIMELEFT_DEFAULT , 0


template<class R, class B1 = void, class B2 = void, class B3 = void> class callback;

template<class R>
class callback<R> {
public:
  virtual int getcolor () = 0;
  virtual int getprio () = 0;
  virtual int get_timeleft () = 0;
  virtual void setFlagAutoFree(bool flag) = 0;
  virtual bool getFlagAutoFree() = 0;
  virtual void* getfaddr () = 0;
  virtual R operator() () = 0;
  virtual ~callback () {}
};


template<class R>
class callback_0_0
  : public callback<R> {
  typedef R (*cb_t) ();
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_0_0 (cb_t ff, int _c, int _p
           , int _timeleft
  )
    : f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
    bool getFlagAutoFree () { return AutoFree; }

  void* getfaddr (){ return (void *)f; }

  R operator() ()
    { return f (); }
};

template<class R>
static inline callback_0_0<R> *
wrap (R (*f) ())
{
  return new (callback_0_0<R>) (f, 0, 0  TIMELEFT_DEFAULT);
}
template<class R>
static inline callback_0_0<R> *
cwrap (R (*f) (), int c)
{
  return new (callback_0_0<R>) (f, c, 0  TIMELEFT_DEFAULT);
}
template<class R>
static inline callback_0_0<R> *
cpwrap (R (*f) (), int c, int prio)
{
  return new (callback_0_0<R>) (f, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R>
class callback_c_0_0
  : public callback<R> {
  typedef R (C::*cb_t) ();
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_c_0_0 (const P &cc, cb_t ff, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (); }
};

template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
wrap (C *p, R (C::*f) ())
{
  return new (callback_c_0_0<C *, C, R>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
cwrap (C *p, R (C::*f) (), int c)
{
  return new (callback_c_0_0<C *, C, R>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
cpwrap (C *p, R (C::*f) (), int c, int prio)
{
  return new (callback_c_0_0<C *, C, R>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R>
static inline callback_c_0_0<C, C, R> *
wrap (const C &p, R (C::*f) ())
{
  return new (callback_c_0_0<C, C, R>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R>
static inline callback_c_0_0<C, C, R> *
cwrap (const C &p, R (C::*f) (), int c)
{
  return new (callback_c_0_0<C, C, R>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R>
static inline callback_c_0_0<C, C, R> *
cpwrap (const C &p, R (C::*f) (), int c, int prio)
{
  return new (callback_c_0_0<C, C, R>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class A1>
class callback_0_1
  : public callback<R> {
  typedef R (*cb_t) (A1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_0_1 (cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() ()
    { return f (a1); }
};

template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
wrap (R (*f) (A1), const AA1 &a1)
{
  return new (callback_0_1<R, A1>) (f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
cwrap (R (*f) (A1), const AA1 &a1, int c)
{
  return new (callback_0_1<R, A1>) (f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
cpwrap (R (*f) (A1), const AA1 &a1, int c, int prio)
{
  return new (callback_0_1<R, A1>) (f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class A1>
class callback_c_0_1
  : public callback<R> {
  typedef R (C::*cb_t) (A1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_c_0_1 (const P &cc, cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (a1); }
};

template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
wrap (C *p, R (C::*f) (A1), const AA1 &a1)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
cwrap (C *p, R (C::*f) (A1), const AA1 &a1, int c)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
cpwrap (C *p, R (C::*f) (A1), const AA1 &a1, int c, int prio)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
wrap (const C &p, R (C::*f) (A1), const AA1 &a1)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
cwrap (const C &p, R (C::*f) (A1), const AA1 &a1, int c)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
cpwrap (const C &p, R (C::*f) (A1), const AA1 &a1, int c, int prio)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class A1, class A2>
class callback_0_2
  : public callback<R> {
  typedef R (*cb_t) (A1, A2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_0_2 (cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() ()
    { return f (a1, a2); }
};

template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
wrap (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2)
{ 
  callback_0_2<R, A1, A2> *cb = new (callback_0_2<R, A1, A2>) (f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
  return cb;
}

unsigned int get_current_proc();
template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
cwrap (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c)
{
  callback_0_2<R, A1, A2> *cb = new (callback_0_2<R, A1, A2>) (f, a1, a2, c, 0  TIMELEFT_DEFAULT);
  //printf("[%d] alloced here : %p\n", get_current_proc(), cb);
  return cb;
}
template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
cpwrap (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_0_2<R, A1, A2>) (f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class A1, class A2>
class callback_c_0_2
  : public callback<R> {
  typedef R (C::*cb_t) (A1, A2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_c_0_2 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (a1, a2); }
};

template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
wrap (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
cwrap (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
cpwrap (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
wrap (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
cwrap (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
cpwrap (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class A1, class A2, class A3>
class callback_0_3
  : public callback<R> {
  typedef R (*cb_t) (A1, A2, A3);
public:
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  callback_0_3 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() ()
    { return f (a1, a2, a3); }
};

template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
wrap (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
cwrap (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
cpwrap (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class A1, class A2, class A3>
class callback_c_0_3
  : public callback<R> {
  typedef R (C::*cb_t) (A1, A2, A3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_c_0_3 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (a1, a2, a3); }
};

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
wrap (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
cwrap (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
cpwrap (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
wrap (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
cwrap (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class A1, class A2, class A3, class A4>
class callback_0_4
  : public callback<R> {
  typedef R (*cb_t) (A1, A2, A3, A4);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_0_4 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() ()
    { return f (a1, a2, a3, a4); }
};

template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
wrap (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  callback_0_4<R, A1, A2, A3, A4> * cb = new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
  printf("[%d] alloced here : %p\n");
  return cb;
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
cwrap (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
cpwrap (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class A1, class A2, class A3, class A4>
class callback_c_0_4
  : public callback<R> {
  typedef R (C::*cb_t) (A1, A2, A3, A4);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_c_0_4 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (a1, a2, a3, a4); }
};

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class A1, class A2, class A3, class A4, class A5>
class callback_0_5
  : public callback<R> {
  typedef R (*cb_t) (A1, A2, A3, A4, A5);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_0_5 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() ()
    { return f (a1, a2, a3, a4, a5); }
};

template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
wrap (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
cwrap (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
cpwrap (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class A1, class A2, class A3, class A4, class A5>
class callback_c_0_5
  : public callback<R> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, A5);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_c_0_5 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() ()
    { return ((*c).*f) (a1, a2, a3, a4, a5); }
};

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1>
class callback<R, B1> {
public:
  virtual int getcolor () = 0;
  virtual int getprio () = 0;
  virtual R operator() (B1) = 0;
  virtual ~callback () {}
};


template<class R, class B1>
class callback_1_0
  : public callback<R, B1> {
  typedef R (*cb_t) (B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_1_0 (cb_t ff, int _c, int _p
           , int _timeleft
  )
    : f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (b1); }
};

template<class R, class B1>
static inline callback_1_0<R, B1> *
wrap (R (*f) (B1))
{
  return new (callback_1_0<R, B1>) (f, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1>
static inline callback_1_0<R, B1> *
cwrap (R (*f) (B1), int c)
{
  return new (callback_1_0<R, B1>) (f, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1>
static inline callback_1_0<R, B1> *
cpwrap (R (*f) (B1), int c, int prio)
{
  return new (callback_1_0<R, B1>) (f, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1>
class callback_c_1_0
  : public callback<R, B1> {
  typedef R (C::*cb_t) (B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_c_1_0 (const P &cc, cb_t ff, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1)
    { return ((*c).*f) (b1); }
};

template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
wrap (C *p, R (C::*f) (B1))
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
cwrap (C *p, R (C::*f) (B1), int c)
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
cpwrap (C *p, R (C::*f) (B1), int c, int prio)
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
wrap (const C &p, R (C::*f) (B1))
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
cwrap (const C &p, R (C::*f) (B1), int c)
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
cpwrap (const C &p, R (C::*f) (B1), int c, int prio)
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class A1>
class callback_1_1
  : public callback<R, B1> {
  typedef R (*cb_t) (A1, B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_1_1 (cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (a1, b1); }
};

template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
wrap (R (*f) (A1, B1), const AA1 &a1)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
cwrap (R (*f) (A1, B1), const AA1 &a1, int c)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
cpwrap (R (*f) (A1, B1), const AA1 &a1, int c, int prio)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class A1>
class callback_c_1_1
  : public callback<R, B1> {
  typedef R (C::*cb_t) (A1, B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_c_1_1 (const P &cc, cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return ((*c).*f) (a1, b1); }
};

template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
wrap (C *p, R (C::*f) (A1, B1), const AA1 &a1)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
cwrap (C *p, R (C::*f) (A1, B1), const AA1 &a1, int c)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
cpwrap (C *p, R (C::*f) (A1, B1), const AA1 &a1, int c, int prio)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
wrap (const C &p, R (C::*f) (A1, B1), const AA1 &a1)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
cwrap (const C &p, R (C::*f) (A1, B1), const AA1 &a1, int c)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
cpwrap (const C &p, R (C::*f) (A1, B1), const AA1 &a1, int c, int prio)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class A1, class A2>
class callback_1_2
  : public callback<R, B1> {
  typedef R (*cb_t) (A1, A2, B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_1_2 (cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (a1, a2, b1); }
};

template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
wrap (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
cwrap (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
cpwrap (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class A1, class A2>
class callback_c_1_2
  : public callback<R, B1> {
  typedef R (C::*cb_t) (A1, A2, B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_c_1_2 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1)
    { return ((*c).*f) (a1, a2, b1); }
};

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
wrap (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
cwrap (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
cpwrap (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
wrap (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
cwrap (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
cpwrap (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class A1, class A2, class A3>
class callback_1_3
  : public callback<R, B1> {
  typedef R (*cb_t) (A1, A2, A3, B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_1_3 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (a1, a2, a3, b1); }
};

template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
wrap (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
cwrap (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
cpwrap (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class A1, class A2, class A3>
class callback_c_1_3
  : public callback<R, B1> {
  typedef R (C::*cb_t) (A1, A2, A3, B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_c_1_3 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1)
    { return ((*c).*f) (a1, a2, a3, b1); }
};

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
wrap (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
cwrap (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
wrap (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class A1, class A2, class A3, class A4>
class callback_1_4
  : public callback<R, B1> {
  typedef R (*cb_t) (A1, A2, A3, A4, B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_1_4 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (a1, a2, a3, a4, b1); }
};

template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
wrap (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
cwrap (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
cpwrap (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class A1, class A2, class A3, class A4>
class callback_c_1_4
  : public callback<R, B1> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_c_1_4 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1)
    { return ((*c).*f) (a1, a2, a3, a4, b1); }
};

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class A1, class A2, class A3, class A4, class A5>
class callback_1_5
  : public callback<R, B1> {
  typedef R (*cb_t) (A1, A2, A3, A4, A5, B1);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_1_5 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1)
    { return f (a1, a2, a3, a4, a5, b1); }
};

template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
wrap (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
cwrap (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
cpwrap (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class A1, class A2, class A3, class A4, class A5>
class callback_c_1_5
  : public callback<R, B1> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, A5, B1);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_c_1_5 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1)
    { return ((*c).*f) (a1, a2, a3, a4, a5, b1); }
};

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2>
class callback<R, B1, B2> {
public:
  virtual int getcolor () = 0;
  virtual int getprio () = 0;
  virtual R operator() (B1, B2) = 0;
  virtual ~callback () {}
};


template<class R, class B1, class B2>
class callback_2_0
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_2_0 (cb_t ff, int _c, int _p
           , int _timeleft
  )
    : f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (b1, b2); }
};

template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
wrap (R (*f) (B1, B2))
{
  return new (callback_2_0<R, B1, B2>) (f, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
cwrap (R (*f) (B1, B2), int c)
{
  return new (callback_2_0<R, B1, B2>) (f, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
cpwrap (R (*f) (B1, B2), int c, int prio)
{
  return new (callback_2_0<R, B1, B2>) (f, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2>
class callback_c_2_0
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_c_2_0 (const P &cc, cb_t ff, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (b1, b2); }
};

template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
wrap (C *p, R (C::*f) (B1, B2))
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
cwrap (C *p, R (C::*f) (B1, B2), int c)
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
cpwrap (C *p, R (C::*f) (B1, B2), int c, int prio)
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
wrap (const C &p, R (C::*f) (B1, B2))
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
cwrap (const C &p, R (C::*f) (B1, B2), int c)
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
cpwrap (const C &p, R (C::*f) (B1, B2), int c, int prio)
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class A1>
class callback_2_1
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (A1, B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_2_1 (cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (a1, b1, b2); }
};

template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
wrap (R (*f) (A1, B1, B2), const AA1 &a1)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
cwrap (R (*f) (A1, B1, B2), const AA1 &a1, int c)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
cpwrap (R (*f) (A1, B1, B2), const AA1 &a1, int c, int prio)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class A1>
class callback_c_2_1
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (A1, B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_c_2_1 (const P &cc, cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (a1, b1, b2); }
};

template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
wrap (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
cwrap (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
cpwrap (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int prio)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
wrap (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
cwrap (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
cpwrap (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int prio)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class A1, class A2>
class callback_2_2
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (A1, A2, B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_2_2 (cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (a1, a2, b1, b2); }
};

template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
wrap (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
cwrap (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
cpwrap (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class A1, class A2>
class callback_c_2_2
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (A1, A2, B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_c_2_2 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (a1, a2, b1, b2); }
};

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
wrap (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
cwrap (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
cpwrap (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
wrap (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
cwrap (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
cpwrap (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class A1, class A2, class A3>
class callback_2_3
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (A1, A2, A3, B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_2_3 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (a1, a2, a3, b1, b2); }
};

template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
wrap (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
cwrap (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
cpwrap (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class A1, class A2, class A3>
class callback_c_2_3
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (A1, A2, A3, B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_c_2_3 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (a1, a2, a3, b1, b2); }
};

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
wrap (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
cwrap (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
wrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class A1, class A2, class A3, class A4>
class callback_2_4
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (A1, A2, A3, A4, B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_2_4 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (a1, a2, a3, a4, b1, b2); }
};

template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
wrap (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
cwrap (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
cpwrap (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class A1, class A2, class A3, class A4>
class callback_c_2_4
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_c_2_4 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (a1, a2, a3, a4, b1, b2); }
};

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class A1, class A2, class A3, class A4, class A5>
class callback_2_5
  : public callback<R, B1, B2> {
  typedef R (*cb_t) (A1, A2, A3, A4, A5, B1, B2);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_2_5 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2)
    { return f (a1, a2, a3, a4, a5, b1, b2); }
};

template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
wrap (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
cwrap (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class A1, class A2, class A3, class A4, class A5>
class callback_c_2_5
  : public callback<R, B1, B2> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, A5, B1, B2);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_c_2_5 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2)
    { return ((*c).*f) (a1, a2, a3, a4, a5, b1, b2); }
};

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3>
class callback {
public:
  virtual int getcolor () = 0;
  virtual int getprio () = 0;
  virtual void* getfaddr () = 0;
  virtual R operator() (B1, B2, B3) = 0;
  virtual ~callback () {}
};


template<class R, class B1, class B2, class B3>
class callback_3_0
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_3_0 (cb_t ff, int _c, int _p
           , int _timeleft
  )
    : f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (b1, b2, b3); }
};

template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
wrap (R (*f) (B1, B2, B3))
{
  return new (callback_3_0<R, B1, B2, B3>) (f, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
cwrap (R (*f) (B1, B2, B3), int c)
{
  return new (callback_3_0<R, B1, B2, B3>) (f, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
cpwrap (R (*f) (B1, B2, B3), int c, int prio)
{
  return new (callback_3_0<R, B1, B2, B3>) (f, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3>
class callback_c_3_0
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
public:
  callback_c_3_0 (const P &cc, cb_t ff, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
wrap (C *p, R (C::*f) (B1, B2, B3))
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
cwrap (C *p, R (C::*f) (B1, B2, B3), int c)
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
cpwrap (C *p, R (C::*f) (B1, B2, B3), int c, int prio)
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
wrap (const C &p, R (C::*f) (B1, B2, B3))
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
cwrap (const C &p, R (C::*f) (B1, B2, B3), int c)
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
cpwrap (const C &p, R (C::*f) (B1, B2, B3), int c, int prio)
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3, class A1>
class callback_3_1
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (A1, B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_3_1 (cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (a1, b1, b2, b3); }
};

template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
wrap (R (*f) (A1, B1, B2, B3), const AA1 &a1)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
cwrap (R (*f) (A1, B1, B2, B3), const AA1 &a1, int c)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
cpwrap (R (*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3, class A1>
class callback_c_3_1
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (A1, B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
public:
  callback_c_3_1 (const P &cc, cb_t ff, const A1 &aa1, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (a1, b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
wrap (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
cwrap (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
cpwrap (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
wrap (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
cwrap (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
cpwrap (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3, class A1, class A2>
class callback_3_2
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (A1, A2, B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_3_2 (cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (a1, a2, b1, b2, b3); }
};

template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
wrap (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
cwrap (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
cpwrap (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3, class A1, class A2>
class callback_c_3_2
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (A1, A2, B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
public:
  callback_c_3_2 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (a1, a2, b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
wrap (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
cwrap (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
cpwrap (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
wrap (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
cwrap (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
cpwrap (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3, class A1, class A2, class A3>
class callback_3_3
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (A1, A2, A3, B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_3_3 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (a1, a2, a3, b1, b2, b3); }
};

template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
wrap (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
cwrap (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
cpwrap (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3, class A1, class A2, class A3>
class callback_c_3_3
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (A1, A2, A3, B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
public:
  callback_c_3_3 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (a1, a2, a3, b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
wrap (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
cwrap (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
wrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
class callback_3_4
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (A1, A2, A3, A4, B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_3_4 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (a1, a2, a3, a4, b1, b2, b3); }
};

template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
wrap (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
cwrap (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3, class A1, class A2, class A3, class A4>
class callback_c_3_4
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
public:
  callback_c_3_4 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (a1, a2, a3, a4, b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio  TIMELEFT_DEFAULT);
}

template<class R, class B1, class B2, class B3, class A1, class A2, class A3, class A4, class A5>
class callback_3_5
  : public callback<R, B1, B2, B3> {
  typedef R (*cb_t) (A1, A2, A3, A4, A5, B1, B2, B3);
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_3_5 (cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return f (a1, a2, a3, a4, a5, b1, b2, b3); }
};

template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class P, class C, class R, class B1, class B2, class B3, class A1, class A2, class A3, class A4, class A5>
class callback_c_3_5
  : public callback<R, B1, B2, B3> {
  typedef R (C::*cb_t) (A1, A2, A3, A4, A5, B1, B2, B3);
  P c;
  cb_t f;
  int col;
  int pri;
  int timeleft;
  bool AutoFree;
  A1 a1;
  A2 a2;
  A3 a3;
  A4 a4;
  A5 a5;
public:
  callback_c_3_5 (const P &cc, cb_t ff, const A1 &aa1, const A2 &aa2, const A3 &aa3, const A4 &aa4, const A5 &aa5, int _c, int _p
           , int _timeleft
  )
    : c (cc), f (ff), a1 (aa1), a2 (aa2), a3 (aa3), a4 (aa4), a5 (aa5) { col = _c; pri = _p;
    timeleft = _timeleft;
    AutoFree = true;
  }
  int getcolor () { return col; }
  int getprio () { return pri; }
  int get_timeleft () { return timeleft; }
  void setFlagAutoFree (bool flag) { AutoFree = flag; }
  bool getFlagAutoFree () { return AutoFree; }
  void* getfaddr (){ return (void *)&f; }
  R operator() (B1 b1, B2 b2, B3 b3)
    { return ((*c).*f) (a1, a2, a3, a4, a5, b1, b2, b3); }
};

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0  TIMELEFT_DEFAULT);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio  TIMELEFT_DEFAULT);
}

template<class R>
static inline callback_0_0<R> *
wrap_timeleft (R (*f) (), int timeleft)
{
  return new (callback_0_0<R>) (f, 0, 0, timeleft);
}
template<class R>
static inline callback_0_0<R> *
cwrap_timeleft (R (*f) (), int c, int timeleft)
{
  return new (callback_0_0<R>) (f, c, 0, timeleft);
}
template<class R>
static inline callback_0_0<R> *
cpwrap_timeleft (R (*f) (), int c, int prio, int timeleft)
{
  return new (callback_0_0<R>) (f, c, prio, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
wrap_timeleft (C *p, R (C::*f) (), int timeleft)
{
  return new (callback_c_0_0<C *, C, R>) (p, f, 0, 0, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
cwrap_timeleft (C *p, R (C::*f) (), int c, int timeleft)
{
  return new (callback_c_0_0<C *, C, R>) (p, f, c, 0, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C *, C, R> *
cpwrap_timeleft (C *p, R (C::*f) (), int c, int prio, int timeleft)
{
  return new (callback_c_0_0<C *, C, R>) (p, f, c, prio, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C, C, R> *
wrap_timeleft (const C &p, R (C::*f) (), int timeleft)
{
  return new (callback_c_0_0<C, C, R>) (p, f, 0, 0, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C, C, R> *
cwrap_timeleft (const C &p, R (C::*f) (), int c, int timeleft)
{
  return new (callback_c_0_0<C, C, R>) (p, f, c, 0, timeleft);
}
template<class C, class R>
static inline callback_c_0_0<C, C, R> *
cpwrap_timeleft (const C &p, R (C::*f) (), int c, int prio, int timeleft)
{
  return new (callback_c_0_0<C, C, R>) (p, f, c, prio, timeleft);
}
template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
wrap_timeleft (R (*f) (A1), const AA1 &a1, int timeleft)
{
  return new (callback_0_1<R, A1>) (f, a1, 0, 0, timeleft);
}
template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
cwrap_timeleft (R (*f) (A1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_0_1<R, A1>) (f, a1, c, 0, timeleft);
}
template<class R, class A1, class AA1>
static inline callback_0_1<R, A1> *
cpwrap_timeleft (R (*f) (A1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_0_1<R, A1>) (f, a1, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
wrap_timeleft (C *p, R (C::*f) (A1), const AA1 &a1, int timeleft)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
cwrap_timeleft (C *p, R (C::*f) (A1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C *, C, R, A1> *
cpwrap_timeleft (C *p, R (C::*f) (A1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_0_1<C *, C, R, A1>) (p, f, a1, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
wrap_timeleft (const C &p, R (C::*f) (A1), const AA1 &a1, int timeleft)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
cwrap_timeleft (const C &p, R (C::*f) (A1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1>
static inline callback_c_0_1<C, C, R, A1> *
cpwrap_timeleft (const C &p, R (C::*f) (A1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_0_1<C, C, R, A1>) (p, f, a1, c, prio, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
wrap_timeleft (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_0_2<R, A1, A2>) (f, a1, a2, 0, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
cwrap_timeleft (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_0_2<R, A1, A2>) (f, a1, a2, c, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2>
static inline callback_0_2<R, A1, A2> *
cpwrap_timeleft (R (*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_0_2<R, A1, A2>) (f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
wrap_timeleft (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C *, C, R, A1, A2> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_0_2<C *, C, R, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2>
static inline callback_c_0_2<C, C, R, A1, A2> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_0_2<C, C, R, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
wrap_timeleft (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, 0, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
cwrap_timeleft (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, c, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_0_3<R, A1, A2, A3> *
cpwrap_timeleft (R (*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_0_3<R, A1, A2, A3>) (f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C *, C, R, A1, A2, A3> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_0_3<C *, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_0_3<C, C, R, A1, A2, A3> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_0_3<C, C, R, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
wrap_timeleft (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_0_4<R, A1, A2, A3, A4> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_0_4<R, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C *, C, R, A1, A2, A3, A4> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_0_4<C *, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_0_4<C, C, R, A1, A2, A3, A4> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_0_4<C, C, R, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_0_5<R, A1, A2, A3, A4, A5> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_0_5<R, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_0_5<C *, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_0_5<C, C, R, A1, A2, A3, A4, A5> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_0_5<C, C, R, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class R, class B1>
static inline callback_1_0<R, B1> *
wrap_timeleft (R (*f) (B1), int timeleft)
{
  return new (callback_1_0<R, B1>) (f, 0, 0, timeleft);
}
template<class R, class B1>
static inline callback_1_0<R, B1> *
cwrap_timeleft (R (*f) (B1), int c, int timeleft)
{
  return new (callback_1_0<R, B1>) (f, c, 0, timeleft);
}
template<class R, class B1>
static inline callback_1_0<R, B1> *
cpwrap_timeleft (R (*f) (B1), int c, int prio, int timeleft)
{
  return new (callback_1_0<R, B1>) (f, c, prio, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
wrap_timeleft (C *p, R (C::*f) (B1), int timeleft)
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
cwrap_timeleft (C *p, R (C::*f) (B1), int c, int timeleft)
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C *, C, R, B1> *
cpwrap_timeleft (C *p, R (C::*f) (B1), int c, int prio, int timeleft)
{
  return new (callback_c_1_0<C *, C, R, B1>) (p, f, c, prio, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
wrap_timeleft (const C &p, R (C::*f) (B1), int timeleft)
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
cwrap_timeleft (const C &p, R (C::*f) (B1), int c, int timeleft)
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1>
static inline callback_c_1_0<C, C, R, B1> *
cpwrap_timeleft (const C &p, R (C::*f) (B1), int c, int prio, int timeleft)
{
  return new (callback_c_1_0<C, C, R, B1>) (p, f, c, prio, timeleft);
}
template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
wrap_timeleft (R (*f) (A1, B1), const AA1 &a1, int timeleft)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, 0, 0, timeleft);
}
template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
cwrap_timeleft (R (*f) (A1, B1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, c, 0, timeleft);
}
template<class R, class B1, class A1, class AA1>
static inline callback_1_1<R, B1, A1> *
cpwrap_timeleft (R (*f) (A1, B1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_1_1<R, B1, A1>) (f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
wrap_timeleft (C *p, R (C::*f) (A1, B1), const AA1 &a1, int timeleft)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
cwrap_timeleft (C *p, R (C::*f) (A1, B1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C *, C, R, B1, A1> *
cpwrap_timeleft (C *p, R (C::*f) (A1, B1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_1_1<C *, C, R, B1, A1>) (p, f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
wrap_timeleft (const C &p, R (C::*f) (A1, B1), const AA1 &a1, int timeleft)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
cwrap_timeleft (const C &p, R (C::*f) (A1, B1), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1>
static inline callback_c_1_1<C, C, R, B1, A1> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, B1), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_1_1<C, C, R, B1, A1>) (p, f, a1, c, prio, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
wrap_timeleft (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, 0, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
cwrap_timeleft (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, c, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_1_2<R, B1, A1, A2> *
cpwrap_timeleft (R (*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_1_2<R, B1, A1, A2>) (f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C *, C, R, B1, A1, A2> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_1_2<C *, C, R, B1, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2>
static inline callback_c_1_2<C, C, R, B1, A1, A2> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_1_2<C, C, R, B1, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
wrap_timeleft (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, 0, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
cwrap_timeleft (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, c, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_1_3<R, B1, A1, A2, A3> *
cpwrap_timeleft (R (*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_1_3<R, B1, A1, A2, A3>) (f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C *, C, R, B1, A1, A2, A3> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_1_3<C *, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_1_3<C, C, R, B1, A1, A2, A3> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_1_3<C, C, R, B1, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_1_4<R, B1, A1, A2, A3, A4> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_1_4<R, B1, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_1_4<C *, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_1_4<C, C, R, B1, A1, A2, A3, A4> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_1_4<C, C, R, B1, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_1_5<R, B1, A1, A2, A3, A4, A5> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_1_5<R, B1, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_1_5<C *, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_1_5<C, C, R, B1, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
wrap_timeleft (R (*f) (B1, B2), int timeleft)
{
  return new (callback_2_0<R, B1, B2>) (f, 0, 0, timeleft);
}
template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
cwrap_timeleft (R (*f) (B1, B2), int c, int timeleft)
{
  return new (callback_2_0<R, B1, B2>) (f, c, 0, timeleft);
}
template<class R, class B1, class B2>
static inline callback_2_0<R, B1, B2> *
cpwrap_timeleft (R (*f) (B1, B2), int c, int prio, int timeleft)
{
  return new (callback_2_0<R, B1, B2>) (f, c, prio, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
wrap_timeleft (C *p, R (C::*f) (B1, B2), int timeleft)
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
cwrap_timeleft (C *p, R (C::*f) (B1, B2), int c, int timeleft)
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C *, C, R, B1, B2> *
cpwrap_timeleft (C *p, R (C::*f) (B1, B2), int c, int prio, int timeleft)
{
  return new (callback_c_2_0<C *, C, R, B1, B2>) (p, f, c, prio, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
wrap_timeleft (const C &p, R (C::*f) (B1, B2), int timeleft)
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
cwrap_timeleft (const C &p, R (C::*f) (B1, B2), int c, int timeleft)
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1, class B2>
static inline callback_c_2_0<C, C, R, B1, B2> *
cpwrap_timeleft (const C &p, R (C::*f) (B1, B2), int c, int prio, int timeleft)
{
  return new (callback_c_2_0<C, C, R, B1, B2>) (p, f, c, prio, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
wrap_timeleft (R (*f) (A1, B1, B2), const AA1 &a1, int timeleft)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, 0, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
cwrap_timeleft (R (*f) (A1, B1, B2), const AA1 &a1, int c, int timeleft)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, c, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1>
static inline callback_2_1<R, B1, B2, A1> *
cpwrap_timeleft (R (*f) (A1, B1, B2), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_2_1<R, B1, B2, A1>) (f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
wrap_timeleft (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1, int timeleft)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
cwrap_timeleft (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C *, C, R, B1, B2, A1> *
cpwrap_timeleft (C *p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_2_1<C *, C, R, B1, B2, A1>) (p, f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
wrap_timeleft (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1, int timeleft)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
cwrap_timeleft (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1>
static inline callback_c_2_1<C, C, R, B1, B2, A1> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, B1, B2), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_2_1<C, C, R, B1, B2, A1>) (p, f, a1, c, prio, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
wrap_timeleft (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, 0, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
cwrap_timeleft (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, c, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_2_2<R, B1, B2, A1, A2> *
cpwrap_timeleft (R (*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_2_2<R, B1, B2, A1, A2>) (f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C *, C, R, B1, B2, A1, A2> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_2_2<C *, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2>
static inline callback_c_2_2<C, C, R, B1, B2, A1, A2> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_2_2<C, C, R, B1, B2, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
wrap_timeleft (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, 0, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
cwrap_timeleft (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, c, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_2_3<R, B1, B2, A1, A2, A3> *
cpwrap_timeleft (R (*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_2_3<R, B1, B2, A1, A2, A3>) (f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_2_3<C *, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_2_3<C, C, R, B1, B2, A1, A2, A3> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_2_3<C, C, R, B1, B2, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_2_4<R, B1, B2, A1, A2, A3, A4> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_2_4<R, B1, B2, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_2_4<C *, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_2_4<C, C, R, B1, B2, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_2_5<R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_2_5<R, B1, B2, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_2_5<C *, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_2_5<C, C, R, B1, B2, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
wrap_timeleft (R (*f) (B1, B2, B3), int timeleft)
{
  return new (callback_3_0<R, B1, B2, B3>) (f, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
cwrap_timeleft (R (*f) (B1, B2, B3), int c, int timeleft)
{
  return new (callback_3_0<R, B1, B2, B3>) (f, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3>
static inline callback_3_0<R, B1, B2, B3> *
cpwrap_timeleft (R (*f) (B1, B2, B3), int c, int prio, int timeleft)
{
  return new (callback_3_0<R, B1, B2, B3>) (f, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
wrap_timeleft (C *p, R (C::*f) (B1, B2, B3), int timeleft)
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
cwrap_timeleft (C *p, R (C::*f) (B1, B2, B3), int c, int timeleft)
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C *, C, R, B1, B2, B3> *
cpwrap_timeleft (C *p, R (C::*f) (B1, B2, B3), int c, int prio, int timeleft)
{
  return new (callback_c_3_0<C *, C, R, B1, B2, B3>) (p, f, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
wrap_timeleft (const C &p, R (C::*f) (B1, B2, B3), int timeleft)
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
cwrap_timeleft (const C &p, R (C::*f) (B1, B2, B3), int c, int timeleft)
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3>
static inline callback_c_3_0<C, C, R, B1, B2, B3> *
cpwrap_timeleft (const C &p, R (C::*f) (B1, B2, B3), int c, int prio, int timeleft)
{
  return new (callback_c_3_0<C, C, R, B1, B2, B3>) (p, f, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
wrap_timeleft (R (*f) (A1, B1, B2, B3), const AA1 &a1, int timeleft)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
cwrap_timeleft (R (*f) (A1, B1, B2, B3), const AA1 &a1, int c, int timeleft)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_3_1<R, B1, B2, B3, A1> *
cpwrap_timeleft (R (*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_3_1<R, B1, B2, B3, A1>) (f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
wrap_timeleft (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int timeleft)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
cwrap_timeleft (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C *, C, R, B1, B2, B3, A1> *
cpwrap_timeleft (C *p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_3_1<C *, C, R, B1, B2, B3, A1>) (p, f, a1, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
wrap_timeleft (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int timeleft)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
cwrap_timeleft (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int timeleft)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1>
static inline callback_c_3_1<C, C, R, B1, B2, B3, A1> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, B1, B2, B3), const AA1 &a1, int c, int prio, int timeleft)
{
  return new (callback_c_3_1<C, C, R, B1, B2, B3, A1>) (p, f, a1, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
wrap_timeleft (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
cwrap_timeleft (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_3_2<R, B1, B2, B3, A1, A2> *
cpwrap_timeleft (R (*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_3_2<R, B1, B2, B3, A1, A2>) (f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_3_2<C *, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int timeleft)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int timeleft)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2>
static inline callback_c_3_2<C, C, R, B1, B2, B3, A1, A2> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, B1, B2, B3), const AA1 &a1, const AA2 &a2, int c, int prio, int timeleft)
{
  return new (callback_c_3_2<C, C, R, B1, B2, B3, A1, A2>) (p, f, a1, a2, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
wrap_timeleft (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
cwrap_timeleft (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_3_3<R, B1, B2, B3, A1, A2, A3> *
cpwrap_timeleft (R (*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_3_3<R, B1, B2, B3, A1, A2, A3>) (f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_3_3<C *, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int timeleft)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int timeleft)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3>
static inline callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, int c, int prio, int timeleft)
{
  return new (callback_c_3_3<C, C, R, B1, B2, B3, A1, A2, A3>) (p, f, a1, a2, a3, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_3_4<R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_3_4<R, B1, B2, B3, A1, A2, A3, A4>) (f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_3_4<C *, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int timeleft)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int timeleft)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4>
static inline callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, int c, int prio, int timeleft)
{
  return new (callback_c_3_4<C, C, R, B1, B2, B3, A1, A2, A3, A4>) (p, f, a1, a2, a3, a4, c, prio, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap_timeleft (R (*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_3_5<R, B1, B2, B3, A1, A2, A3, A4, A5>) (f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap_timeleft (C *p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_3_5<C *, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
wrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int timeleft)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, 0, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int timeleft)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, 0, timeleft);
}
template<class C, class R, class B1, class B2, class B3, class A1, class AA1, class A2, class AA2, class A3, class AA3, class A4, class AA4, class A5, class AA5>
static inline callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5> *
cpwrap_timeleft (const C &p, R (C::*f) (A1, A2, A3, A4, A5, B1, B2, B3), const AA1 &a1, const AA2 &a2, const AA3 &a3, const AA4 &a4, const AA5 &a5, int c, int prio, int timeleft)
{
  return new (callback_c_3_5<C, C, R, B1, B2, B3, A1, A2, A3, A4, A5>) (p, f, a1, a2, a3, a4, a5, c, prio, timeleft);
}

typedef callback<void> * cbv;
typedef callback<void, int> * cbi;
#define CBV_PTR_TYPE cbv

#endif
