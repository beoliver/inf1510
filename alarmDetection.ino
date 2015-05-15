int mic = A0;
int led = 13;
int button = 12;

int THRESHOLD = 200; // sound must be > than to be considered on

// define the alarm patter -------------------------------------
int ALARM_PATTERN_TIME[]  = {30,40,30,40,30,120};
int ALARM_PATTERN_STATE[] = {1,0,1,0,1,0};
int ALARM_PATTERN_STATE_COUNT = 6;
int REPETITIONS = 1;
int GIVE = 25; // (time + give) | (time - give)
int allowable_random_alarm = 8;
int allowable_random_silence = 8;
// -------------------------------------------------------------

// variables from sampling audio -------------------------------
int sampleValue;
int sampleCount;
int sampleAccumulator;
// -------------------------------------------------------------

// variables for calculating position in alarm pattern ---------
int position = 0;
int cycle = 1;
int state = 0;
int ON = 1; // 1
int OFF = 0; // 0
int alarm_duration_counter = 0;
int silence_duration_counter = 0;
// -------------------------------------------------------------

int alarm_is_on_for = 300000;

void setup() {
  pinMode(mic,INPUT);
  pinMode(led,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if ((alarm_is_on_for == 0) || (digitalRead(button) == LOW)) {
    digitalWrite(led,LOW);
  }
  if (sampleCount < 100) {
    sampleCount++;
    sampleAccumulator += analogRead(mic); 
  } else {
    // we have collected 100 samples
    sampleValue = sampleAccumulator / 100; // compute sampleValue
    sampleAccumulator = 0; // reset 
    sampleCount = 0; // reset
    // now test that shit...
    int desired_time  = ALARM_PATTERN_TIME[position];
    int desired_state = ALARM_PATTERN_STATE[position];
    if (sampleValue < THRESHOLD) { // silence
       if (state == ON) {
	 // if last state was the buzzer...
	 // if the alarm has just stopped.
         if ((alarm_duration_counter >= desired_time - GIVE) && (alarm_duration_counter <= desired_time + GIVE) && (state == desired_state)) {
	   position++; // test if we have completed search at end of loop
	   Serial.print("VALID BEEP :: ");
	   Serial.print("duration ~ ");
	   Serial.println(alarm_duration_counter);
	   Serial.print("POSITION ~ ");
	   Serial.print(position);
	   Serial.print(" :: CYCLE ~ ");
	   Serial.println(cycle);
	 } else if (alarm_duration_counter < allowable_random_alarm) {
	   Serial.println("! previous BEEP < 5    -- ignore");
	   // do not change position
	 } else {
	   // this block is executed if the previous alarm beep is not valid...
	   position = 0; // reset position
	   cycle    = 1; // reset cycle count
	 }
	 // no matter what the outcome we need to do some house keeping...
	 alarm_duration_counter = 0;
	 silence_duration_counter = 1;
	 state = OFF; // chnage state to silent
       } else {
	 // previous state was silence, and we are still in silence...
	 silence_duration_counter++;
       }
       // vv this code is executed when (sampleValue < Threshold). vv
       // ^^ this code is executed when (sampleValue < Threshold). ^^
    } else {
      // sample is > THRESHOLD
      if (state == OFF) {
	// if the alarm has just begun...
	if ((silence_duration_counter >= desired_time - GIVE) && (silence_duration_counter <= desired_time + GIVE) && (state == desired_state)) {
	  position++;
	  Serial.print("VALID SILENCE :: ");
	  Serial.print("duration ~ ");
	  Serial.println(silence_duration_counter);
	  Serial.print("POSITION ~ ");
	  Serial.print(position);
	  Serial.print(" :: CYCLE ~ ");
	  Serial.println(cycle);
	} else if (silence_duration_counter < allowable_random_silence) {
	  Serial.println("! previous SILENCE < 5 -- ignore");
	  // do not change position
	} else {
	  // this block is executed if the previous silence is not valid...
	  position = 0; // reset position
	  cycle    = 1; // reset cycle count
	}
	// no matter what the outcome we need to do some house keeping...
	silence_duration_counter = 0;
	alarm_duration_counter = 1;
	state = ON;
      } else {
	// previous state was ON, and the ALARM IS STILL GOING ...
	alarm_duration_counter++;
      }
    }

    // test to see if we have completed our cycles
    if (position != 0) {
      if ((position % ALARM_PATTERN_STATE_COUNT) == 0) {
	// eg position 6 of 6 == 0
	if (cycle == REPETITIONS) {
	  Serial.println("*********** ALARM ***********");
          digitalWrite(led,HIGH);
          alarm_is_on_for = 300000;
          position = 0;
          cycle = 1;
	} else {
	  cycle++;
	  position = 0;
	}
      }
    }

   
  }
}
