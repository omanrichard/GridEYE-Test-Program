//
//  wiringPiI2C.hpp
//  GridEYE Test Program
//
//  Created by Richard Oman on 12/1/17.
//  Copyright © 2017 Richard Oman. All rights reserved.
//

#ifndef wiringPiI2C_hpp
#define wiringPiI2C_hpp

#include <stdio.h>


int wiringPiI2CWrite( int fd, int cmd );

int wiringPiI2CSetup( int address );

int wiringPiI2CWriteReg16( int fd, int reg, int cmd );
int wiringPiI2CReadReg16( int fd, int reg);  // Receive value from device, end transmission
#endif /* wiringPiI2C_hpp */
