#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#define ROWS 4
#define COLS 3



int main(){
    elevio_init();
    int matrise[ROWS][COLS] = {0};
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
   
    


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



        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                

                int btnPressed = elevio_callButton(f, b);
                nanosleep(&(struct timespec){0, 2}, NULL);
                int btnPressedAfterDelay = elevio_callButton(f, b);
                if (btnPressed && btnPressed == btnPressedAfterDelay){
                    elevio_buttonLamp(f, b, btnPressed);
                    matrise[f][b] = btnPressed;
                }
                
                while(matrise[f][b] == 1){

                    floor = elevio_floorSensor();


                    for(int f = 0; f < N_FLOORS; f++){
                        for(int b = 0; b < N_BUTTONS; b++){
                            


                            btnPressed = elevio_callButton(f, b);
                            nanosleep(&(struct timespec){0, 2}, NULL);
                            btnPressedAfterDelay = elevio_callButton(f, b);
                            if (btnPressed && btnPressed == btnPressedAfterDelay){
                                elevio_buttonLamp(f, b, btnPressed);
                            }
                        }
                    }

                    if(elevio_stopButton()){
                        elevio_motorDirection(DIRN_STOP);

                        for(int f = 0; f < N_FLOORS; f++){
                            for(int b = 0; b < N_BUTTONS; b++){
                                printf("%d ", matrise[f][b]);       
                            }
                            printf("\n");   
                        }
                        break;
                    }
                    
                    if(0 <= floor && floor <= (N_FLOORS-1)){
                        
                        elevio_floorIndicator(floor);
                        
                        if(floor > f){
                            elevio_motorDirection(DIRN_DOWN);
                        }

                        if(floor < f){
                            elevio_motorDirection(DIRN_UP);
                        }
                        
                        if(floor == f){
                            elevio_motorDirection(DIRN_STOP);
                            elevio_buttonLamp(f, b, 0);
                            elevio_doorOpenLamp(1);
                            nanosleep(&(struct timespec){3, 0}, NULL);
                            elevio_doorOpenLamp(0);
                            matrise[f][b] = 0;
                            
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
            elevio_motorDirection(DIRN_STOP);

            for(int f = 0; f < N_FLOORS; f++){
                for(int b = 0; b < N_BUTTONS; b++){
                    printf("%d ", matrise[f][b]);
                }
                printf("\n");
            }


            break;
        }

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
