
#include "include.h"
#include "flash.h"
#include "ak8975.h"
#include "mpu6050.h"	
#include "mymath.h"	
#include "flash_w25.h"
#include "mavl.h"
//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
 		
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	if(addrx<0X1FFF0000)			//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
				if(status!=FLASH_COMPLETE)break;	//����������
			}else addrx+=4;
		} 
	}
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)//д����
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}
//-----------------------------------------�洢����
#define SIZE_PARAM 50*2
float k_sensitivity[3]={0.78,1,1};//�ж�
float yaw_off_qr=0;
u32 FLASH_SIZE=16*1024*1024;	//FLASH ��СΪ16�ֽ�
u16 LENGTH_OF_DRONE=330;//���������
int H_INT; //��ͣ����
float SONAR_HEIGHT=0.3+0.015;//��������װ�߶�
u8 need_init_mems=0;//mems flash error
TIME time_fly;
u16 SBUS_MIN =954;
u16 SBUS_MAX =2108;
u16 SBUS_MID =1524;
u16 SBUS_MIN_A =954;
u16 SBUS_MAX_A =2108;
u16 SBUS_MID_A =1524;

u8 FLASH_Buffer[SIZE_PARAM]={0};	
void READ_PARM(void)
{
u8 need_init=0;	
#if FLASH_USE_STM32
STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)FLASH_Buffer,SIZE);	
#else	
W25QXX_Read(FLASH_Buffer,FLASH_SIZE-(SIZE_PARAM+10),SIZE_PARAM);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
#endif
mpu6050_fc.Gyro_Offset.x=(vs16)(FLASH_Buffer[1]<<8|FLASH_Buffer[0]);
mpu6050_fc.Gyro_Offset.y=(vs16)(FLASH_Buffer[3]<<8|FLASH_Buffer[2]);
mpu6050_fc.Gyro_Offset.z=(vs16)(FLASH_Buffer[5]<<8|FLASH_Buffer[4]);
	
mpu6050_fc.Acc_Offset.x=(vs16)(FLASH_Buffer[7]<<8|FLASH_Buffer[6]);
mpu6050_fc.Acc_Offset.y=(vs16)(FLASH_Buffer[9]<<8|FLASH_Buffer[8]);
mpu6050_fc.Acc_Offset.z=(vs16)(FLASH_Buffer[11]<<8|FLASH_Buffer[10]);
	
ak8975_fc.Mag_Offset.x=(vs16)(FLASH_Buffer[13]<<8|FLASH_Buffer[12]);
ak8975_fc.Mag_Offset.y=(vs16)(FLASH_Buffer[15]<<8|FLASH_Buffer[14]);
ak8975_fc.Mag_Offset.z=(vs16)(FLASH_Buffer[17]<<8|FLASH_Buffer[16]);
	
ak8975_fc.Mag_Gain.x =(float)((vs16)((FLASH_Buffer[19]<<8|FLASH_Buffer[18])))/100.;
ak8975_fc.Mag_Gain.y=(float)((vs16)((FLASH_Buffer[21]<<8|FLASH_Buffer[20])))/100.;
ak8975_fc.Mag_Gain.z =(float)((vs16)((FLASH_Buffer[23]<<8|FLASH_Buffer[22])))/100.;
	
SONAR_HEIGHT=(float)((vs16)((FLASH_Buffer[25]<<8|FLASH_Buffer[24])))/1000.;	
LENGTH_OF_DRONE=(float)((vs16)((FLASH_Buffer[27]<<8|FLASH_Buffer[26])));

imu_board.flow_module_offset_x=(float)((vs16)((FLASH_Buffer[29]<<8|FLASH_Buffer[28])))/1000.;	
imu_board.flow_module_offset_y=(float)((vs16)((FLASH_Buffer[31]<<8|FLASH_Buffer[30])))/1000.;	
imu_board.k_flow_sel=(float)((vs16)((FLASH_Buffer[33]<<8|FLASH_Buffer[32])))/1000.;	
imu_board.flow_set_yaw=(float)((vs16)((FLASH_Buffer[35]<<8|FLASH_Buffer[34])))/100.;//��ݮ������ͷ��װ�Ƕ�
H_INT=((vs16)((FLASH_Buffer[37]<<8|FLASH_Buffer[36])));
circle.yaw_off=0;

mpu6050_fc.Off_3d.x=(vs16)(FLASH_Buffer[39]<<8|FLASH_Buffer[38]);
mpu6050_fc.Off_3d.y=(vs16)(FLASH_Buffer[41]<<8|FLASH_Buffer[40]);
mpu6050_fc.Off_3d.z=(vs16)(FLASH_Buffer[43]<<8|FLASH_Buffer[42]);
	
mpu6050_fc.Gain_3d.x =(float)((vs16)((FLASH_Buffer[45]<<8|FLASH_Buffer[44])))/1000.;
mpu6050_fc.Gain_3d.y =(float)((vs16)((FLASH_Buffer[47]<<8|FLASH_Buffer[46])))/1000.;
mpu6050_fc.Gain_3d.z =(float)((vs16)((FLASH_Buffer[49]<<8|FLASH_Buffer[48])))/1000.;

mpu6050_fc.att_off[0]=(float)((vs16)((FLASH_Buffer[51]<<8|FLASH_Buffer[50])))/100.;
mpu6050_fc.att_off[1]=(float)((vs16)((FLASH_Buffer[53]<<8|FLASH_Buffer[52])))/100.;


k_sensitivity[0]=(float)((vs16)((FLASH_Buffer[55]<<8|FLASH_Buffer[54])))/100.;
k_sensitivity[1]=(float)((vs16)((FLASH_Buffer[57]<<8|FLASH_Buffer[56])))/100.;

//	
//SBUS_MIN=(vs16)(FLASH_Buffer[59]<<8|FLASH_Buffer[58]);
//SBUS_MAX=(vs16)(FLASH_Buffer[61]<<8|FLASH_Buffer[60]);
//SBUS_MID=(vs16)(FLASH_Buffer[63]<<8|FLASH_Buffer[62]);
////	
//SBUS_MIN_A=(vs16)(FLASH_Buffer[65]<<8|FLASH_Buffer[64]);
//SBUS_MAX_A=(vs16)(FLASH_Buffer[67]<<8|FLASH_Buffer[66]);
//SBUS_MID_A=(vs16)(FLASH_Buffer[69]<<8|FLASH_Buffer[68]);
//if(SBUS_MIN==65535)
//{
SBUS_MIN=860;
SBUS_MID=1524;
SBUS_MAX=2180;

SBUS_MIN_A=644;
SBUS_MID_A=1524;
SBUS_MAX_A=2484;	
//need_init=1;
//}	

k_sensitivity[2]=(float)((vs16)((FLASH_Buffer[71]<<8|FLASH_Buffer[70])))/100.;
eso_att_inner_c[PITr].b0=eso_att_inner_c[ROLr].b0=0;//(float)((vs16)((FLASH_Buffer[73]<<8|FLASH_Buffer[72])))/10.;
if(eso_att_inner_c[PITr].b0>40||eso_att_inner_c[PITr].b0<10)
	eso_att_inner_c[PITr].b0=eso_att_inner_c[ROLr].b0=0;
time_fly.longe=(float)((vs16)((FLASH_Buffer[75]<<8|FLASH_Buffer[74])));
time_fly.shorte=(float)((vs16)((FLASH_Buffer[77]<<8|FLASH_Buffer[76])));

yaw_off_qr=(float)((vs16)((FLASH_Buffer[79]<<8|FLASH_Buffer[78])));
//----------------------------------------------------------
if(k_sensitivity[0]<=0||k_sensitivity[0]>22)
	k_sensitivity[0]=1;
if(k_sensitivity[1]<=0||k_sensitivity[1]>22)
	k_sensitivity[1]=1;
if(k_sensitivity[2]<=0||k_sensitivity[2]>22)
	k_sensitivity[2]=1;

if(LENGTH_OF_DRONE<100||LENGTH_OF_DRONE>1200){
 LENGTH_OF_DRONE=330;//���������
 need_init=1;	
}

if(SONAR_HEIGHT<0||SONAR_HEIGHT>0.5){
	SONAR_HEIGHT=0.054+0.015;
	 need_init=1;	
 }

if(ABS(mpu6050_fc.Acc_Offset.x)<10&&ABS(mpu6050_fc.Acc_Offset.y)<10&&ABS(mpu6050_fc.Acc_Offset.z)<10){
	need_init_mems=1;	
}

 if(need_init)
	 WRITE_PARM();
}

void WRITE_PARM(void)
{ 

int16_t _temp;
u8 cnt=0;
//u8 FLASH_Buffer[SIZE_PARAM]={0};
_temp=(int16_t)mpu6050_fc.Gyro_Offset.x;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Gyro_Offset.y;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Gyro_Offset.z;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)mpu6050_fc.Acc_Offset.x;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Acc_Offset.y;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Acc_Offset.z;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)ak8975_fc.Mag_Offset.x;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)ak8975_fc.Mag_Offset.y;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)ak8975_fc.Mag_Offset.z;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);


_temp=(int16_t)(ak8975_fc.Mag_Gain.x*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(ak8975_fc.Mag_Gain.y*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(ak8975_fc.Mag_Gain.z*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(SONAR_HEIGHT*1000);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=LENGTH_OF_DRONE;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);


//-----------imu para
_temp=imu_board.flow_module_offset_x*1000;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=imu_board.flow_module_offset_y*1000;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=imu_board.k_flow_sel*1000;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=imu_board.flow_set_yaw*100;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=H_INT;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);



_temp=(int16_t)mpu6050_fc.Off_3d .x;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Off_3d.y;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)mpu6050_fc.Off_3d.z;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(mpu6050_fc.Gain_3d.x*1000);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(mpu6050_fc.Gain_3d.y*1000);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(mpu6050_fc.Gain_3d.z*1000);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(mpu6050_fc.att_off[0]*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(mpu6050_fc.att_off[1]*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(k_sensitivity[0]*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(k_sensitivity[1]*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);


_temp=(int16_t)SBUS_MIN;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)SBUS_MAX;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)SBUS_MID;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)SBUS_MIN_A;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)SBUS_MAX_A;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)SBUS_MID_A;
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(k_sensitivity[2]*100);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(eso_att_inner_c[PITr].b0*10);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(time_fly.longe);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
_temp=(int16_t)(time_fly.shorte);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);

_temp=(int16_t)(yaw_off_qr);
FLASH_Buffer[cnt++]=BYTE0(_temp);
FLASH_Buffer[cnt++]=BYTE1(_temp);
#if FLASH_USE_STM32
STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)FLASH_Buffer,SIZE);
#else
W25QXX_Write((u8*)FLASH_Buffer,FLASH_SIZE-(SIZE_PARAM+10),SIZE_PARAM);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
#endif
}


#define FRAM_SIZE 16
#define SIZE_WAY FRAM_SIZE*(NAV_MAX_MISSION_LEGS+1)
u8 FLASH_Bufferw[SIZE_WAY]={0};		
void WRITE_PARM_WAY_POINTS(void)
{ 
int16_t _temp;
int32_t	_temp32;
u16 cnt=0,i;

int max_num=LIMIT(SIZE_WAY/FRAM_SIZE-2,0,NAV_MAX_MISSION_LEGS);
FLASH_Bufferw[cnt++]=navData.Leg_num;
for(i=0;i<LIMIT(navData.Leg_num,0,max_num);i++){
_temp32=(int32_t)(navData.missionLegs[i].targetLat*10000000);
FLASH_Bufferw[cnt++]=BYTE0(_temp32);
FLASH_Bufferw[cnt++]=BYTE1(_temp32);
FLASH_Bufferw[cnt++]=BYTE2(_temp32);
FLASH_Bufferw[cnt++]=BYTE3(_temp32);
_temp32=(int32_t)(navData.missionLegs[i].targetLon*10000000);
FLASH_Bufferw[cnt++]=BYTE0(_temp32);
FLASH_Bufferw[cnt++]=BYTE1(_temp32);
FLASH_Bufferw[cnt++]=BYTE2(_temp32);
FLASH_Bufferw[cnt++]=BYTE3(_temp32);
_temp=(int16_t)(navData.missionLegs[i].targetAlt*10);
FLASH_Bufferw[cnt++]=BYTE0(_temp);
FLASH_Bufferw[cnt++]=BYTE1(_temp);
_temp=(int16_t)(navData.missionLegs[i].poiHeading*1000);
FLASH_Bufferw[cnt++]=BYTE0(_temp);
FLASH_Bufferw[cnt++]=BYTE1(_temp);
_temp=(int16_t)(navData.missionLegs[i].maxHorizSpeed*100);
FLASH_Bufferw[cnt++]=BYTE0(_temp);
FLASH_Bufferw[cnt++]=BYTE1(_temp);
_temp=(int16_t)(navData.missionLegs[i].loiterTime*100);
FLASH_Bufferw[cnt++]=BYTE0(_temp);
FLASH_Bufferw[cnt++]=BYTE1(_temp);
}

W25QXX_Write((u8*)FLASH_Bufferw,FLASH_SIZE-(SIZE_WAY+10+SIZE_PARAM+10),SIZE_WAY);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
}

void READ_WAY_POINTS(void)
{
u16 i;
W25QXX_Read(FLASH_Bufferw,FLASH_SIZE-(SIZE_WAY+10+SIZE_PARAM+10),SIZE_WAY);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
navData.Leg_num=LIMIT(FLASH_Bufferw[0],0,NAV_MAX_MISSION_LEGS);
for(i=0;i<navData.Leg_num;i++){
navData.missionLegs[i].targetLat=(float)((vs32)(FLASH_Bufferw[4+i*FRAM_SIZE]<<24|FLASH_Bufferw[3+i*FRAM_SIZE]<<16|
	FLASH_Bufferw[2+i*FRAM_SIZE]<<8|FLASH_Bufferw[1+i*FRAM_SIZE]))/10000000.;
navData.missionLegs[i].targetLon=(float)((vs32)(FLASH_Bufferw[8+i*FRAM_SIZE]<<24|FLASH_Bufferw[7+i*FRAM_SIZE]<<16|
	FLASH_Bufferw[6+i*FRAM_SIZE]<<8|FLASH_Bufferw[5+i*FRAM_SIZE]))/10000000.;
navData.missionLegs[i].targetAlt=(float)((vs16)(FLASH_Bufferw[10+i*FRAM_SIZE]<<8|FLASH_Bufferw[9+i*FRAM_SIZE]))/10.;
navData.missionLegs[i].poiHeading=(float)((vs16)(FLASH_Bufferw[12+i*FRAM_SIZE]<<8|FLASH_Bufferw[11+i*FRAM_SIZE]))/1000.;
navData.missionLegs[i].maxHorizSpeed=(float)((vs16)(FLASH_Bufferw[14+i*FRAM_SIZE]<<8|FLASH_Bufferw[13+i*FRAM_SIZE]))/100.;
navData.missionLegs[i].loiterTime=(float)((vs16)(FLASH_Bufferw[16+i*FRAM_SIZE]<<8|FLASH_Bufferw[15+i*FRAM_SIZE]))/100.;
if(navData.missionLegs[i].targetLat>20&&navData.missionLegs[i].targetLon>20)
	navData.missionLegs[i].type=2;
}
}