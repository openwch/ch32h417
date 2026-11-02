/*
 * jconfig.h
 *
 * Port of libjpeg for the CH32V4x7 (RISC-V) embedded target.
 * Derived from jconfig_template.h.
 *
 * Memory backend: stdlib malloc()/free() (newlib heap). No filesystem.
 * Fatal errors are caught by the application error manager (see main.c),
 * so NO_GETENV / no stderr reliance at runtime.
 */

/*
 * These defines indicate the memory allocation methods.
 * jmemnobs.c implements the system-dependent memory manager and calls
 * JMALLOC/JFREE for every allocation.
 */
#define JMALLOC malloc
#define JFREE   free

/*
 * These symbols indicate the properties of your machine or compiler.
 */
#define NO_GETENV
#undef  USE_MSDOS_MEMMGR
#undef  USE_MAC_MEMMGR
#define USE_HEAP_MEM
#define MAX_ALLOC_CHUNK  0x100000 /* 1MB: 32-bit flat address space */

/* ANSI-C featured compiler */
#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#undef  CHAR_IS_UNSIGNED

#define HAVE_STDDEF_H
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_STDIO_H

#undef  NEED_BSD_STRINGS
#undef  NEED_SYS_TYPES_H
#undef  NEED_FAR_POINTERS
#undef  NEED_SHORT_EXTERNAL_NAMES
#undef  INCOMPLETE_TYPES_BROKEN

/*
 * JFILE selects the stdio-based source/sink managers (jdatasrc/jdatadst).
 * We only use the in-memory source (jpeg_mem_src), so leave JFILE undefined.
 */
#undef JFILE


/*
 * The following options affect code selection within the JPEG library,
 * but they don't need to be visible to applications using the library.
 * To minimize application namespace pollution, the symbols won't be
 * defined unless JPEG_INTERNALS has been defined.
 */
#ifdef JPEG_INTERNALS

/* Define this if your compiler implements ">>" on signed values as a
 * logical (unsigned) shift; leave it undefined for the normal arithmetic shift.
 */
#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */