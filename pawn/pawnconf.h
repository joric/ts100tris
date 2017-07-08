#ifndef PAWNCONF_H_INCLUDED
#define PAWNCONF_H_INCLUDED

#define AMX_ASM // Use assembler abstract machine core

#define AMX_ANSIONLY
#define AMX_NODYNALOAD
#define AMX_NO_MACRO_INSTR
#define AMX_NO_OVERLAY
#define AMX_NO_PACKED_OPC

// Define only used functions here
//#define AMX_ALIGN // amx_Align16(), amx_Align32() and amx_Align64() */
//#define AMX_ALLOT // amx_Allot() and amx_Release() */
#define AMX_DEFCALLBACK
#define AMX_CLEANUP
//#define AMX_CLONE
#define AMX_EXEC
//#define AMX_FLAGS
#define AMX_INIT
//#define AMX_MEMINFO
//#define AMX_NAMELENGTH
//#define AMX_NATIVEINFO
//#define AMX_PUSHXXX // amx_Push(), amx_PushAddress(), amx_PushArray() and amx_PushString() */
//#define AMX_RAISEERROR
//#define AMX_REGISTER
//#define AMX_SETCALLBACK
//#define AMX_SETDEBUGHOOK
//#define AMX_UTF8XXX // amx_UTF8Check(), amx_UTF8Get(), amx_UTF8Len() and amx_UTF8Put() */
//#define AMX_XXXNATIVES // amx_NumNatives(), amx_GetNative() and amx_FindNative() */
//#define AMX_XXXPUBLICS // amx_NumPublics(), amx_GetPublic() and amx_FindPublic() */
//#define AMX_XXXPUBVARS // amx_NumPubVars(), amx_GetPubVar() and amx_FindPubVar() */
//#define AMX_XXXSTRING // amx_StrLen(), amx_GetString() and amx_SetString() */
//#define AMX_XXXTAGS // amx_NumTags(), amx_GetTag() and amx_FindTagId() */
//#define AMX_XXXUSERDATA // amx_GetUserData() and amx_SetUserData() */

// Handle packed opcodes and token threading dependency (cut from ASM file)
#ifndef AMX_NO_PACKED_OPC
  #ifndef AMX_TOKENTHREADING
    #define AMX_TOKENTHREADING
  #endif
#endif

#endif // PAWNCONF_H_INCLUDED
