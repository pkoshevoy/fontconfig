/*
 * Mutex operations.  Originally copied from HarfBuzz.
 *
 * Copyright © 2007  Chris Wilson
 * Copyright © 2009,2010  Red Hat, Inc.
 * Copyright © 2011,2012  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Contributor(s):
 *	Chris Wilson <chris@chris-wilson.co.uk>
 * Red Hat Author(s): Behdad Esfahbod
 * Google Author(s): Behdad Esfahbod
 */

#ifndef _FCATOMIC_H_
#define _FCATOMIC_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/* atomic_int */

/* We need external help for these */

#if 0


#elif !defined(FC_NO_MT) && defined(_MSC_VER) || defined(__MINGW32__)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* mingw32 does not have MemoryBarrier.
 * MemoryBarrier may be defined as a macro or a function.
 * Just make a failsafe version for ourselves. */
#ifdef MemoryBarrier
#define HBMemoryBarrier MemoryBarrier
#else
static inline void HBMemoryBarrier (void) {
  long dummy = 0;
  InterlockedExchange (&dummy, 1);
}
#endif

typedef long fc_atomic_int_t;
#define fc_atomic_int_add(AI, V)	InterlockedExchangeAdd (&(AI), (V))

#define fc_atomic_ptr_get(P)		(HBMemoryBarrier (), (void *) *(P))
#define fc_atomic_ptr_cmpexch(P,O,N)	(InterlockedCompareExchangePointer ((void **) (P), (void *) (N), (void *) (O)) == (void *) (O))


#elif !defined(FC_NO_MT) && defined(__APPLE__)

#include <libkern/OSAtomic.h>

typedef int32_t fc_atomic_int_t;
#define fc_atomic_int_add(AI, V)	(OSAtomicAdd32Barrier ((V), &(AI)) - (V))

#define fc_atomic_ptr_get(P)		(OSMemoryBarrier (), (void *) *(P))
#define fc_atomic_ptr_cmpexch(P,O,N)	OSAtomicCompareAndSwapPtrBarrier ((void *) (O), (void *) (N), (void **) (P))


#elif !defined(FC_NO_MT) && defined(HAVE_INTEL_ATOMIC_PRIMITIVES)

typedef int fc_atomic_int_t;
#define fc_atomic_int_add(AI, V)	__sync_fetch_and_add (&(AI), (V))

#define fc_atomic_ptr_get(P)		(void *) (__sync_synchronize (), *(P))
#define fc_atomic_ptr_cmpexch(P,O,N)	__sync_bool_compare_and_swap ((P), (O), (N))


#elif !defined(FC_NO_MT)

#define FC_ATOMIC_INT_NIL 1 /* Warn that fallback implementation is in use. */
typedef volatile int fc_atomic_int_t;
#define fc_atomic_int_add(AI, V)	(((AI) += (V)) - (V))

#define fc_atomic_ptr_get(P)		((void *) *(P))
#define fc_atomic_ptr_cmpexch(P,O,N)	(* (void * volatile *) (P) == (void *) (O) ? (* (void * volatile *) (P) = (void *) (N), true) : false)


#else /* FC_NO_MT */

typedef int fc_atomic_int_t;
#define fc_atomic_int_add(AI, V)	(((AI) += (V)) - (V))

#define fc_atomic_ptr_get(P)		((void *) *(P))
#define fc_atomic_ptr_cmpexch(P,O,N)	(* (void **) (P) == (void *) (O) ? (* (void **) (P) = (void *) (N), true) : false)

#endif

/* reference count */
#define FC_REF_CONSTANT ((fc_atomic_int_t) -1)
#define FC_REF_CONSTANT_INIT {FC_REF_CONSTANT}
typedef struct _FcRef { fc_atomic_int_t count; } FcRef;
static inline void   FcRefInit    (FcRef *r, int v) { r->count = v; }
static inline int    FcRefInc     (FcRef *r) { return fc_atomic_int_add (r->count, +1); }
static inline int    FcRefDec     (FcRef *r) { return fc_atomic_int_add (r->count, -1); }
static inline void   FcRefFinish  (FcRef *r) { r->count = FC_REF_CONSTANT; }
static inline FcBool FcRefIsConst (FcRef *r) { return r->count == FC_REF_CONSTANT; }

#endif /* _FCATOMIC_H_ */
