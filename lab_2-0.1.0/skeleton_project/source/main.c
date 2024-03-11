#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#define ROWS 4
#define COLS 3

int matrise[ROWS][COLS] = {0};


void knapp(int f, int b){
    int btnPressed = elevio_callButton(f, b);
    nanosleep(&(struct timespec){0, 2}, NULL);
    int btnPressedAfterDelay = elevio_callButton(f, b);
    if (btnPressed && btnPressed == btnPressedAfterDelay){
        elevio_buttonLamp(f, b, btnPressed);
        matrise[f][b] = btnPressed;
    }
}

void printMatrise(){     
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            printf("%d ", matrise[f][b]);       
        }
        printf("\n");   
    }

}

void klokke(){
    struct timespec sleep_time;
    struct timespec remaining_time;

    // Set sleep time to 3 seconds
    sleep_time.tv_sec = 3;
    sleep_time.tv_nsec = 0;

    // Run code during the waiting period
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            knapp(f,b);
        }
    }

    // Wait for 3 seconds
    while (nanosleep(&sleep_time, &remaining_time) == -1) {
        // If interrupted, handle the remaining time
        printf("Sleep interrupted. Remaining time: %ld seconds, %ld nanoseconds\n",
               remaining_time.tv_sec, remaining_time.tv_nsec);
        // Sleep for the remaining time
        sleep_time = remaining_time;
    }
}

void heisFremme(int f, int b){
    elevio_motorDirection(DIRN_STOP);
    elevio_buttonLamp(f, b, 0);
    elevio_doorOpenLamp(1);
    klokke();
    elevio_doorOpenLamp(0);
    matrise[f][b] = 0;
}

int main(){


    elevio_init();
    
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
   

    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            elevio_buttonLamp(f, b, 0);
        }
    }
    
    int floor = elevio_floorSensor();

    while(floor == -1){
        elevio_motorDirection(DIRN_DOWN);
        floor = elevio_floorSensor();
    }
    
    elevio_motorDirection(DIRN_STOP);

    while(1){

        floor = elevio_floorSensor();

        if(0 <= floor && floor <= (N_FLOORS-1)){
            elevio_floorIndicator(floor);
        }


        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                
                knapp(f,b);
                
                while(matrise[f][b] == 1){

                    floor = elevio_floorSensor();


                    for(int f = 0; f < N_FLOORS; f++){
                        for(int b = 0; b < N_BUTTONS; b++){
                           knapp(f,b);
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

                            for (int i =1; i<3; i++){
                                if (matrise[floor][i]== 1){
                                    heisFremme(floor, i);
                                }
                            }
                        }

                        if(floor < f){
                            
                            elevio_motorDirection(DIRN_UP);
                            
                            for (int i = 0; i<3; i+=2){
                                if (matrise[floor][i]== 1){
                                    heisFremme(floor,i);
                                }
                            }   
                        }
                        
                        if(floor == f){
                            heisFremme(f,b);
                            
                        }                  
                    }
                }
            }
        }

        if(elevio_obstruction()){
          //  elevio_stopLamp(1);

        } else {
            // elevio_stopLamp(0);
        }

        if(elevio_stopButton()){
           // stoppknapp
            break;
        }

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
       
    } 
    return 0;
}