/*
 * BMP180.h
 *
 *  Created on: Oct 30, 2022
 *      Author: yunus
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_




#define BMP_ADR         0xEE
#define CALIB_ADDR		0xAA

void CalibRead();
uint16_t ReadTemp();
float GetTemp();
uint32_t ReadPress(int oss);
float GetPress(int oss);
float GetAltitude(int oss);

#define AtmPress 101325


#endif /* INC_BMP180_H_ */
