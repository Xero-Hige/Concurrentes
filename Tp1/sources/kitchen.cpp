/**
 kitchen.cpp

 Copyright 2015 Gaston Martinez Gaston.martinez.90@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses
 */

#include <cstdlib>
#include <sys/wait.h>

#include "kitchen.h"
#include "logger.h"
#include "pipenames.h"
#include "locknames.h"

#define MIN_TIME 0.5
#define MAX_TIME 2.0

using std::string;

float generate_cooking_time() {
    static bool seeded = false;
    if (! seeded) {
        srand(time(NULL));
        seeded = true;
    }
    return MIN_TIME + (rand() / (RAND_MAX / (MAX_TIME - MIN_TIME)));
}

void Kitchen::simulate_cook(std::string pizza) {
    int pid = fork();
    if (pid == 0) {
        //TODO: do something

#ifdef __DEBUG__
		Logger::log(__FILE__,Logger::DEBUG,"Amasando: "+pizza);
#endif
        sleep(2);
        free_ovens_semaphore.p();
        ovens.cook(pizza, generate_cooking_time());
        free_ovens_semaphore.v();
#ifdef __DEBUG__
		Logger::log(__FILE__,Logger::DEBUG,"Al horno: "+pizza);
#endif
        chefs.v(); //TODO: ver en que orden se deberia liberar esto
        exit(EXIT_SUCCESS);
    }
}


void Kitchen::accept_order(std::string pizza) {
    launched_process++;
    chefs.p();
    simulate_cook(pizza);
}

void Kitchen::accept_orders() {
    static const int BUFFSIZE = 200;
    char buff[BUFFSIZE];
    char len_buff[sizeof(int)];
    while (requests_fifo.read_fifo(len_buff, sizeof(int)) > 0) {
        max_requests.v();
        int len = *(int *) len_buff;
        if (len == 0) {
            break;
        }

        if (requests_fifo.read_fifo(buff, len) == 0) {
            //TODO: Error
        }

        string pizza = buff;
        pizza.resize(len);
        accept_order(pizza);
    }

    requests_fifo.close_fifo();
    request_fifo_lock.release();


    for (size_t i = 0; i < launched_process; i++) {
        wait(0); //Waits for all chefs to finish
    }

}

Kitchen::Kitchen(Semaphore &chefs_semaphore, Semaphore &max_requests_semaphore, OvenSet &ovenSet, Semaphore &free_ovens) :
        chefs(chefs_semaphore), request_fifo_lock(REQUEST_FIFO_LOCK),
        requests_fifo(REQUEST_PIPE) ,max_requests(max_requests_semaphore),
        ovens(ovenSet), free_ovens_semaphore(free_ovens) {
    request_fifo_lock.lock();
    requests_fifo.open_fifo();
    launched_process = 0;
}
