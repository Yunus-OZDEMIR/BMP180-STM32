/*
 * BMP180.c
 *
 *  Created on: 31 Eki 2022
 *      Author: yunus
 */


#include "stm32f4xx_hal.h"
#include "BMP180.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

short AC1,AC2,AC3,B1,B2,MB,MC,MD;
unsigned short AC4,AC5,AC6;
long UT,UP,X1,X2,X3,B5,B3,T,p,B6;
unsigned long B4,B7;

uint8_t Callib_Data[22];
uint8_t temp_raw[2];
uint8_t press_raw[3];

void CalibRead(){
	HAL_I2C_Mem_Read(&hi2c1, BMP_ADR, CALIB_ADDR, 1, Callib_Data, 22, 1000);
	AC1 = ((Callib_Data[0] << 8) | Callib_Data[1]);
	AC2 = ((Callib_Data[2] << 8) | Callib_Data[3]);
	AC3 = ((Callib_Data[4] << 8) | Callib_Data[5]);
	AC4 = ((Callib_Data[6] << 8) | Callib_Data[7]);
	AC5 = ((Callib_Data[8] << 8) | Callib_Data[9]);
	AC6 = ((Callib_Data[10] << 8) | Callib_Data[11]);
	B1 =  ((Callib_Data[12] << 8) | Callib_Data[13]);
	B2 =  ((Callib_Data[14] << 8) | Callib_Data[15]);
	MB =  ((Callib_Data[16] << 8) | Callib_Data[17]);
	MC =  ((Callib_Data[18] << 8) | Callib_Data[19]);
	MD =  ((Callib_Data[20] << 8) | Callib_Data[21]);
}

uint16_t ReadTemp(){
	uint8_t WriteValue = 0x2E;
	HAL_I2C_Mem_Write(&hi2c1, BMP_ADR, 0xF4, 1, &WriteValue, 1, 1000);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1, BMP_ADR, 0xF6, 1, temp_raw, 2, 1000);
	return ((temp_raw[0]<<8) + temp_raw[1]);
}

float GetTemp(){
	UT = ReadTemp();
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	T  = (B5+8)/(pow(2,4));
	return T/10.0;
}

uint32_t ReadPress(int oss){
	uint8_t WriteValue = 0x34 + (oss<<6);
	HAL_I2C_Mem_Write(&hi2c1, BMP_ADR, 0xF4, 1, &WriteValue, 1, 1000);
	if (oss == 0) HAL_Delay(5);
	else if (oss == 1)HAL_Delay(8);
	else if (oss == 2)HAL_Delay(14);
	else if (oss == 3)HAL_Delay(26);
	HAL_I2C_Mem_Read(&hi2c1, BMP_ADR, 0xF6, 1, press_raw, 3, 1000);
	return (((press_raw[0]<<16) + (press_raw[1]<<8) + (press_raw[2])) >> (8-oss));
}

float GetPress(int oss){

	UP = ReadPress(oss);
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	/**********************************/
	B6= B5-4000;
	X1 = (B2*(B6*B6/pow(2,12))/pow(2,11));
	X2 = AC2*B6/pow(2,11);
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1*(B6*B6/pow(2,12)))/pow(2,16);
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4* (unsigned long)(X3+32768)/pow(2,15);
	B7 = ((unsigned long)UP-B3)*(50000>>oss);
	if (B7<0x80000000)p=(B7*2)/B4;
	else p = (B7/B4)*2;
	X1 = (p / pow(2,8)) * (p /pow(2,8));
	X1 = (X1*3038)/ pow(2,16);
	X2 = (-7357*p)/pow(2,16);
	p = p+(X1+X2+3791)/pow(2,4);

	return p;
}

float GetAltitude(int oss){
	float Press;
     Press = GetPress(0);
     return 44330*(1-pow(((float)Press/(float)AtmPress),0.19029495718));
}




