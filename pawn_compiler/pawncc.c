#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined __WIN32__ || defined _WIN32 || defined __MSDOS__
  #include <conio.h>
  #include <io.h>
#endif

#if defined __WIN32__ || defined _WIN32 || defined _Windows
  #include <windows.h>
#endif

#include "sc.h"

#if defined __TURBOC__ && !defined __32BIT__
  extern unsigned int _stklen = 0x2000;
#endif

/* Pawn compiler main() */
int main(int argc, char *argv[])
{
  return pc_compile(argc,argv);
}

/* pc_printf()
 * Called for general purpose "console" output. This function prints general
 * purpose messages; errors go through pc_error(). The function is modelled
 * after printf().
 */
int pc_printf(const char *message,...)
{
  int ret;
  va_list argptr;

  va_start(argptr,message);
  ret=vprintf(message,argptr);
  va_end(argptr);
  fflush(stdout);

  return ret;
}

/* pc_error()
 * Called for producing error output.
 *    number      the error number (as documented in the manual)
 *    message     a string describing the error with embedded %d and %s tokens
 *    filename    the name of the file currently being parsed
 *    firstline   the line number at which the expression started on which
 *                the error was found, or -1 if there is no "starting line"
 *    lastline    the line number at which the error was detected
 *    argptr      a pointer to the first of a series of arguments (for macro
 *                "va_arg")
 * Return:
 *    If the function returns 0, the parser attempts to continue compilation.
 *    On a non-zero return value, the parser aborts.
 */
int pc_error(int number,char *message,char *filename,int firstline,int lastline,va_list argptr)
{
static char *prefix[3]={ "error", "fatal error", "warning" };

  if (number!=0) {
    char *pre;

    pre=prefix[number/100];
    if (firstline>=0)
      fprintf(stderr,"%s(%d -- %d) : %s %03d: ",filename,firstline,lastline,pre,number);
    else
      fprintf(stderr,"%s(%d) : %s %03d: ",filename,lastline,pre,number);
  } /* if */
  vfprintf(stderr,message,argptr);
  fflush(stderr);
  return 0;
}

/* pc_opensrc()
 * Opens a source file (or include file) for reading. The "file" does not have
 * to be a physical file, one might compile from memory.
 *    filename    the name of the "file" to read from
 * Return:
 *    The function must return a pointer, which is used as a "magic cookie" to
 *    all I/O functions. When failing to open the file for reading, the
 *    function must return NULL.
 * Note:
 *    Several "source files" may be open at the same time. Specifically, one
 *    file can be open for reading and another for writing.
 */
void *pc_opensrc(char *filename)
{
  return fopen(filename,"r");
}

/* pc_createsrc()
 * Creates/overwrites a source file for writing. The "file" does not have
 * to be a physical file, one might compile from memory.
 *    filename    the name of the "file" to create
 * Return:
 *    The function must return a pointer, which is used as a "magic cookie" to
 *    all I/O functions. When failing to open the file for reading, the
 *    function must return NULL.
 * Note:
 *    Several "source files" may be open at the same time. Specifically, one
 *    file can be open for reading and another for writing.
 */
void *pc_createsrc(char *filename)
{
  return fopen(filename,"w");
}

/* pc_closesrc()
 * Closes a source file (or include file). The "handle" parameter has the
 * value that pc_opensrc() returned in an earlier call.
 */
void pc_closesrc(void *handle)
{
  assert(handle!=NULL);
  fclose((FILE*)handle);
}

/* pc_readsrc()
 * Reads a single line from the source file (or up to a maximum number of
 * characters if the line in the input file is too long).
 */
char *pc_readsrc(void *handle,unsigned char *target,int maxchars)
{
  return fgets((char*)target,maxchars,(FILE*)handle);
}

/* pc_writesrc()
 * Writes to to the source file. There is no automatic line ending; to end a
 * line, write a "\n".
 */
int pc_writesrc(void *handle,const unsigned char *source)
{
  return fputs((char*)source,(FILE*)handle) >= 0;
}

#define MAXPOSITIONS  4
static fpos_t srcpositions[MAXPOSITIONS];
static unsigned char srcposalloc[MAXPOSITIONS];

void pc_clearpossrc(void)
{
  memset(srcpositions,0,sizeof srcpositions);
  memset(srcposalloc,0,sizeof srcposalloc);
}

void *pc_getpossrc(void *handle,void *position)
{
  if (position==NULL) {
    /* allocate a new slot */
    int i;
    for (i=0; i<MAXPOSITIONS && srcposalloc[i]!=0; i++)
      /* nothing */;
    assert(i<MAXPOSITIONS); /* if not, there is a queue overrun */
    if (i>=MAXPOSITIONS)
      return NULL;
    position=&srcpositions[i];
    srcposalloc[i]=1;
  } else {
    /* use the gived slot */
    assert(position>=(void*)srcpositions && position<(void*)((char*)srcpositions+sizeof(srcpositions)));
  } /* if */
  fgetpos((FILE*)handle,(fpos_t*)position);
  return position;
}

/* pc_resetsrc()
 * "position" may only hold a pointer that was previously obtained from
 * pc_getpossrc()
 */
void pc_resetsrc(void *handle,void *position)
{
  assert(handle!=NULL);
  assert(position!=NULL);
  fsetpos((FILE*)handle,(fpos_t*)position);
  /* note: the item is not cleared from the pool */
}

int pc_eofsrc(void *handle)
{
  return feof((FILE*)handle);
}

/* should return a pointer, which is used as a "magic cookie" to all I/O
 * functions; return NULL for failure
 */
void *pc_openlst(char *filename)
{
  #if defined __MSDOS__
    return fopen(filename,"w+");
  #else
    return mfcreate(filename);
  #endif
}

void pc_closelst(void *handle, int deletefile)
{
  #if defined __MSDOS__
    if (handle!=NULL)
      fclose((FILE*)handle);
    if (deletefile)
      remove(outfname);
  #else
    if (handle!=NULL) {
      if (!deletefile)
        mfdump((memfile_t*)handle);
      mfclose((memfile_t*)handle);
    } /* if */
  #endif
}

void pc_resetlst(void *handle)
{
  assert(handle!=NULL);
  #if defined __MSDOS__
    fflush((FILE*)handle);
    fseek((FILE*)handle,0,SEEK_SET);
  #else
    mfseek((memfile_t*)handle,0,SEEK_SET);
  #endif
}

int pc_writelst(void *handle,const char *string)
{
  #if defined __MSDOS__
    return fputs(string,(FILE*)handle) >= 0;
  #else
    return mfputs((memfile_t*)handle,string);
  #endif
}

char *pc_readlst(void *handle, char *string, int maxchars)
{
  #if defined __MSDOS__
    return fgets(string,maxchars,(FILE*)handle);
  #else
    return mfgets((memfile_t*)handle,string,maxchars);
  #endif
}

/* should return a pointer, which is used as a "magic cookie" to all I/O
 * functions; return NULL for failure
 */
void *pc_openasm(char *filename)
{
  #if defined __MSDOS__
    return fopen(filename,"w+");
  #else
    return mfcreate(filename);
  #endif
}

void pc_closeasm(void *handle, int deletefile)
{
  #if defined __MSDOS__
    if (handle!=NULL)
      fclose((FILE*)handle);
    if (deletefile)
      remove(outfname);
  #else
    if (handle!=NULL) {
      if (!deletefile)
        mfdump((memfile_t*)handle);
      mfclose((memfile_t*)handle);
    } /* if */
  #endif
}

void pc_resetasm(void *handle)
{
  assert(handle!=NULL);
  #if defined __MSDOS__
    fflush((FILE*)handle);
    fseek((FILE*)handle,0,SEEK_SET);
  #else
    mfseek((memfile_t*)handle,0,SEEK_SET);
  #endif
}

int pc_writeasm(void *handle,const char *string)
{
  #if defined __MSDOS__
    return fputs(string,(FILE*)handle) >= 0;
  #else
    return mfputs((memfile_t*)handle,string);
  #endif
}

char *pc_readasm(void *handle, char *string, int maxchars)
{
  #if defined __MSDOS__
    return fgets(string,maxchars,(FILE*)handle);
  #else
    return mfgets((memfile_t*)handle,string,maxchars);
  #endif
}

/* Should return a pointer, which is used as a "magic cookie" to all I/O
 * functions; return NULL for failure.
 */
void *pc_openbin(char *filename)
{
  return fopen(filename,"wb");
}

void pc_closebin(void *handle,int deletefile)
{
  fclose((FILE*)handle);
  if (deletefile)
    remove(binfname);
}

/* pc_resetbin()
 * Can seek to any location in the file.
 * The offset is always from the start of the file.
 */
void pc_resetbin(void *handle,long offset)
{
  fflush((FILE*)handle);
  fseek((FILE*)handle,offset,SEEK_SET);
}

int pc_writebin(void *handle,const void *buffer,int size)
{
  return (int)fwrite(buffer,1,size,(FILE*)handle) == size;
}

long pc_lengthbin(void *handle)
{
  return ftell((FILE*)handle);
}
