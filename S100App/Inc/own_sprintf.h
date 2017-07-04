/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. **********************
File Name :      own_sprintf.h
Version :        S100 APP Ver 2.13   
Description:
Author :         Celery
Data:            2017/01/17
History:
2015/07/07   统一命名；
2017/01/17   增加文件
*******************************************************************************/



char * __own_itoa(int num,char *str);
char * own_sprintf(char* dest,const char*cntrl_string, ...);
