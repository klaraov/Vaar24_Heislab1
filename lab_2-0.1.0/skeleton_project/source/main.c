#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#define ROWS 4
#define COLS 3

int matrise[ROWS][COLS] = {0};

void rightFloor(int floor, int i){
    if (matrise[floor][i]== 1){
        elevio_motorDirection(DIRN_STOP);
        elevio_buttonLamp(floor, i, 0);
        elevio_doorOpenLamp(1);
        for(int i = 0; i < 200; i++){buttonIsPressed();}
        elevio_doorOpenLamp(0);
        matrise[floor][i] = 0;
        
    }
}

void stopBtnIsPressed(){
    elevio_motorDirection(DIRN_STOP);
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            printf("%d ", matrise[f][b]);       
        }
        printf("\n");   
    }
}


void buttonIsPressed(){
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            nanosleep(&(struct timespec){0, 2}, NULL);
            int btnPressedAfterDelay = elevio_callButton(f, b);
            if (btnPressed && btnPressed == btnPressedAfterDelay){
                elevio_buttonLamp(f, b, btnPressed);
                matrise[f][b] = btnPressed;
            }
        }
    }
}

int main(){
    elevio_init();
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    int floor = elevio_floorSensor();

    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            elevio_buttonLamp(f, b, 0);
        }
    }


    while(1){

        int floor = elevio_floorSensor();

        if(0 <= floor && floor <= (N_FLOORS-1)){
            elevio_floorIndicator(floor);
        }

        buttonIsPressed();
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                
                while(matrise[f][b] == 1){

                    floor = elevio_floorSensor();


                    buttonIsPressed();

                    if(elevio_stopButton()){
                        stopBtnIsPressed();
                        break;
                    }
                    
                    if(0 <= floor && floor <= (N_FLOORS-1)){
                        
                        elevio_floorIndicator(floor);
                        
                        if(floor > f){
                            
                            elevio_motorDirection(DIRN_DOWN);

                            for (int i =1; i<3; i++){
                                rightFloor(floor, i);
                            }
                        }

                        if(floor < f){
                            
                            elevio_motorDirection(DIRN_UP);
                            
                            for (int i = 0; i<3; i+=2){
                                rightFloor(floor, i);
                            }   
                        }
                        
                        if(floor == f){
                            rightFloor(f, b);
                        }                  
                    }
                }
            }
        }

        if(elevio_obstruction()){
            elevio_stopLamp(1);

        } else {
            elevio_stopLamp(0);
        }

        if(elevio_stopButton()){
            stopBtnIsPressed();
            break;
        }

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
