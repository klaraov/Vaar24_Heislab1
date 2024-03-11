#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#define ROWS 4
#define COLS 3

int matrise[ROWS][COLS] = {0};

void ifButtonPressed(){
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            for (int i = 0; i < 1000; ++i) {}
            int btnPressedAfterDelay = elevio_callButton(f, b);
            elevio_buttonLamp(f, b, 1);
            matrise[f][b] = 1;
        }
    }
}

void stoppProgram(){
    elevio_motorDirection(DIRN_STOP);

    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            printf("%d ", matrise[f][b]);
            
        }
        printf("\n");
        
    }
}

void moveElevator(int f, int b){
    ifButtonPressed();
    int floor = elevio_floorSensor();
    elevio_floorIndicator(floor);
    assert(0 <= floor);
    assert(floor <= (N_FLOORS-1));

    if(floor > f){
        ifButtonPressed();
        elevio_motorDirection(DIRN_DOWN);
    }

    if(floor < f){
        ifButtonPressed();
        elevio_motorDirection(DIRN_UP);
    }
    
    if(floor == f){
        ifButtonPressed();
        elevio_motorDirection(DIRN_STOP);
        elevio_buttonLamp(f, b, 0);
        elevio_doorOpenLamp(1);
        nanosleep(&(struct timespec){3, 0}, NULL);
        elevio_doorOpenLamp(0);
        matrise[f][b] = 0;
        
    }
}


int main(){
    elevio_init();

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
   
    while(1){
        ifButtonPressed();
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                elevio_buttonLamp(f, b, 0);
            }
        }

        int floor = elevio_floorSensor();

        if(0 <= floor && floor <= (N_FLOORS-1)){
            elevio_floorIndicator(floor);
        }



        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){  
                while(matrise[f][b] == 1){
                    ifButtonPressed();
                    if(elevio_stopButton()){
                        stoppProgram();
                        break;
                    }
                    moveElevator(f, b);                  
                }
                
            }
        }

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        if(elevio_stopButton()){
            stoppProgram();
            break;
        }
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
