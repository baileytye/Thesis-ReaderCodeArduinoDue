# Thesis-ReaderCodeArduinoDue

Reader code for the Arduino Due.

Has three modes:
1. Read 60 ADC readings back-to-back (READ_60)


2. Bit Error Rate test mode (BER_TEST)


3. Continuous Read mode (When READ_60 == 0, and BER_TEST == 0)


User inputs:
There are many variables a user can change to effect results:
1. Debug level - Adjust this to increase or decrease messages being sent to the monitor.
    Levels (Higher levels contain all messages from lower levels):
      0 - 'Tests remaining alert'
          'Results display'
          
      1 - 'Mode set display'
          'User inputs display (recharge times, distance)'
          'Initial power send alert'
          
      2 - 'Total number of bit errors at the end of each test run'
          'Data where error occured warning'
          
      3 - 'Variance of each data bit during read'
          'Interval power send alert'
          'Waiting for data alert'
          'Rising edge variance alert'
          'Checking for errors alert' 

2. Distance - Adjust based on the separation between tag and reader coils. This value is in meters, and is used for calculating initial, and interval charge times.

3. Pipe Power Adjust - This value is used to adjust the initial and interval charge times when you need more than the basic calculation gives you. Mainly used when pipe is present to boost all power times. This value is a direct multiplier to both charge times.

4. Interval Adjust - This value is used to multiply just the interval charge times. Used to speed up, or slow down transmission speed. 

5. Startup Adjust - this value is used to multiply just the startup charge time. 

6. Number of Tests - This is the amount of BER tests to run. This value is not used in Read 60, or Continuous Read mode. Adjust this to adjust how many packets are tested.
