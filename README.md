# Thesis-ReaderCodeArduinoDue

Reader code for the Arduino Due. This code requires an ADC of 200kps and will not work on other Arduinos. The Arduino Due must be paired with the reader circuit, function generator, current amplifier, DC power supply, oscilloscope, and reader power/data coils. 

## Modes:
#### 1. Read 60 ADC readings back-to-back (READ_60)
This mode is used to read 60 consecutive ADC values. This was mainly used to graph the incoming data signal to measure amplitude. Currently this does not wait for a rising edge, it just reads the first 60 ADC readings as soon as it starts. That means you must set the tag to continuos transmit mode.

#### 2. Bit Error Rate test mode (BER_TEST)
This mode allows you to run bit error tests with the tag. This reuires the tag to be in BER mode and must be drained before tests begin. (completely drain battery caps, and Vcc on tag so that it is not sitting in deep sleep. This is to sync the states with the reader.)
To correctly use BER mode, make sure to set the user input parameters to the correct values.

#### 3. Continuous Read mode (When READ_60 == 0, and BER_TEST == 0)
This mode allows the reader to continually charge and read the tag. This is useful when you are using the tag in sensor mode and want to continually probe the results. 

## User inputs:
There are many variables a user can change to effect results:
#### 1. Debug level:
Adjust this to increase or decrease messages being sent to the monitor.

##### Levels (Higher levels contain all messages from lower levels):
0. - 'Tests remaining alert'
   - 'Results display'
          
1. - 'Mode set display'
   - 'User inputs display (recharge times, distance)'
   - 'Initial power send alert'
          
2. - 'Total number of bit errors at the end of each test run'
   - 'Data where error occured warning'
          
3. - 'Variance of each data bit during read'
   - 'Interval power send alert'
   - 'Waiting for data alert'
   - 'Rising edge variance alert'
   - 'Checking for errors alert' 

#### 2. Distance:
Adjust based on the separation between tag and reader coils. This value is in meters, and is used for calculating initial, and interval charge times.

#### 3. Pipe Power Adjust: 
This value is used to adjust the initial and interval charge times when you need more than the basic calculation gives you. Mainly used when pipe is present to boost all power times. This value is a direct multiplier to both charge times.

#### 4. Interval Adjust:
This value is used to multiply just the interval charge times. Used to speed up, or slow down transmission speed. 

#### 5. Startup Adjust:
This value is used to multiply just the startup charge time. 

#### 6. Number of Tests:
This is the amount of BER tests to run. This value is not used in Read 60, or Continuous Read mode. Adjust this to adjust how many packets are tested.


## How Data is Read:
To read data the following steps are taken:
1. Wait for a rising edge - Continuously read back to back ADC readings (spaced by 4) until a rising edge is detected. This is confirmed by being greater than the system variable RISING_OFFSET.
2. Wait till next peak of signal, this is determined to be START_INTERVAL_OFFSET after the rising edge is detected. This will be the location of the first data bit to read as the first peak is the start bit.  
3. Measure and record the next 15 ADC values. These are spaced by 5us due to the 200kbps sampling rate in the Arduino Due. One period of the data signal is 62.5us, the 15 ADC values are equal to 75 us, slightly more than a period. 
4. Calculate the average of those values, and then compare each value to that average. The sum of the differences between the value and the average are stored as the 'Variance' of the bit. If that value is greater than VARIANCE_THRESHOLD, then this value is saved as a '1', if not it is saved as a '0'.
5. Wait till the next bit peak which is spaced by BIT_INTERVAL. This is due to the ringing caused on the reader, the bits must be this far away for the signal to stablize low enough for a zero to be read as a zero. 
6. Repeat steps 3-5 8 times to create a full byte.
7. Repeat steps 1-6 BYTES_TO_RECEIVE times.


## System Variables
These should only be changed if you know what you are doing, and need to adjust timings or variance thresholds.

#### 1. Rising Offset

#### 2. Variance Threshold

#### 3. Bit Interval

#### 4. Start interval Offset

#### 5. Bytes to Receive


