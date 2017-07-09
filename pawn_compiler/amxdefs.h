/*  Pawn Abstract Machine definitions for pawncc
 *
 *  Copyright (c) ITB CompuPhase, 1997-2011
 *
 *  Based on amx.h and amxdbg.h version 4535
 */

#ifndef AMXDEFS_H_INCLUDED
#define AMXDEFS_H_INCLUDED

// Common includes and defines (from amx.h)

#include <stdlib.h>   /* for size_t */
#include <limits.h>

#if defined __linux || defined __linux__
  #define __LINUX__
#endif
#if defined FREEBSD && !defined __FreeBSD__
  #define __FreeBSD__
#endif
#if defined __LINUX__ || defined __FreeBSD__ || defined __OpenBSD__
  #include <sclinux.h>
#endif

#if !defined HAVE_STDINT_H
  #if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) \
      || defined __GNUC__ || defined __LCC__ || defined __DMC__ \
      || (defined __WATCOMC__ && __WATCOMC__ >= 1200)
    #define HAVE_STDINT_H 1
  #endif
#endif
#if !defined HAVE_INTTYPES_H
  #if defined __FreeBSD__
    #define HAVE_INTTYPES_H 1
  #endif
#endif
#if defined HAVE_STDINT_H
  #include <stdint.h>
#elif defined HAVE_INTTYPES_H
  #include <inttypes.h>
#else
  #if defined __MACH__
    #include <ppc/types.h>
  #endif
  typedef short int           int16_t;
  typedef unsigned short int  uint16_t;
  #if defined SN_TARGET_PS2
    typedef int               int32_t;
    typedef unsigned int      uint32_t;
  #else
    typedef long int          int32_t;
    typedef unsigned long int uint32_t;
  #endif
  #if defined __WIN32__ || defined _WIN32 || defined WIN32
    typedef __int64	          int64_t;
    typedef unsigned __int64  uint64_t;
    #define HAVE_I64
  #endif
  #if !defined _INTPTR_T_DEFINED
    #if defined _LP64 || defined WIN64 || defined _WIN64
      typedef __int64         intptr_t;
    #else
      typedef int32_t         intptr_t;
    #endif
  #endif
#endif
#if defined _LP64 || defined WIN64 || defined _WIN64
  #if !defined __64BIT__
    #define __64BIT__
  #endif
#endif

#if !defined HAVE_ALLOCA_H
  #if defined __GNUC__ || defined __LCC__ || defined __DMC__ || defined __ARMCC_VERSION
    #define HAVE_ALLOCA_H 1
  #elif defined __WATCOMC__ && __WATCOMC__ >= 1200
    #define HAVE_ALLOCA_H 1
  #endif
#endif
#if defined HAVE_ALLOCA_H && HAVE_ALLOCA_H
  #include <alloca.h>
#elif defined __BORLANDC__
  #include <malloc.h>
#endif
#if defined __WIN32__ || defined _WIN32 || defined WIN32 /* || defined __MSDOS__ */
  #if !defined alloca
    #define alloca(n)   _alloca(n)
  #endif
#endif

/* Some compilers do not support the #pragma align, which should be fine. Some
 * compilers give a warning on unknown #pragmas, which is not so fine...
 */
#if (defined SN_TARGET_PS2 || defined __GNUC__) && !defined AMX_NO_ALIGN
  #define AMX_NO_ALIGN
#endif

#if defined __GNUC__
  #define PACKED        __attribute__((packed))
#else
  #define PACKED
#endif

#if defined _MSC_VER
  #pragma warning(disable:4103)  /* disable warning message 4103 that complains about pragma pack in a header file */
  #pragma warning(disable:4100)  /* "'%$S' : unreferenced formal parameter" */
  #pragma warning(disable:4996)  /* POSIX name is deprecated */
#endif

#if !defined PAWN_CELL_SIZE
  #define PAWN_CELL_SIZE 32     /* by default, use 32-bit cells */
#endif
#if PAWN_CELL_SIZE==16
  typedef uint16_t  ucell;
  typedef int16_t   cell;
#elif PAWN_CELL_SIZE==32
  typedef uint32_t  ucell;
  typedef int32_t   cell;
#elif PAWN_CELL_SIZE==64
  typedef uint64_t  ucell;
  typedef int64_t   cell;
#else
  #error Unsupported cell size (PAWN_CELL_SIZE)
#endif

#define AMX_MAGIC_16    0xf1e2
#define AMX_MAGIC_32    0xf1e0
#define AMX_MAGIC_64    0xf1e1
#if PAWN_CELL_SIZE==16
  #define AMX_MAGIC     AMX_MAGIC_16
#elif PAWN_CELL_SIZE==32
  #define AMX_MAGIC     AMX_MAGIC_32
#elif PAWN_CELL_SIZE==64
  #define AMX_MAGIC     AMX_MAGIC_64
#endif

// Defines, structs and consts from amx.h

#define CUR_FILE_VERSION 11     /* current file version; also the current AMX version */
#define MIN_FILE_VERSION 11     /* lowest supported file format version for the current AMX version */
#define MIN_AMX_VERSION  11     /* minimum AMX version needed to support the current file format */
#define MAX_FILE_VER_JIT 11     /* file version supported by the JIT */
#define MIN_AMX_VER_JIT  11     /* AMX version supported by the JIT */

#define AMX_FLAG_OVERLAY  0x01  /* all function calls use overlays */
#define AMX_FLAG_DEBUG    0x02  /* symbolic info. available */
#define AMX_FLAG_NOCHECKS 0x04  /* no array bounds checking; no BREAK opcodes */
#define AMX_FLAG_SLEEP    0x08  /* script uses the sleep instruction (possible re-entry or power-down mode) */
#define AMX_FLAG_CRYPT    0x10  /* file is encrypted */
#define AMX_FLAG_DSEG_INIT 0x20 /* data section is explicitly initialized */
#define AMX_FLAG_SYSREQN 0x800  /* script uses new (optimized) version of SYSREQ opcode */
#define AMX_FLAG_NTVREG 0x1000  /* all native functions are registered */
#define AMX_FLAG_JITC   0x2000  /* abstract machine is JIT compiled */
#define AMX_FLAG_VERIFY 0x4000  /* busy verifying P-code */
#define AMX_FLAG_INIT   0x8000  /* AMX has been initialized */

enum {
  AMX_ERR_NONE,
  /* reserve the first 15 error codes for exit codes of the abstract machine */
  AMX_ERR_EXIT,         /* forced exit */
  AMX_ERR_ASSERT,       /* assertion failed */
  AMX_ERR_STACKERR,     /* stack/heap collision */
  AMX_ERR_BOUNDS,       /* index out of bounds */
  AMX_ERR_MEMACCESS,    /* invalid memory access */
  AMX_ERR_INVINSTR,     /* invalid instruction */
  AMX_ERR_STACKLOW,     /* stack underflow */
  AMX_ERR_HEAPLOW,      /* heap underflow */
  AMX_ERR_CALLBACK,     /* no callback, or invalid callback */
  AMX_ERR_NATIVE,       /* native function failed */
  AMX_ERR_DIVIDE,       /* divide by zero */
  AMX_ERR_SLEEP,        /* go into sleepmode - code can be restarted */
  AMX_ERR_INVSTATE,     /* no implementation for this state, no fall-back */

  AMX_ERR_MEMORY = 16,  /* out of memory */
  AMX_ERR_FORMAT,       /* invalid file format */
  AMX_ERR_VERSION,      /* file is for a newer version of the AMX */
  AMX_ERR_NOTFOUND,     /* function not found */
  AMX_ERR_INDEX,        /* invalid index parameter (bad entry point) */
  AMX_ERR_DEBUG,        /* debugger cannot run */
  AMX_ERR_INIT,         /* AMX not initialized (or doubly initialized) */
  AMX_ERR_USERDATA,     /* unable to set user data field (table full) */
  AMX_ERR_INIT_JIT,     /* cannot initialize the JIT */
  AMX_ERR_PARAMS,       /* parameter error */
  AMX_ERR_DOMAIN,       /* domain error, expression result does not fit in range */
  AMX_ERR_GENERAL,      /* general error (unknown or unspecific error) */
  AMX_ERR_OVERLAY,      /* overlays are unsupported (JIT) or uninitialized */
};

#define sNAMEMAX 31  /* maximum name length of symbol name */

typedef struct tagAMX_HEADER {
  int32_t size;             /* size of the "file" */
  uint16_t magic;           /* signature */
  char    file_version;     /* file format version */
  char    amx_version;      /* required version of the AMX */
  int16_t flags;
  int16_t defsize;          /* size of a definition record */
  int32_t cod;              /* initial value of COD - code block */
  int32_t dat;              /* initial value of DAT - data block */
  int32_t hea;              /* initial value of HEA - start of the heap */
  int32_t stp;              /* initial value of STP - stack top */
  int32_t cip;              /* initial value of CIP - the instruction pointer */
  int32_t publics;          /* offset to the "public functions" table */
  int32_t natives;          /* offset to the "native functions" table */
  int32_t libraries;        /* offset to the table of libraries */
  int32_t pubvars;          /* offset to the "public variables" table */
  int32_t tags;             /* offset to the "public tagnames" table */
  int32_t nametable;        /* offset to the name table */
  int32_t overlays;         /* offset to the overlay table */
} PACKED AMX_HEADER;

typedef struct tagFUNCSTUB {
  uint32_t address;
  uint32_t nameofs;
} PACKED AMX_FUNCSTUB;

typedef struct tagOVERLAYINFO {
  int32_t offset;           /* offset relative to the start of the code block */
  int32_t size;             /* size in bytes */
} PACKED AMX_OVERLAYINFO;

// Defines and structs from amxdbg.h

#define AMX_DBG_MAGIC   0xf1ef

typedef struct tagAMX_DBG_HDR {
  int32_t  size;            /* size of the debug information chunk */
  uint16_t magic;           /* signature, must be 0xf1ef */
  char     file_version;    /* file format version */
  char     amx_version;     /* required version of the AMX */
  int16_t  flags;           /* currently unused */
  int16_t  files;           /* number of entries in the "file" table */
  int16_t  lines;           /* number of entries in the "line" table */
  int16_t  symbols;         /* number of entries in the "symbol" table */
  int16_t  tags;            /* number of entries in the "tag" table */
  int16_t  automatons;      /* number of entries in the "automaton" table */
  int16_t  states;          /* number of entries in the "state" table */
} PACKED AMX_DBG_HDR;

typedef struct tagAMX_DBG_FILE {
  uint32_t address;         /* address in the code segment where generated code (for this file) starts */
  char     name[1];         /* ASCII string, zero-terminated */
} PACKED AMX_DBG_FILE;

typedef struct tagAMX_DBG_LINE {
  uint32_t address;         /* address in the code segment where generated code (for this line) starts */
  int32_t  line;            /* line number */
} PACKED AMX_DBG_LINE;

typedef struct tagAMX_DBG_SYMBOL {
  uint32_t address;         /* address in the data segment or relative to the frame */
  int16_t  tag;             /* tag for the symbol */
  uint32_t codestart;       /* address in the code segment from which this symbol is valid (in scope) */
  uint32_t codeend;         /* address in the code segment until which this symbol is valid (in scope) */
  char     ident;           /* kind of symbol (function/variable) */
  char     vclass;          /* class of symbol (global/local) */
  int16_t  dim;             /* number of dimensions */
  char     name[1];         /* ASCII string, zero-terminated */
} PACKED AMX_DBG_SYMBOL;

typedef struct tagAMX_DBG_SYMDIM {
  int16_t  tag;             /* tag for the array dimension */
  uint32_t size;            /* size of the array dimension */
} PACKED AMX_DBG_SYMDIM;

typedef struct tagAMX_DBG_TAG {
  int16_t  tag;             /* tag id */
  char     name[1];         /* ASCII string, zero-terminated */
} PACKED AMX_DBG_TAG;

typedef struct tagAMX_DBG_MACHINE {
  int16_t  automaton;       /* automaton id */
  uint32_t address;         /* address of state variable */
  char     name[1];         /* ASCII string, zero-terminated */
} PACKED AMX_DBG_MACHINE;

typedef struct tagAMX_DBG_STATE {
  int16_t  state;           /* state id */
  int16_t  automaton;       /* automaton id */
  char     name[1];         /* ASCII string, zero-terminated */
} PACKED AMX_DBG_STATE;

#endif /* AMXDEFS_H_INCLUDED */
