#ifndef H_TIMER
#define H_TIMER

int timer0_counter = 0;
int timer0_flag = 0;
int TIMER_CYCLE = 10;

void timer_run() {
  while(timer0_counter--){
    if(!timer0_counter){
      timer0_flag = 1;
      break;
    }
  }
}

void setTimer0(int duration) {
  timer0_counter = duration / TIMER_CYCLE ;
  timer0_flag = 0;
  timer_run();
}
#endif
