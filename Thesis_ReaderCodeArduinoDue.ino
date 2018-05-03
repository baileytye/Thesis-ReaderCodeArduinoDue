/* File: Reader.ino
 * Created by: Bailey Tye
 * Date Created: 3/22/2018
 * Last Edited: 5/3/2018
 */



#include "Arduino.h"



//-----------------------DEFINES------------------------//
#define SIGNAL_PIN A0

#define DATA_CONTROL_PIN 52

#define POWER_CONTROL_PIN 53



//ADC value of offset you consider to be a rising edge
#define RISING_OFFSET 35

//Variance of readings[] you consider to be on
#define VARIANCE_THRESHOLD 100

//Interval between bits of data
#define BIT_INTERVAL 1277

//Start interval offset
#define START_INTERVAL_OFFSET 1362

//Number of bits to receive
#define BYTES_TO_RECEIVE 4

//Set to 1 to read 60 ADC values and print them to the monitor
#define READ_60 0


unsigned int readings[15];

byte data[BYTES_TO_RECEIVE];



//-------------------INITIALIZATION--------------------//

void setup()
{
// Add your initialization code here
	Serial.begin(115200);

//Make sure the ADC is running in its fastest mode
	int reg = REG_ADC_MR;
	Serial.print("REG_ADC_MR = ");
	Serial.println(reg, HEX);

	REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00020000;
	Serial.print("Changed to REG_ADC_MR = ");
	Serial.println(reg, HEX);

//Set resolution to 12 bits
	analogReadResolution(12);
	pinMode(SIGNAL_PIN, INPUT);
	pinMode(DATA_CONTROL_PIN, OUTPUT);
	pinMode(POWER_CONTROL_PIN, OUTPUT);

}

//-----------------END OF INITIALIZATION-----------------//


//---------------------MAIN LOOP-------------------------//


void loop()
{

	uint32_t startTime;
	uint32_t endTime;





	if(READ_60){
		unsigned int r[60];
		for(int i = 0; i < 60; i++){
			r[i] = analogRead(SIGNAL_PIN);
		}

		//Display values
		for(int i = 0; i < 60; i ++){
			Serial.println(r[i]);
		}

		//Only execute once
		while(1);
	} else {

		while(1){

//			//Turn off data signal pass-through
//			digitalWrite(DATA_CONTROL_PIN, LOW);
//
//			//Turn on function generator for power transmission
//			digitalWrite(POWER_CONTROL_PIN, HIGH);
//
//			//Wait enough time to charge the tag
//			delay(5000);
//
//			//Turn off function generator
//			digitalWrite(POWER_CONTROL_PIN, LOW);
//
			//Turn on data signal pass-through
			digitalWrite(DATA_CONTROL_PIN, HIGH);


			//Wait till a rising edge is detected
			Serial.println("//---------Waiting for Data--------// ");
			waitForRising();
			startTime = micros();

			//Serial.println(readings[1] - readings[0]);

			//Make sure its real
			readings[0] = analogRead(SIGNAL_PIN);
			readings[1] = analogRead(SIGNAL_PIN);
			readings[2] = analogRead(SIGNAL_PIN);
			readings[3] = analogRead(SIGNAL_PIN);
			readings[4] = analogRead(SIGNAL_PIN);
			readings[5] = analogRead(SIGNAL_PIN);
			readings[6] = analogRead(SIGNAL_PIN);
			readings[7] = analogRead(SIGNAL_PIN);
			readings[8] = analogRead(SIGNAL_PIN);
			readings[9] = analogRead(SIGNAL_PIN);
			readings[10] = analogRead(SIGNAL_PIN);
			readings[11] = analogRead(SIGNAL_PIN);



			if(getVariance(readings, 12) < VARIANCE_THRESHOLD){
				continue;
			}

			//Serial.println(getVariance(readings, 6));

			//Wait till data starts
			while((micros() - startTime) < (START_INTERVAL_OFFSET));

			//Read data
			readData();


			for(int i = 0; i < BYTES_TO_RECEIVE;i++){
				Serial.print(data[i],HEX);
				Serial.print(" ");
			}

			Serial.print("\n");


			for(int i = 0; i < BYTES_TO_RECEIVE; i++){
				Serial.print(char(data[i]));
			}

			Serial.print("\n");
			delay(500);
		}

	}

	delay(1);


}
//--------------------END OF MAIN LOOP--------------------//




//-----------------------FUNCTIONS------------------------//

//Waits till a rising edge is detected
void waitForRising(){

	do {
		readings[0] = analogRead(SIGNAL_PIN);
		readings[1] = analogRead(SIGNAL_PIN);
		readings[1] = analogRead(SIGNAL_PIN);
		readings[1] = analogRead(SIGNAL_PIN);

	} while(readings[1] < (readings[0] + RISING_OFFSET));

}

//Reads a byte bits of data
void readData(){

	uint32_t startTime;

	for(int j = 0; j < BYTES_TO_RECEIVE; j++){
	for(int i = 0; i < 8; i++){

		startTime = micros();

		readings[0] = analogRead(SIGNAL_PIN);
		readings[1] = analogRead(SIGNAL_PIN);
		readings[2] = analogRead(SIGNAL_PIN);
		readings[3] = analogRead(SIGNAL_PIN);
		readings[4] = analogRead(SIGNAL_PIN);
		readings[5] = analogRead(SIGNAL_PIN);
		readings[6] = analogRead(SIGNAL_PIN);
		readings[7] = analogRead(SIGNAL_PIN);
		readings[8] = analogRead(SIGNAL_PIN);
		readings[9] = analogRead(SIGNAL_PIN);
		readings[10] = analogRead(SIGNAL_PIN);
		readings[11] = analogRead(SIGNAL_PIN);
		readings[12] = analogRead(SIGNAL_PIN);
		readings[13] = analogRead(SIGNAL_PIN);
		readings[14] = analogRead(SIGNAL_PIN);


		Serial.println(getVariance(readings, 15));

		if(getVariance(readings, 15) > VARIANCE_THRESHOLD)
			data[j] |= 1 << (7 - i);

		//Serial.println(micros()-startTime);
		while((micros() - startTime) < BIT_INTERVAL);

	}
	}


}

//Gets the average of array
double getAverage(unsigned int array[], int length){

	double average = 0;

	for(int i = 0; i < length; i++){
		average += array[i];
	}

	average /= length;

	return average;
}

//Gets the variance of array, given the average
double getVariance(double average, unsigned int array[], int length){

	double variance = 0;
	for(int i = 0; i < length; i++){
		variance += abs((double)array[i] - average);
	}
	return variance;
}

//Calculates average then gets the variance of array
double getVariance(unsigned int array[], int length){

	double average = getAverage(array, length);

	double variance = 0;
	for(int i = 0; i < length; i++){
		variance += abs((double)array[i] - average);
	}
	return variance;
}





