/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      UI.h
Version :        S100 APP Ver 2.11
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   Í³Ò»ÃüÃû£»
*******************************************************************************/
#ifndef _UI_H
#define _UI_H

#include "STM32F10x.h"

#define HIGHLIGHT_FREQUENCY     60
#define HIGHLTGHT_REDUCE        20
#define SET_PROMPT_X            9 
#define SET_INFO_X              8

u32 Get_UpdataFlag(void);
void Set_UpdataFlag(u32 Cont);
void APP_Init(void);
u32 Calculation_TWork(u8 Flag);
u8 Calculated_Digit(int number);
void Set_temperature(void);
void SetOpt_Proc(void);
void SetOpt_UI(u8 key);
void Temperature_UI(void);
void Temp_SetProc(u8 key);
void Show_Volt(void);
void Display_Temp(u8 x,s16 Temp);
void Show_Notice(void);
void Show_Warning(void);
void Show_MiniTS(void);
void Show_TempDown(s16 Temp,s16 Dst_Temp);
void Set_TemperatureShowFlag(u8 flag);
s16 TemperatureShow_Change(u8 flag ,s16 Tmp);
u8 Get_TemperatureShowFlag(void);
void Show_Ver(u8 ver[],u8 flag);
void Show_OrderChar(u8* ptr,u8 num,u8 width);
u8 Reverse_Bin8(u8 data);
void Show_ReverseChar(u8* ptr,u8 num,u8 width,u8 direction);
void Show_HeatingIcon(u32 ht_flag,u16 active);
void Display_Str(u8 x, char* str);
void Display_Str8(u8 x, char* str ,u8 mode);
void Display_Str6(u8 x, char* str);
void Display_Str10(u8 x, char* str);
void Display_Str12(u8 x, char* str);
void Clear_Pervious(u16 data);
void Print_Integer(s32 data,u8 posi);
void OLed_Display(void);
void Show_Cal(u8 flag);
void Show_Config(void);
void SetOpt_Detailed(void);
void Show_Tetris(void);
extern int bTetris;

#endif
/******************************** END OF FILE *********************************/
