/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      CTRL.c
Version :        S100 APP Ver 2.11
Description:
Author :         Celery
Data:            2015/08/03
History:
2015/07/07   统一命名；
2015/08/03   优化移动判定
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "CTRL.h"
#include "Bios.h"
#include "UI.h"
#include "HARDWARE.h"
#include "S100V0_1.h"
#include "Disk.h"
#include "MMA8652FC.h"
#include "I2C.h"
#include "Oled.h"

#define HEATINGCYCLE  30
/******************************************************************************/
DEVICE_INFO_SYS device_info;
u8 gHanders;
extern u8 gSet_opt;
extern u8 gTemperatureshowflag;
/******************************************************************************/

u8 gCtrl_status = 1;
u16 gHt_flag = 0;
vs16 gTemp_data = 250;//25℃
s16 gPrev_temp = 250; // 前一个温度值
u8 gIs_restartkey = 0;/*软重启标志*/
u8 gPre_status = 1;
u8 TEMP_SET_Pos = 0;//设置模式标志位 0：只临时改变温度 1：改变设置参数并保存

const DEVICE_INFO_SYS info_def = {
    "2.17",     //Ver
    2000,       //T_Standby;    // 200°C=1800  2520,待机温度
    3000,       // T_Work;      // 350°C=3362, 工作温度
    100,        //T_Step;
    3*60*100,   //Wait_Time;    //3*60*100   3  mintute
    6*60*100,    // Idle_Time;   //6*60*100  6 minute
    0           //handers
};
struct _pid {
    s16 settemp;        //定义设定温度
    s16 actualtemp;     //定义实际温度
    s16 err;            //定义温度差值
    s16 err_last;       //定义上一个温度差值
    s32 ht_time;        //定义加热时间
    u16 kp,ki,kd;       //定义比例、积分、微分系数
    s32 integral;       //定义积分值
} pid;

/*******************************************************************************
函数名: Get_Ctrl_Status
函数作用:获取当前状态
输入参数:无
返回参数:当前状态
*******************************************************************************/
u8 Get_CtrlStatus(void)
{
    return gCtrl_status;
}
/*******************************************************************************
函数名: Set_CtrlStatus
函数作用:设置当前状态
输入参数:status 设置的状态
返回参数:无
*******************************************************************************/
void Set_CtrlStatus(u8 status)
{
    gCtrl_status = status;
}
/*******************************************************************************
函数名: Set_PrevTemp
函数作用:保存前一温度
输入参数:temp 前一温度值
返回参数:无
*******************************************************************************/
void Set_PrevTemp(s16 temp)
{
    gPrev_temp = temp;
}

/*******************************************************************************
函数名: Get_HtFlag
函数作用:获取当前加热标志,加热标志由加热时间决定
输入参数:NULL
返回参数:当前加热标志
*******************************************************************************/
u16 Get_HtFlag(void)
{
    return gHt_flag;
}
/*******************************************************************************
函数名: Get_TempVal
函数作用: 获取当前温度的值
输入参数:NULL
返回参数:当前温度
*******************************************************************************/
s16 Get_TempVal(void)
{
    return gTemp_data;
}

/*******************************************************************************
函数名: System_Init
函数作用: 系统初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void System_Init(void)
{
    memcpy((void*)&device_info,(void*)&info_def,sizeof(device_info));
}
/*******************************************************************************
函数名: PID_init
函数作用: PID数据初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Pid_Init(void)
{
    pid.settemp     = 0;
    pid.actualtemp  = 0;
    pid.err         = 0;
    pid.err_last    = 0;
    pid.integral    = 0;
    pid.ht_time     = 0;
    pid.kp          = 15;
    pid.ki          = 2;
    pid.kd          = 1;
}

/*******************************************************************************
函数名: Pid_Realize
函数作用: PID自整定加热时间
输入参数:temp当前温度
返回参数:返回数据单位/50us
*******************************************************************************/
u16 Pid_Realize(s16 temp)
{
    u8 index = 0,index1 = 1;
    s16 d_err = 0;

    pid.actualtemp   = temp;
    pid.err          = pid.settemp - pid.actualtemp;    //温差

    if(pid.err >= 500)  index = 0;
    else {
        index = 1;
        pid.integral    += pid.err;//积分项
    }
////////////////////////////////////////////////////////////////////////////////
//降温去积分
    if(pid.settemp < pid.actualtemp) {
        d_err = pid.actualtemp - pid.settemp;
        if(d_err > 20) {
            pid.integral = 0; //过冲5℃
            index1 = 0;
            index = 0;
        }
    }
////////////////////////////////////////////////////////////////////////////////
    if(pid.err <= 30) index1 = 0;
    else index1 = 1;
    pid.ht_time     = pid.kp * pid.err + pid.ki * index * pid.integral + pid.kd * (pid.err - pid.err_last)*index1;
    pid.err_last    = pid.err;

    if(pid.ht_time <= 0)          pid.ht_time = 0;
    else if(pid.ht_time > 30*200) pid.ht_time = 30*200;

    return pid.ht_time;

}

/*******************************************************************************
函数名: Heating_Time
函数作用: 计算加热标志，返回加热时间
输入参数:temp当前温度，wk_temp 工作温度
返回参数:返回数据单位/50us
*******************************************************************************/
u32 Heating_Time(s16 temp,s16 wk_temp)
{
    u32 heat_timecnt;

    pid.settemp = wk_temp;
    if(wk_temp > temp) {
        if(wk_temp - temp >= 18)gHt_flag = 0;//加热
        else gHt_flag = 2;//恒温
    } else {
        if(temp - wk_temp <= 18)gHt_flag = 2;//恒温
        else gHt_flag = 1;//降温
    }

    heat_timecnt = Pid_Realize(temp);//Sub_data * 1000;

    return heat_timecnt;
}
/*******************************************************************************
函数名: Status_Tran
函数作用: 根据按键、温度判断等控制状态转换
输入参数: NULL
返回参数: NULL
*******************************************************************************/
void Status_Tran(void)//状态转换
{
    static u16 init_waitingtime = 0;//初始待机时间标志位: 0=> 未初始化,1=>已初始化
//    static u8 back_prestatus = 0;
    s16 heat_timecnt = 0,wk_temp ;
    u16 mma_active;
    static u8 temo_set_pos=1;
    
    
    switch (Get_CtrlStatus()) {//获取当前状态
        case IDLE://待机状态
            switch(Get_gKey()) {
                case KEY_V1://单按A键，进入控温状态
                    if(gIs_restartkey != 1) {//重启标志位gIs_restartkey不为1
                        if(Read_Vb(1) < 4) {//判断电压是否正常
                            Set_CtrlStatus(TEMP_CTR);//设置当前状态为控温状态
                            init_waitingtime    = 0;//初始化等待计数
                            TEMPSHOW_TIMER  = 0;//初始化计时器
                            UI_TIMER = 0;//图像延时 = 0
                            G6_TIMER = 0;
                        }
                    }
                    break;
                case KEY_V2:////单按B键
                    if(gIs_restartkey != 1) {
        //                Set_CtrlStatus(THERMOMETER);//进入温度计模式
        //                UI_TIMER = 0;
        //                Set_LongKeyFlag(1);
                        TEMP_SET_Pos = 1;//改变参数并保存
                        Display_Str8(0,"                ",0);
                        SetOpt_UI(0);
                        EFFECTIVE_KEY_TIMER = 1000;//退出设置模式时间初始化
                        Clear_Screen();
                                        gSet_opt = HD;
                        Set_CtrlStatus(TEMP_SET);//进入设置模式
                        KD_TIMER = 50; //
                    }
                    break;
                case KEY_CN|KEY_V3://同时按AB两键
                    break;
            }
            if(gIs_restartkey && (KD_TIMER == 0)) {
                gIs_restartkey = 0;//初始化
                Set_gKey(NO_KEY);
            }
            if(Read_Vb(1) == 0) {//电压异常进入报警模式
                if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
                Set_CtrlStatus(ALARM);//ALARM：报警模式
            }
            if(gPre_status != WAIT && gPre_status != IDLE) {//屏幕保护自动黑屏
                G6_TIMER = device_info.idle_time;
                Set_gKey(NO_KEY);
                gPre_status = IDLE;
            }
            break;
        case TEMP_CTR://温控状态，烙铁工作的主要状态
            if(temo_set_pos)//从设置状态回到控温状态
            {
                temo_set_pos = 0;
                Delay_uS(10000000);//延时
                Set_LongKeyFlag(1);
                Set_gKey(NO_KEY);
            }
            switch(Get_gKey()) {
                case KEY_CN|KEY_V1:
                case KEY_CN|KEY_V2://长按任意键
                    Set_HeatingTime(0);//加热时间设置为0，停止加热
                    Clear_Screen();
                    TEMP_SET_Pos = 0;//设置模式只改变临时温度
                    Set_CtrlStatus(TEMP_SET);//进入设置模式
                    Set_gKey(NO_KEY);//清除按键
                    HEATING_TIMER       = 0;//内部加热
                    EFFECTIVE_KEY_TIMER = 1000;//退出设置模式时间初始化
                    KD_TIMER = 150;//按键延时
                    break;
                case KEY_CN|KEY_V3://同时按AB两键
                    Set_HeatingTime(0);
                    Set_LongKeyFlag(0);//设置长按标志位 = 0
                    Set_CtrlStatus(IDLE);//返回待机状态
                    gPre_status = IDLE;
                    gIs_restartkey = 1;//重启标志
                    KD_TIMER = 50; //
                    break;
            }

            if(Read_Vb(1) >= 4) {
                Set_HeatingTime(0);//设置加热时间
                Set_LongKeyFlag(0);
                Set_CtrlStatus(IDLE);//当前状态设置
                gPre_status = TEMP_CTR;
                gIs_restartkey = 1;//软重启标志位1
                KD_TIMER = 50; // 2秒
            }
            if(gTemperatureshowflag)    wk_temp = TemperatureShow_Change(1,device_info.t_work);
            else                        wk_temp = device_info.t_work;
            if(HEATING_TIMER == 0) {//计时结束
                gTemp_data    = Get_Temp(wk_temp);
                heat_timecnt  = Heating_Time(gTemp_data,wk_temp);  //计算加热时间
                Set_HeatingTime(heat_timecnt);
                HEATING_TIMER = HEATINGCYCLE;
            }
            if(Get_HeatingTime() == 0) {
                HEATING_TIMER = 0;
            }

            mma_active = Get_MmaShift();
            if(mma_active == 0) { //MMA_active = 0 ==> static ,MMA_active = 1 ==>move
                if(init_waitingtime == 0) {
                    init_waitingtime    = 1;
                    ENTER_WAIT_TIMER = device_info.wait_time;
                }
                if((init_waitingtime != 0) && (ENTER_WAIT_TIMER == 0)) {
                    gHt_flag     = 0;
                    UI_TIMER     = 0;
                    Set_HeatingTime(0);
                    Set_gKey(0);
                    G6_TIMER = device_info.idle_time;
                    Set_CtrlStatus(WAIT);
                }
            } else {
                init_waitingtime = 0;
            }
            if(Get_AlarmType() > NORMAL_TEMP) {   //////////////////警告
                if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
                Set_CtrlStatus(ALARM);
            }
            break;
        case WAIT://休眠状态
            if(gTemperatureshowflag)    wk_temp = TemperatureShow_Change(1,device_info.t_standby);
            else                        wk_temp = device_info.t_standby;
            if(device_info.t_standby > device_info.t_work) { //休眠温度比工作温度高
                if(gTemperatureshowflag)    wk_temp = TemperatureShow_Change(1,device_info.t_work);
                else                        wk_temp = device_info.t_work;//不再升温保持低的一项温度
            }
            if(HEATING_TIMER == 0) {
                gTemp_data    = Get_Temp(wk_temp);
                heat_timecnt  = Heating_Time(gTemp_data,wk_temp);  //计算加热时间
                Set_HeatingTime(heat_timecnt);
                HEATING_TIMER = 30;
            }

            if(Read_Vb(1) >= 4) {
                Set_HeatingTime(0);
                Set_LongKeyFlag(0);
                Set_CtrlStatus(IDLE);
                G6_TIMER = device_info.idle_time;
                gPre_status = WAIT;
                gIs_restartkey = 1;
                KD_TIMER = 50; // 2秒
            }

            if(G6_TIMER == 0) { //进入待机
                Set_HeatingTime(0);
                Set_LongKeyFlag(0);
                gIs_restartkey = 1;
                KD_TIMER = 200; // 2秒
                gPre_status = WAIT;
                Set_CtrlStatus(IDLE);
            }

            mma_active = Get_MmaShift();
            if(mma_active == 1 || Get_gKey() != 0) {
                UI_TIMER      = 0;
                G6_TIMER      = 0;
                init_waitingtime = 0;
                Set_CtrlStatus(TEMP_CTR);
            }

            if(Get_AlarmType() > NORMAL_TEMP) {   //////////////////警告
                if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
                Set_CtrlStatus(ALARM);
            }
            break;
        case TEMP_SET://温度设置状态
            if(EFFECTIVE_KEY_TIMER == 0) {//时间耗尽
                gCalib_flag = 1;
                gTime[5]=1;
                if(TEMP_SET_Pos == 1)//保存修改的参数
                {
                    Disk_BuffInit();//磁盘数据初始化
                    Config_Analysis();// 启动虚拟U盘
                }
                gCalib_flag = 0;
                temo_set_pos=1;
                if(TEMP_SET_Pos == 0)
                {
                    Set_LongKeyFlag(0);//设置长按标志位 = 0
                    Set_gKey(NO_KEY);
                    Delay_uS(3000000);
                    Set_CtrlStatus(TEMP_CTR);//返回温控状态
                    TEMPSHOW_TIMER = 0;//计时器初始化
                }
                else 
                {
                    Set_HeatingTime(0);
                    Set_LongKeyFlag(0);//设置长按标志位 = 0
                    Set_CtrlStatus(IDLE);//返回待机状态
                    gPre_status = TEMP_CTR;
                    gIs_restartkey = 1;//重启标志
                    KD_TIMER = 100; 
                }
            }
            if(Get_gKey() == (KEY_CN|KEY_V3) && TEMP_SET_Pos == 0){//同时长按AB键推出温度设置状态
    //            gCalib_flag = 1;
    //            Disk_BuffInit();
    //            Config_Analysis();         // 启动虚拟U盘
    //            gCalib_flag = 0;
    //            Set_CtrlStatus(VOLT);
                EFFECTIVE_KEY_TIMER = 0;
                Set_gKey(NO_KEY);
                Set_LongKeyFlag(0);//设置长按标志位 = 0
            }
            
            break;
        case VOLT:
            if(KD_TIMER > 0) {
                Set_gKey(NO_KEY);
                break;
            }
            if(Get_gKey() != NO_KEY){
                EFFECTIVE_KEY_TIMER = 1000;//按键刷新时间
                Set_CtrlStatus(TEMP_CTR);
            }
            break;
    //        case THERMOMETER://温度计模式
    //        if(KD_TIMER > 0) {
    //            Set_gKey(NO_KEY);
    //            break;
    //        }
    //        switch(Get_gKey()) {
    //        case KEY_CN|KEY_V1:
    //        case KEY_CN|KEY_V2:
    //          back_prestatus = 1;
    //            break;
    //        case KEY_CN|KEY_V3:
    //            Zero_Calibration();//校准零点
    //            if(Get_CalFlag() == 1) {
    //                Disk_BuffInit();
    //                Config_Analysis();         // 启动虚拟U盘
    //            }
    //            KD_TIMER = 200; //20150717 修改
    //            break;
    //        default:
    //            break;
    //        }
    //        if(back_prestatus == 1) {
    //            back_prestatus = 0;
    //            Set_HeatingTime(0);
    //            Set_CtrlStatus(IDLE);
    //            gPre_status = THERMOMETER;
    //            gIs_restartkey = 1;
    //            Set_LongKeyFlag(0);
    //            KD_TIMER = 50; //
    //        }
    //        break;
        case ALARM://警告模式
            switch(Get_AlarmType()) {
                case HIGH_TEMP:
                case SEN_ERR:
                    if(gTemperatureshowflag)    wk_temp = TemperatureShow_Change(1,device_info.t_standby);
                    else                        wk_temp = device_info.t_standby;
                    gTemp_data  = Get_Temp(wk_temp);
                    if(Get_AlarmType() == NORMAL_TEMP) {
                        Set_CtrlStatus(TEMP_CTR);
                        Set_UpdataFlag(0);
                    }
                    break;
                case HIGH_VOLTAGE:
                case LOW_VOLTAGE:
                    if(Read_Vb(1) >= 1 && Read_Vb(1) <= 3) {
                        Set_HeatingTime(0);
                        Set_LongKeyFlag(0);
                        gIs_restartkey = 1;
                        UI_TIMER = 2; // 2秒
                        gPre_status = THERMOMETER;
                        Set_CtrlStatus(IDLE);
                    }
                    break;
            }
          if(Get_HeatingTime != 0) {
              Set_HeatingTime(0) ;                          //马上停止加热
              HEAT_OFF();
          }
          break;
          default:
          break;
    }
}

/******************************** END OF FILE *********************************/
