// test_SAM.ino
#include <SPI_TGM.h>
#include <SPLC.h>
#include <RCM.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>


#define initButtonPin 39
#define Freqarraysize 15
#define DBarraysize 4
// #define FreqStepValue 0.05
//#define DbStepValue 10
#define TriggerOutPin 36

#define pauseButtonPin 38


const int RepeatTime=1;
const int MinFreq=4000;
const uint16_t MaxFreq=64000;
const uint16_t MinDB=40;
const int MaxDB=70;
const uint16_t stimDur=300;
const uint16_t InterTrialInterval= 9000;
const byte randSound = 1;
const int carrier_wave_freq = 30;  /*this value should be less than 50Hz*/
const int base_intensity = 30;
const int base_DB=50;
const int modulation_range=20;

// const byte setVol = 165;

double FreqStepValue;
int clockSyncTime;
int _init_second_;

int trialCount = 0;

typedef struct {
	uint16_t frequence;
	uint16_t DB;
} sound;

sound SoundArraySeq[Freqarraysize*DBarraysize];
sound SoundArrayShuff[Freqarraysize*DBarraysize];

// uint16_t vol=170;

void init_time();
void SequenceArrayGene();
void ShuffleArray();

void init_time(){
  RCM.init();
  byte temp_cm_sec = RCM.second();
  while(temp_cm_sec == RCM.second());
  // timer0_millis = 0;

  clockSyncTime = millis();
  _init_second_= RCM.second();
  // _init_min_= RCM.minute();
  // _init_hour_= RCM.hour();
  // _init_date_= RCM.date();
  // _init_month_= RCM.month();
  // _init_year_ = RCM.year();
  // _year_ = _init_year_;
  // _month_ = _init_month_;
  // _date_ = _init_date_;
  
}


void SequenceArrayGene(){

    double Octave = log10(MaxFreq/MinFreq)/log10(2);
    FreqStepValue  = Octave/Freqarraysize;
    int DBStep=10;
    uint16_t OctaveArray[Freqarraysize];
    int DBarray[DBarraysize];
    uint16_t freq;
    int Db=MinDB;
    //int SequenceArray[DBarraysize][Freqarraysize];
    
    for(int i = 0; i<Freqarraysize; i++){
    	freq = MinFreq * pow(2,FreqStepValue*i);   
    	OctaveArray[i]=freq;
       // Serial.println(OctaveArray[i]);

    }

    //Serial.println();

    for(int i=0; i<DBarraysize; i++){
    	DBarray[i]=Db;
      if (DBarraysize != 1){
        Db=Db + (MaxDB-MinDB)/(DBarraysize-1);
      }
      
      // Serial.println(DBarray[i]);
    }

    //Serial.println();

    int m=0;

    for(int i=0;i<DBarraysize;i++){
    	for(int j=0;j<Freqarraysize;j++){
    		  SoundArraySeq[m].DB=DBarray[i];
    		  SoundArraySeq[m].frequence=OctaveArray[j];
    		  m++;
    	}
    }

}

void ShuffleArray(){

	int index;
	int n=Freqarraysize*DBarraysize;
  sound temp;

  for(int i=0;i<(Freqarraysize*DBarraysize);i++){
       SoundArrayShuff[i]=SoundArraySeq[i];

  }

	randomSeed(_init_second_);

	for(int i=0; i<n;i++){
		index=random(0,n-i)+i;
		
    if(index!=i){
			temp=SoundArrayShuff[i];
      SoundArrayShuff[i]=SoundArrayShuff[index];
      SoundArrayShuff[index]=temp;

		}
	}
}

void SAM_tone_display(int stimDur,uint16_t frequence,int Vol){
	   int wave_data_length;
	   wave_data_length=stimDur/2; //give 2ms response time for each vol
	   int Amp_value_data[wave_data_length];
	   double data_time=0;
           int modulated_vol;
	   for (int m=0;m<wave_data_length;m++){
	   		Amp_value_data[m] = base_intensity*sin(2*PI*carrier_wave_freq*data_time);
	   		data_time+=0.002;
	   }  //this loop is used to generate Amp for each data point
		
		SPI_TGM.tone_vol_rampup(rand_cloudy_tone[i].frequence,temp_vol);
			
		for(int m=0;m<wave_data_length;m++){
                   delay(1);
                   modulated_vol=base_DB+Amp_value_data[m];
                    //call the function here
                   delay(1);
                 }
             //call the rampdown function

}


void setup()
{
    Serial.begin(115200);
    //SPLC.init();
    pinMode(TriggerOutPin, OUTPUT);
    digitalWrite(TriggerOutPin, LOW);
    pinMode(initButtonPin, INPUT);
    digitalWrite(initButtonPin, HIGH);

    pinMode(pauseButtonPin, INPUT);
    digitalWrite(pauseButtonPin, LOW);

    SPI_TGM.init(MEGA2560);

    SequenceArrayGene();

    // Serial.println("Press Init Button to start ....................");
   
}

void loop()
{
    
//    Serial.println("Push the start button to start!");

    while (digitalRead(initButtonPin) == HIGH) {
      }
    // digitalWrite(pauseButtonPin, LOW);

   int RealVol;

   
   if (randSound == 1) {
    for(int j=0;j<RepeatTime;j++){
      init_time();

      ShuffleArray();

      for(int i=0;i<(Freqarraysize*DBarraysize);i++){
        while (digitalRead(pauseButtonPin) ==  LOW) {};
          // RealVol=(SoundArrayShuff[i].DB+25) * 2;//changes according to the real equation of individual rig, this equation suits for rig06
          RealVol= SPLC.get_D_SPL(SoundArrayShuff[i].frequence, SoundArrayShuff[i].DB);
         //RealVol = setVol;
         trialCount++;
         // Serial.print("\n# ");
         // Serial.print(trialCount);
         // Serial.print("\t");
         // Serial.print(i);
         // Serial.print("\t");
         Serial.print(SoundArrayShuff[i].frequence);
         Serial.print("\t");
         Serial.print(SoundArrayShuff[i].DB);
         Serial.print("\t");
    // Serial.println(SoundArrayShuff[i].DB);
    Serial.println(RealVol);

    delay(100);
    // Trigger Ephys recording
    digitalWrite(TriggerOutPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerOutPin, LOW);

    delay(1000);
    
    // SPI_TGM.quick_tone_vol_cosramp_5ms(stimDur,SoundArrayShuff[i].frequence,RealVol);
    // this place should be changed into SAM tone display
    SAM_tone_display(stimDur,SoundArrayShuff[i].frequence,RealVol);
    
    delay(InterTrialInterval);

         //each sound takes about stimDur plus InterTrialDur time long
       }

     }

   }
 
   // ShuffleArray();
   // for(int i=0;i<Freqarraysize*DBarraysize;i++){
   //     Serial.print(SoundArray[i].frequence);
   //     Serial.print("\t");
   //     Serial.println(SoundArray[i].DB);
   // }
   // Serial.println();


   while(1);

 }
