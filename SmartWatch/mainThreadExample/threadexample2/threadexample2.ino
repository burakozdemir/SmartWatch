#include <pthread.h>
 
void *printThreadId(void *threadid) {
   Serial.println((int)threadid);
}
 
void setup() {
 
   Serial.begin(115200);
 
   pthread_t threads[4];
   int returnValue;
 
   for( int i = 0; i< 30; i++ ) {
 
      returnValue = pthread_create(&threads[i], NULL, printThreadId, (void *)i);
 
      if (returnValue) {
         Serial.println("An error has occurred");
      }
   }
 
}
 
void loop() {}
