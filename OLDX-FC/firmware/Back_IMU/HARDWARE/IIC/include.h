#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stm32f4xx.h>	
#include "ekf_ins.h"
#include "time.h"
#include "mpu6050.h"
#include "parameter.h"
#include "delay.h" 
#include "malloc.h"   
#include "usart_fc.h"   
#include "spi_nrf.h"							
#include "rc_mine.h"						
#include "nrf.h"							
#include "ultrasonic.h"
#include "rc.h"
#include "att.h"
#include "height_ctrl.h"
#include "flash.h"
#include "dma.h"
#define DRONE_330X6
//#define DRONE_330

#if defined(DRONE_330X6)
#define USE_OUTER_LINK 0 //ʹ�ô���1��Ϊ�ⲿ2������
#define USE_GPS_U4 0     //ʹ�ô���IMU��Ϊ��GPS
#define USE_UWB_AS_POS 0//ʹ�� UWB��Ϊȫ��λ�ò���
//#define USE_WIFI_CONTROL  //ʹ������ԭ��wifiģ��
#define NO_HML_CORRECT 1
#endif

#if defined(DRONE_330)
#define USE_OUTER_LINK 1 //ʹ�ô���1��Ϊ�ⲿ2������
#define USE_GPS_U4 1     //ʹ�ô���IMU��Ϊ��GPS
#define USE_UWB_AS_POS 1//ʹ�� UWB��Ϊȫ��λ�ò���
//#define USE_WIFI_CONTROL  //ʹ������ԭ��wifiģ��
#endif

//================SYSTEM==================
#define NVIC_GROUP NVIC_PriorityGroup_2		//�жϷ���ѡ��
#define EN_DMA_UART1 0  //UPLOAD
#define EN_DMA_UART2 1  //GOL_LINK
#define EN_DMA_UART3 0  //GPS
#define EN_DMA_UART4 0 //SD
/***********************************************/
//================SENSORS===================
#define IMU_UPDATE 1  //ʹ��LSD IMU
#define USE_MINI_BOARD 1 //ʹ��LSD IMU
#define NEW_IMU 1  //ʹ��LSD IMU

#define USE_VER_5 0//Finalv1Ӳ�� IIC mems
#define USE_VER_4 1//���ɰ� SPI mems  FinalV2
#if USE_VER_4
#define USE_VER_3 1//SPI mems
#else
#define USE_VER_3 0//��һ�� IIC mems
#endif

//------------FUSION PARAM----------
#define PX4_USE_FLOW 1 //px4�������ù���
#define USE_UKF_FROM_AUTOQUAD 0
//��̬λ���ں�ʹ��UKF
#if USE_UKF_FROM_AUTOQUAD
#define UKF_IN_ONE_THREAD 0
//����һ���߳����������ں�
#else
#define UKF_IN_ONE_THREAD 0 
#endif

#define USE_M100_IMU 0  //ʹ��DJI SDK����
#define USE_LASER_AVOID 0

#define USE_CYCLE_HML_CAL  1//1->ʹ���������
#define GET_TIME_NUM 	(30)		//���û�ȡʱ�����������
//-------------SONAR PARAM-----------
//#define USE_LIDAR             //ʹ�ñ�xing�״�
#define USE_US100           //ʹ��us100�ͺų����� 
//#define URM07
//#define USE_KS103					//ʹ��ks103�ͺų�����
//#define SONAR_SAMPLE1					//0-5m 32ms  no fix
//#define SONAR_SAMPLE2					//0-5m 100ms T fixf
#define SONAR_SAMPLE3					//0-11m 68ms no fix
//#define SONAR_USE_SCL  
//#define SONAR_USE_TIG
#define SONAR_USE_UART 

#define SONAR_HEIGHT 80
#if !defined(USE_WIFI_CONTROL)
#define USE_IMU_BACK_IO_AS_SONAR 0 //SONAR�ڹ����³������ɼ�
#endif
//---------------------FLOW PARAM---------------------------------
#define FLOW_SET_ANGLE 0 //������װ�Ƕ�ANO
#define FLOW_SET_ANGLE1 180*0.0173 //ȫ�ֹ�����ת�Ƕ�
#define SONAR_USE_FLOW 0 //ʹ��PixFLow�����ĳ�����
#define SENSOR_FORM_PI_FLOW_SONAR_NOT 1 //ʹ��OLDX-AMF���Ǹ߶�ʹ�ó�����

#define SENSOR_FORM_PI_FLOW 0  //ʹ��OLDX-AMF ��ά��SLAMģ��
#define USE_ANO_FLOW 0 //ʹ����������
#define FLOW_USE_IIC 0  //Px4 IIc ����
#define FLOW_USE_OPENMV 0//Openmv Oldx ����
#define USE_FLOW_FLY_ROBOT 1//ʹ�÷���ʵ���ҵĹ���ģ��


//===================PARAM==================
#define PI                     3.14159265359f //Բ����
#define RAD_DEG            		 57.2957795056f //����ת���ɽǶȵı�������
#define DEG_RAD                0.01745329252f //�Ƕ�ת���ɻ��ȵı�������
#define GRAVITY_MSS                  9.80665f //�����������ٶ�
#define earthRate                0.000072921f //������ת���ٶ�
#define earthRadius                6378145.0f //����뾶
#define earthRadiusInv          1.5678540e-7f //����뾶�ĵ���

#define OFFSET_AV_NUM 	50					//У׼ƫ����ʱ��ƽ��������
#define FILTER_NUM 			10					//����ƽ���˲���ֵ����

#define TO_ANGLE 				0.06103f 		//0.061036 //   4000/65536  +-2000   ???

#define FIX_GYRO_Y 			1.02f				//������Y����в���
#define FIX_GYRO_X 			1.02f				//������X����в���

#define TO_M_S2 				0.23926f   	//   980cm/s2    +-8g   980/4096
#define ANGLE_TO_RADIAN 0.01745329f //*0.01745 = /57.3	�Ƕ�ת����

#define MAX_ACC  4096.0f						//+-8G		���ٶȼ�����
#define TO_DEG_S 500.0f      				//T = 2ms  Ĭ��Ϊ2ms ����ֵ����1/T

#define RtA 		57.324841				
#define AtR    	0.0174533				
#define Acc_G 	0.0011963				
#define Gyro_G 	0.03051756				
#define Gyro_Gr	0.0005426

#define A_X 0
#define A_Y 1
#define A_Z 2
#define G_Y 3
#define G_X 4
#define G_Z 5
#define TEM 6
#define ITEMS 7

#define ROLr 0
#define PITr 1
#define THRr 2
#define YAWr 3
#define AUX1r 4 
#define AUX2r 5
#define AUX3r 6
#define AUX4r 7

enum
{
	SETBIT0 = 0x0001,  SETBIT1 = 0x0002,	SETBIT2 = 0x0004,	 SETBIT3 = 0x0008,
	SETBIT4 = 0x0010,	 SETBIT5 = 0x0020,	SETBIT6 = 0x0040,	 SETBIT7 = 0x0080,
	SETBIT8 = 0x0100,	 SETBIT9 = 0x0200,	SETBIT10 = 0x0400, SETBIT11 = 0x0800,
	SETBIT12 = 0x1000, SETBIT13 = 0x2000,	SETBIT14 = 0x4000, SETBIT15 = 0x8000		
};
//CLR BIT.    Example: a &= CLRBIT0
enum
{
	CLRBIT0 = 0xFFFE,  CLRBIT1 = 0xFFFD,	CLRBIT2 = 0xFFFB,	 CLRBIT3 = 0xFFF7,	
	CLRBIT4 = 0xFFEF,	 CLRBIT5 = 0xFFDF,	CLRBIT6 = 0xFFBF,	 CLRBIT7 = 0xFF7F,
	CLRBIT8 = 0xFEFF,	 CLRBIT9 = 0xFDFF,	CLRBIT10 = 0xFBFF, CLRBIT11 = 0xF7FF,
	CLRBIT12 = 0xEFFF, CLRBIT13 = 0xDFFF,	CLRBIT14 = 0xBFFF, CLRBIT15 = 0x7FFF
};
//CHOSE BIT.  Example: a = b&CHSBIT0
enum
{
	CHSBIT0 = 0x0001,  CHSBIT1 = 0x0002,	CHSBIT2 = 0x0004,	 CHSBIT3 = 0x0008,
	CHSBIT4 = 0x0010,	 CHSBIT5 = 0x0020,	CHSBIT6 = 0x0040,	 CHSBIT7 = 0x0080,
	CHSBIT8 = 0x0100,	 CHSBIT9 = 0x0200,	CHSBIT10 = 0x0400, CHSBIT11 = 0x0800,
	CHSBIT12 = 0x1000, CHSBIT13 = 0x2000,	CHSBIT14 = 0x4000, CHSBIT15 = 0x8000		
};

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))


//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //��� 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //����

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //��� 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //����

typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */


typedef struct 
{ u8 pi_flow;
	u8 fc;
	u8 acc;
	u8 gyro;
	u8 hml,outer_hml;
	u8 gps;
	u8 sonar;
	u8 flow;
	u8 flow_iic;
	u8 laser;
	u8 dji;
}SYSTEM;
extern u8 fly_ready,force_Thr_low;
extern SYSTEM module;
#endif
