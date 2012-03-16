#include "controller.h"
#include "mthread.h"
#include "config_handler.h"
#include "mthread_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

// Running Thread
struct mthread *current;

// Threads list
struct mthread **threads;

// Configuration info
struct config_info * cinfo;

// For test purposes
int finisheds[2];
int finished = 0;
int started = 0;

// Alarm interval (quantum)
struct itimerval timer_interval;

/*
 * Function to calculate the value
 */
void calc_function(){
        int i = 0;
        while (i < 200){
                printf("calc_funct(%i)\n", i);
                usleep(10000);
                i++;
        }
}

/*
 * Handles the logic for thread ending
 */
void thread_ended(){
        // TODO: Implement thread ending logic        
        printf("Thread finished\n");

        mthread_free(current);

       finisheds[current->id] = 1;

       finished = finisheds[0] + finisheds[1] > 1 ? 1 : 0; 

       if (finished){
                timer_interval.it_value.tv_usec = 0;
       }
}

int k = 0;
void controller_alarm_handler(int sig){
        // TODO: Implement alarm logic

        if (!started){
                started = 1;
                current = threads[0];
                setcontext(threads[0]->env);
        }
        
        int c = current->id;
        int next = 2 - (c + 1);

        printf("Current = [%d]; next = [%d]\n", c, next);

        if (!finished && !finisheds[next]){
                current = threads[next];
                swapcontext(threads[c]->env, threads[next]->env); 
        }
}

static void set_timer(int quantum){
        // configure the alarm
        struct sigaction act;
        struct timeval timer_slice;
        struct timeval timer_init;
        timer_slice.tv_sec = 0;
        timer_slice.tv_usec = quantum * 1000;
        timer_init.tv_sec = 0;
        timer_init.tv_usec =  quantum * 1000;
        timer_interval.it_value = timer_init;
        timer_interval.it_interval = timer_slice;
        act.sa_handler = controller_alarm_handler;
        sigaction (SIGALRM, &act, 0);
}

void controller_init(){
        printf("Controller initialized;\n");

        // Load the configuration
        cinfo = config_handler_load();

        printf("Configuration readed: is_preemptive = %d; quantum = %d\n", cinfo->is_preemptive, cinfo->quantum);
        int i;
        for (i = 0; i < cinfo->thread_list->count; i++){
                printf("Thread: id =%d; tickets = %d; fticket = %d; work = %d\n",
                                cinfo->thread_list->threads[i]->id,
                                cinfo->thread_list->threads[i]->ticketc,
                                cinfo->thread_list->threads[i]->fticket,
                                cinfo->thread_list->threads[i]->workc);
        }

        // Initialize the timer
        set_timer(cinfo->quantum);

        // Initialize thread environment
        //mthread_init_environment(thread_ended);

        
        // Test thread creation
        //threads = malloc(2 * sizeof(struct mthread *));
        
        //threads[0] = mthread_create(calc_function);
        //threads[1] = mthread_create(calc_function);
        //current = threads[0];
        
        //finished = 0;
        //memset(finisheds, 0, 2 + sizeof(int));

}

void controller_start(){
        //mthread_run(current);


        // Start timer
        //setitimer(ITIMER_REAL, &timer_interval, NULL);

        //while(!finished);
}

void controller_end(){
        // Destroy the thread handling environment
        //mthread_destroy_environment();
}
