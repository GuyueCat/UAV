#ifndef _OLDX_API_H
#define	_OLDX_API_H
#include "pos_ctrl.h"
#include "usart.h"
#define  MAX_CNT_NUM 35
extern u32 cnt_mission[MAX_CNT_NUM];extern u16 cnt_task[MAX_CNT_NUM];
#define COMMON_INT 0
#define COMMON_CNT 1
#define COMMON_CNT1 2
#define COMMON_CNT2 3
#define SPD_MOVE_CNT 6
#define BODY_MOVE_INIT 7
#define BODY_MOVE_CNT 8
#define DRONE_Z_CNT 9
#define EST_INT 10
#define F_YAW_CNT 11
#define F_YAW_INIT 12
#define MISSION_CNT 13
#define PAN_SEARCH_FLAG 14
#define HOME_CNT 15
#define WAY_INIT 16
#define WAY_CNT 17
#define SET_POS_CNT 18
#define C_SEARCH_INT 19
#define D_DOWN_CNT 20
#define D_DOWN_INT 21
#define F_DOWN_CNT 22
#define F_DOWN_INT 23
#define DELAY_CNT 24
#define LAND_CNT 25
#define LAND_CNT1 26
#define LIGHT_DRAW_CNT1 27
#define LIGHT_DRAW_CNT2 28
#define LIGHT_DRAW_CNT3 29
#define C_SEARCH_T_INT 30
#define C_SEARCH_T_INT1 31
//-----------MODE
#define MODE_FAST_WAY 1
#define NMODE_FAST_WAY 0
#define MODE_SPD 1
#define MODE_POS 0
//-----------------------------------
#define LAND_SPD 0.68//m/s
#define MAX_WAYP_Z 100//m
#define WAY_POINT_DEAD1 0.5 //m
#define LAND_CHECK_G 0.25//g
#define LAND_CHECK_TIME 0.86//s
#define YAW_LIMIT_RATE 10//��
#define DRAW_TIME 5//s
//api
extern u32 cnt_mission[MAX_CNT_NUM];
void init_mission_reg(void);
void track_init(CI tracker);
u8 way_point_task(double lat,double lon, float height,float loter_time,u8 mode,float dt);

u8 spd_move_task(float x,float y,float z,float time,float dt);

u8 pos_move_global_task(float x,float y,float z,float dt);

u8 pos_move_body_task(float x,float y,float z,float dt);

//������ ���루�����߶ȣ�ʹ�ܺ���
u8 back_home_task(float z,u8 yaw_target,float dt);

// ��̨����API�����루�����᣺����0��λ��̨��
u8 pan_set_task(float pitch,float dt);

//��̨������ ���루��С�Ƕȣ����Ƕȣ������Ƕȣ�����������
void pan_search_task(float min,float max,float da,u8 en_yaw,u8 en_forward,float dt);

//���䴹ֱ������  ���루�뾶���߶ȣ������Ƕȣ������뾶��
void down_ward_search_task(float r,float z,float d_angle,float d_r,float dt);

//��������ͷ��׼API�� ���루Ŀ�꣬�½��ٶȣ����ո߶ȣ�ģʽ��
//mode 0->λ�� 1->ͼ�� 2 auto
u8 target_track_downward_task(CI *target,float spdz,float end_z,u8 mode,float dt);
u8 target_track_downward_task_search(CI *target,float spdz,float end_z,float dt);
// ��̨����API�� ���루Ŀ�꣬ʹ�ܱƽ���ʹ�ܸ����ᣬʹ�ܺ��򣬱ƽ�ģʽ��0�ٶ� 1λ�ã���
u8 target_track_pan_task(CI* target,float close_param,u8 en_pitch,u8 en_yaw,u8 close_mode,float dt);
u8 set_drone_yaw_task(float yaw,float rate_limit,float dt);//���÷ɻ�����
u8 set_drone_z_task(float z,float dt);//���÷ɻ��߶�
u8 target_track_forward_task(float dt);
u8 take_off_task(u8 auto_diarm,float dt);
u8 land_task(float dt);
u8 delay_task(float delay,float dt);
u8 set_pos_task(float x,float y,float z,float dt);
float est_target_state(CI* target,_CAR *target_b, float dt);
u8 traj_init_task(float ps[3],float pe[3],float T,u8 sel);
u8 follow_traj(float yaw,u8 sel,u8 mode,float dt);
u8 land_task1(float spd,float dt);	

u8 way_point_mission(float spd_limit,u8 head_to_waypoint,float dt);
u8 return_home(float set_z, float spd_limit, u8 en_land, u8 head_to_home, float dt);
u8 take_off_task1(float set_z,float set_spd,float dt);
void aux_open(u8 sel,u8 open);
u8 avoid_task(CI* target,float set_x,float set_y,float set_z, float dead, float max_spd, float dt);
u8 down_ward_search_task_tangle(float w,float h,int wn,int hn,float spd_limit,float z,float dt);
// light draw simple
u8 draw_heart(float time,float size, float dt);
u8 avoid_way_point_task(CI* target,float tar_lat,float tar_lon,float spd, float dead, float max_spd,u8 mode, float dt);
#endif