#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-07-25 12:59:29

#include "Arduino.h"
#include "Arduino.h"
#include "math.h"

void setup() ;
void loop() ;
void waitForRising();
void readData();
double getAverage(unsigned int array[], int length);
double getVariance(double average, unsigned int array[], int length);
double getVariance(unsigned int array[], int length);
void checkErrors(int state);
void printReceivedData();
void sendPower(int time);
void BERTest();
void continuousReadTest();
void read60();
double calculateVarianceAverage(unsigned int val);

#include "Thesis_ReaderCodeArduinoDue.ino"


#endif
