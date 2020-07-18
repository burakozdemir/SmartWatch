#include <pthread.h>


pthread_t threads[5];
int thread4;
int flag=0;
 
 
void *printThread1(void *threadid) {
   while(1){
      
      for(int i=0;i<1000;i++){
          Serial.print("Thread1:");Serial.println(i);
        //delay(10);
        }
           
    };
}

void *printThread2(void *threadid) {
   while(1){
      for(int i=0;i<1000;i++){
          Serial.print("Thread2:");Serial.println(i);
        //delay(10);
        }  
    };
}

void *printThread3(void *threadid) {
   while(1){
      for(int i=0;i<2000;i++){
        Serial.print("Thread3:");Serial.println(i);
      //delay(10);
      }
      if(flag==0){
        thread4==pthread_create(&threads[4], NULL, printThread4, (void *)4);
        flag=1;
      }
   }
}

void *printThread4(void *threadid) {
   for(int i=0;i<2000;i++){
      Serial.print("Thread4:");Serial.println(i);
    //delay(10);
   }
   
}

void setup() {
 
   Serial.begin(115200);
 
   int returnValue1=pthread_create(&threads[1], NULL, printThread1, (void *)1);
   int returnValue2=pthread_create(&threads[2], NULL, printThread2, (void *)2);
   int returnValue3=pthread_create(&threads[3], NULL, printThread3, (void *)3);
   //int returnValue4=pthread_create(&threads[4], NULL, printThread4, (void *)4);

}
 
void loop() {
  
  for(int i=0;i<1000;i++){
      Serial.print("Main:");Serial.println(i);
      delay(10);
  }
}
