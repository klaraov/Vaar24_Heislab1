#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"



int main(){ 
    elevio_init();
    
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    elevio_motorDirection(DIRN_UP);

    while(1){
        int floor = elevio_floorSensor();

        if(floor == 0){
            elevio_motorDirection(DIRN_UP);
        }

        if(floor == N_FLOORS-1){
            elevio_motorDirection(DIRN_DOWN);
        }
        
        

        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                int btnPressed = elevio_callButton(f, b);
                for (int i = 0; i < 1000; ++i) {}
                int btnPressedAfterDelay = elevio_callButton(f, b);
                // Return true if the button is pressed and its state remained stable after the delay
                if(btnPressed == 1 && btnPressed == btnPressedAfterDelay){
                    elevio_buttonLamp(f, b, btnPressed);
                    if(floor == f){
                        elevio_motorDirection(DIRN_STOP);
                        nanosleep(&(struct timespec){3, 0}, NULL);
                        elevio_motorDirection(DIRN_UP);
                    }
    
                }
                //elevio_buttonLamp(f, b, btnPressed);
            }
        }

        

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        
        if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        }
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
