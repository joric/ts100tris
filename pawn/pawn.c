#include <stdio.h>
#include <string.h>
#include "pawn.h"
#include "amxfuncs.h"

#include "ff.h" // Filesystem

void *memblock;

int AMXAPI PawnLoadProgram(AMX *amx, const char *filename)
{
  FIL fp;
  AMX_HEADER hdr;
  int result, didalloc;
  UINT fr;

  if (f_open(&fp, filename, FA_OPEN_EXISTING | FA_READ))
    return AMX_ERR_NOTFOUND;
  f_read(&fp, &hdr, sizeof hdr, &fr);

  if (hdr.magic != AMX_MAGIC) {
    f_close(&fp);
    return AMX_ERR_FORMAT;
  }

  // Allocate the memblock if it is NULL
  didalloc = 0;
  if (memblock == NULL) {
    if ((memblock = malloc(hdr.stp)) == NULL) {
      f_close(&fp);
      return AMX_ERR_MEMORY;
    }
    didalloc = 1;
  }

  // Read in the file
  f_lseek(&fp, 0);
  f_read(&fp, memblock, (size_t)hdr.size, &fr);
  f_close(&fp);

  // Initialize the abstract machine
  memset(amx, 0, sizeof *amx);
  result = amx_Init(amx, memblock);

  // Free the memory block on error, if it was allocated here
  if (result != AMX_ERR_NONE && didalloc) {
    free(memblock);
    amx->base = NULL; // Avoid a double free in future
  }

  return result;
}

int AMXAPI PawnFreeProgram(AMX *amx)
{
  if (amx->base!=NULL) {
    amx_Cleanup(amx);
    free(amx->base);
    memset(amx, 0, sizeof(AMX));
  }

  return AMX_ERR_NONE;
}

int PawnExec(char *filename)
{
  AMX amx;
  cell ret = 0;
  int err;

  err = PawnLoadProgram(&amx, filename);
  if (err != AMX_ERR_NONE)
    return err;

  //err = PawnRegister(&amx, core_Natives, -1);
  //if (err != AMX_ERR_NONE)
  //  return err;

  err = amx_Exec(&amx, &ret, AMX_EXEC_MAIN);
  if (err != AMX_ERR_NONE)
    return err;

  PawnFreeProgram(&amx);

  return 0;
}
