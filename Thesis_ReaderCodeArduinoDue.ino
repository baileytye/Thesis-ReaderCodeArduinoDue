/* File: Reader.ino
 * Created by: Bailey Tye
 * Date Created: 3/22/2018
 * Last Edited: 5/31/2018
 */



#include "Arduino.h"



//-----------------------DEFINES------------------------//
#define SIGNAL_PIN A0

#define DATA_CONTROL_PIN 52

#define POWER_CONTROL_PIN 53



//ADC value of offset you consider to be a rising edge
#define RISING_OFFSET 17

//Variance of readings[] you consider to be on
#define VARIANCE_THRESHOLD 90

//Interval between bits of data
#define BIT_INTERVAL 1260

//Start interval offset
#define START_INTERVAL_OFFSET 1345

//Number of bits to receive
#define BYTES_TO_RECEIVE 5


//-----------------------MODES---------------------------//

//Set to 1 to read 60 ADC values and print them to the monitor
#define READ_60 0

//Set to 1 for BER test mode
#define BER_TEST 1

//-------------------------------------------------------//


//---------------------USER INPUTS-----------------------//

//Set to: 1 for only important messages, 2 for more messages, 3 for all
#define DEBUG_LEVEL 2

//Distance in feet
#define DISTANCE 4

//User power adjustment to counteract the pipe detuning effect
#define PIPE_POWER_ADJUST 2.8

//Number of runs for the BER test
#define NUMBER_OF_TESTS 500
//-------------------------------------------------------//


//-------------------GLOBAL VERIABLES--------------------//
unsigned int readings[15];

byte data[BYTES_TO_RECEIVE];

unsigned int averageOfOne[NUMBER_OF_TESTS];

unsigned int averageOfZero[NUMBER_OF_TESTS];

unsigned int test = 0;

//BER states to test
byte BER_STATE_1[] = {0x00, 0x00, 0x00, 0x00, 0x80};
byte BER_STATE_2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x80};
byte BER_STATE_3[] = {0x55, 0x55, 0x55, 0x55, 0x80};
byte BER_STATE_4[] = {0xF0, 0xF0, 0xF0, 0xF0, 0x80};
byte BER_STATE_5[] = {0x6F, 0x70, 0x65, 0x6E, 0x80};
byte BER_STATE_6[] = {0x21, 0x40, 0x23, 0x25, 0x00};

//total bit errors
int bitErrors = 0;
//--------------------------------------------------------//


//--------------------INITIALIZATION----------------------//

void setup()
{
// Add your initialization code here
	Serial.begin(115200);

//Make sure the ADC is running in its fastest mode
	REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00020000;


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

	if(READ_60) {
		if(DEBUG_LEVEL >= 1)
			Serial.println("Mode set to Read 60");
		read60();
	} else if(BER_TEST) {
		if(DEBUG_LEVEL >= 1)
			Serial.println("Mode set to BER test");
		BERTest();
	} else {
		if(DEBUG_LEVEL >= 1)
			Serial.println("Mode set to Continuous read");
		continuousReadTest();
	}

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
		readings[1] = analogRead(SIGNAL_PIN);


	} while(readings[1] < (readings[0] + RISING_OFFSET));

}

//Reads a byte bits of data
void readData(){

	uint32_t startTime;
	unsigned int one = 0;
	unsigned int zero = 0;
	unsigned int variance = 0;
	unsigned int ones = 0;

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

		if(DEBUG_LEVEL >= 3)
			Serial.println(getVariance(readings, 15));

		variance = getVariance(readings, 15);

		if(variance > VARIANCE_THRESHOLD){
			data[j] |= 1 << (7 - i);
			one += variance;
			ones++;
		} else {
			zero += variance;
		}

		averageOfOne[test] = (double)one/(ones);
		averageOfZero[test] = (double)zero/((BYTES_TO_RECEIVE * 8) - ones);

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

//Check for errors during BER test
void checkErrors(int state){
	int mask = 1;

	switch(state){
	case 1:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_1[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	case 2:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_2[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	case 3:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_3[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	case 4:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_4[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	case 5:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_5[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	case 6:
		for(int i = 0; i < (BYTES_TO_RECEIVE - 1); i++){
			for(int j = 0; j < 8; j++){
				if((data[i] & (mask << j)) != (BER_STATE_6[i] & (mask << j))){
					bitErrors++;
				}
			}
		}
		break;
	default:
		Serial.println("ERROR, INCORRECT STATE");
		break;
	}
	if(DEBUG_LEVEL >= 2){
		Serial.print("Total Bit Errors: ");
		Serial.print(bitErrors);
		Serial.print("\n");
	}

	return;
}

//Print received data to serial, prints in hex and ascii
void printReceivedData(){

	Serial.print("0x");
	for(int i = 0; i < BYTES_TO_RECEIVE;i++){
		Serial.print(data[i],HEX);
		Serial.print(" ");
	}

	Serial.print("\nASCII: ");


	for(int i = 0; i < BYTES_TO_RECEIVE - 1; i++){
		Serial.print(char(data[i]));
	}

	Serial.print("\n");
}

//Send power for 'time' time
void sendPower(int time){

	//Turn off data signal pass-through
	digitalWrite(DATA_CONTROL_PIN, LOW);

	//Turn on function generator for power transmission
	digitalWrite(POWER_CONTROL_PIN, HIGH);

	//Wait enough time to charge the tag
	delay(time);

	//Turn off function generator
	digitalWrite(POWER_CONTROL_PIN, LOW);

	//Turn on data signal pass-through
	digitalWrite(DATA_CONTROL_PIN, HIGH);
}

//Perform BER test
void BERTest(){

	uint32_t startTime;
	int state = 1;
	int numberOfTests = NUMBER_OF_TESTS;
	int testsRemaining = numberOfTests;

	int sendTime = (int)((DISTANCE*DISTANCE*DISTANCE)*20.0 * PIPE_POWER_ADJUST);
	uint32_t time = millis();



	if(DEBUG_LEVEL >= 1){
		Serial.print("Recharge time between tests: ");
		Serial.print((int)((DISTANCE*DISTANCE*DISTANCE)*20.0* PIPE_POWER_ADJUST));
		Serial.print("ms\nInitial charge set to: ");
		Serial.print(600 * DISTANCE * DISTANCE * PIPE_POWER_ADJUST);
		Serial.print("ms\nDistance: ");
		Serial.print(DISTANCE);
		Serial.print("ft\nTimer started...\n//------ Sending Initial Power--------//\n");
	}

	sendPower(400 * DISTANCE * DISTANCE * PIPE_POWER_ADJUST);


	while(1){

		//Reset data
		for(int i = 0; i < BYTES_TO_RECEIVE; i++){
			data[i] = 0;
		}

		if(DEBUG_LEVEL >= 3)
			Serial.println("//--------- Sending Power----------//");

		sendPower(sendTime);

		//Delay that is synced with tag, to allow the op amp to reset for a moment
		delay(12);

		while(1){

			//Wait till a rising edge is detected
			if(DEBUG_LEVEL >= 3)
				Serial.println("//------Waiting for Data---- ---// ");

			waitForRising();
			startTime = micros();

			if(DEBUG_LEVEL >= 3){
				Serial.println("Rising edge variance: ");
				Serial.print(readings[1] - readings[0]);
				Serial.print("\n");
			}


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

			//Noise detected, not data
			if(getVariance(readings, 12) < VARIANCE_THRESHOLD){
				continue;
			}

			//Wait till data starts
			while((micros() - startTime) < (START_INTERVAL_OFFSET));

			//Read data
			readData();

			//Print received data to serial
			if(DEBUG_LEVEL >= 2)
				printReceivedData();

			if(DEBUG_LEVEL >= 3)
				Serial.println("//-------- Checking for errors--------//");

			//Check and count bit errors
			checkErrors(state);
			Serial.print("Remaining Tests: ");
			Serial.print(testsRemaining - 1);
			Serial.print("\n\n");


			//Cycle state
			state++;
			if(state == 7){
				state = 1;
			}

			testsRemaining--;
			test++;

			break;
		}
		if(testsRemaining == 0){

			time = millis() - time;
			uint32_t timeS = (int)time/1000;
			while(timeS >= 60)
				timeS -= 60;

			time /= (int)60000;
			double oneAv = calculateVarianceAverage(1);
			double zeroAv = calculateVarianceAverage(0);


			Serial.print("//----------------------------------------//"
					"\nFinished calculating errors after ");
			Serial.print(numberOfTests);
			Serial.print(" tests\nTotal bits: ");
			Serial.print(numberOfTests * (BYTES_TO_RECEIVE - 1) * 8);
			Serial.print("\nTotal Errors: ");
			Serial.print(bitErrors);
			Serial.print("\nBit Error Rate: ");
			Serial.print(100 * ((double)bitErrors/(double)(numberOfTests *(BYTES_TO_RECEIVE - 1) * 8)));
			Serial.print("%\nAverage 1 Variance: ");
			Serial.print(oneAv);
			Serial.print("\nAverage 0 Variance: ");
			Serial.print(zeroAv);
			Serial.print("\nTotal Time Taken: ");
			Serial.print(time);
			Serial.print(":");
			if(timeS < 10)
				Serial.print("0");
			Serial.print(timeS);
			Serial.print("\n");
			Serial.print("//----------------------------------------//");
			while(1);
		}
	}
}

//Continually send power, and receive data
void continuousReadTest(){

	uint32_t startTime;

	int sendTime = (int)((DISTANCE*DISTANCE*DISTANCE)*20.0 * PIPE_POWER_ADJUST);


	if(DEBUG_LEVEL >= 1){
		Serial.print("Recharge time between tests: ");
		Serial.print((int)((DISTANCE*DISTANCE*DISTANCE)*20.0* PIPE_POWER_ADJUST));
		Serial.print("ms\nInitial charge set to: ");
		Serial.print(600 * DISTANCE * DISTANCE * PIPE_POWER_ADJUST);
		Serial.print("ms\nDistance: ");
		Serial.print(DISTANCE);
		Serial.print("ft\n");
		Serial.println("//------ Sending Initial Power--------//");
	}

	sendPower(600 * DISTANCE * DISTANCE * PIPE_POWER_ADJUST);



	while(1){

		//Reset data
		for(int i = 0; i < BYTES_TO_RECEIVE; i++){
			data[i] = 0;
		}


		if(DEBUG_LEVEL >= 2)
			Serial.println("//------ Sending Power--------//");

		sendPower(sendTime);

		//Serial.println("//--------Letting OP AMP settle-------//");

		delay(12);

		while(1){
			//Wait till a rising edge is detected
			if(DEBUG_LEVEL >= 2)
				Serial.println("//------Waiting for Data---- ---// ");
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



			//Noise detected, not data
			if(getVariance(readings, 12) < VARIANCE_THRESHOLD){
				continue;
			}


			//Wait till data starts
			while((micros() - startTime) < (START_INTERVAL_OFFSET));

			//Read data
			readData();

			//Print received data to serial
			//Print received data to serial
			if(DEBUG_LEVEL >= 2)
				printReceivedData();

			if(DEBUG_LEVEL >= 3)
				Serial.println("//-------- Restarting--------//");

			break;
		}
	}
}

//Read 60 points of waveform, used to check continuous wave amplitude
void read60(){

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
}

//Calculate the average variance of a 1 and 0
double calculateVarianceAverage(unsigned int val){

	double av = 0;

	if(val == 0){
		for(int i = 0; i < NUMBER_OF_TESTS; i ++){
			av += averageOfZero[i];
		}
	} else {
		for(int i = 0; i < NUMBER_OF_TESTS; i ++){
			av += averageOfOne[i];
		}
	}

	av /= NUMBER_OF_TESTS;

	return av;
}


