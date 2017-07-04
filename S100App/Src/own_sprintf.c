#include <stdarg.h>
#include "own_sprintf.h"

char * __own_itoa(int num,char *str)
{
    int i=0,digits=0,n;
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }
    n = num;
    n /= 10;
    digits = 1;
    while(n) {
        n /= 10;
        digits*= 10;
    }

    while(digits) {
        str[i++] = (num/digits)+0x30;
        num %= digits;
        digits /= 10;

    }

    str[i] = '\0';
    return str;
}

char * own_sprintf(char* dest,const char*cntrl_string, ...)
{
    int pos=0,cnt_printed_chars=0,i;
    unsigned char* chptr;
    char str[8];
    int flp;
    va_list ap;
    if (!dest ) return 0;
    va_start(ap,cntrl_string);//使ap指向第一个可选参数

    while (cntrl_string[pos]) {
        if (cntrl_string[pos] == '%') {
            pos++;
            switch (cntrl_string[pos]) {
            case 'c':
                dest[cnt_printed_chars++] = va_arg (ap, unsigned char);//va_arg返回参数列表中的当前参数并使ap指向参数列表中的下一个参数
                break;
            case 's':
                chptr = va_arg (ap, unsigned char*);
                i = 0;

                while (chptr [i]) {
                    dest[cnt_printed_chars++] = chptr [i++];
                }
                break;
            case 'd':
                __own_itoa (va_arg (ap, int), str);
                i = 0;
                while (str [i]) {
                    dest[cnt_printed_chars++] = str [i++];
                }
                break;
            case 'f':
                flp = va_arg (ap, int);
                if(flp <= 9 && flp > 0){//小于0.1大于0.01
                    
                }else if(flp > 9 && flp <= 99){//大于0.1 小于1

                }else if(flp > 99 && flp <= 999){//大于1 小于10
                    dest[3] = (flp % 10) + '0';
                    flp /= 10;
                    dest[2] = (flp % 10) + '0';
                    dest[1] = '.';
                    flp /= 10;
                    dest[0] = (flp % 10) + '0';
                    cnt_printed_chars = 4;
                }else if(flp > 999 && flp <= 9999){//大于10 小于100
                    dest[4] = (flp % 10) + '0';
                    flp /= 10;
                    dest[3] = (flp % 10) + '0';
                    dest[2] = '.';
                    flp /= 10;
                    dest[1] = (flp % 10) + '0';
                    flp /= 10;
                    dest[0] = (flp % 10) + '0';
                    cnt_printed_chars = 5;
                }
                break;
            default:
                dest[cnt_printed_chars++] = cntrl_string[pos];
                break;

            }
        } else {
            dest[cnt_printed_chars++] = cntrl_string[pos];
        }
        pos++;
    }
    va_end(ap); //va_end把ap指针清为NULL
    dest[cnt_printed_chars] = '\0';
    return dest;
}
/******************************** END OF FILE *********************************/
