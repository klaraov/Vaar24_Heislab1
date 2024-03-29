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

void obstruction(){
    while (elevio_obstruction()){
        elevio_doorOpenLamp(1);
        
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                knapp(f,b);
            }
        }
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
    time_t start = time(NULL);
    time_t slutt = time(NULL);
    while (difftime(slutt,start)< 3){ 
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                knapp(f,b);
            }
        }
        slutt = time(NULL);
    }
}

void heisFremme(int f, int b){
    elevio_motorDirection(DIRN_STOP);
    elevio_buttonLamp(f, b, 0);
    elevio_doorOpenLamp(1);
    obstruction();
    klokke();
    obstruction();
    elevio_doorOpenLamp(0);
    matrise[f][b] = 0;
}

void resetMatrise(){
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            elevio_buttonLamp(f, b, 0);
            matrise[f][b] = 0;
        }
    }
}

void stoppKnapp(int floor){
    int stopp1 = elevio_stopButton();
    nanosleep(&(struct timespec){0, 2}, NULL);
    int stopp2 = elevio_stopButton();

    if (stopp1 && stopp2){
        elevio_motorDirection(DIRN_STOP);
        resetMatrise();
        
        while (1){ 
            elevio_motorDirection(DIRN_STOP);
            elevio_stopLamp(1);  
            if (floor!= -1){
                elevio_doorOpenLamp(1);
            }
            stopp2 = elevio_stopButton();
            nanosleep(&(struct timespec){0, 2}, NULL);
            stopp1 = elevio_stopButton();
            
            if (!stopp1 && stopp1==stopp2){
                break;
            }

    
        }
        
        elevio_stopLamp(0);
        if (floor!= -1){     
            klokke();
            elevio_doorOpenLamp(0);
        }
    }
    
}

void mellomEtasjer(int floor){
    if (floor == -1){
        nanosleep(&(struct timespec){3, 0}, NULL);
    }
    while(floor == -1){
        
        elevio_motorDirection(DIRN_DOWN);
        floor = elevio_floorSensor();
    }
    elevio_motorDirection(DIRN_STOP);

}

int main(){


    elevio_init();
    
    printf("=== Selma and Klaras amazing elevator ===\n");
    printf("There is no exit button, sorry");
   
    resetMatrise();
    
    int floor = elevio_floorSensor();

    mellomEtasjer(floor);
    


    while(1){
      
      
        
        floor = elevio_floorSensor();

        if(0 <= floor && floor <= (N_FLOORS-1)){
            elevio_floorIndicator(floor);
        }
        
        stoppKnapp(floor);
        mellomEtasjer(floor);

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

                    stoppKnapp(floor);
                    
                    
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
      
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }
    

    
    return 0;
} 
   
