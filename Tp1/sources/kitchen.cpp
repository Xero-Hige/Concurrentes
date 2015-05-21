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
#include "times.h"

using std::string;
using std::to_string;

float generate_cooking_time() {
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    return OVEN_MIN_TIME + (rand() / (RAND_MAX / (OVEN_MAX_TIME - OVEN_MIN_TIME)));
}

void Kitchen::simulate_cook(int order) {
    int pid = fork();
    if (pid == 0) {
        //TODO: do something

#ifdef __DEBUG__
		Logger::log(__FILE__,Logger::DEBUG,"Pedido levantado, amasando: "+to_string(order));
#endif
        sleep(COOKING_TIME);

        put_in_oven(order, generate_cooking_time());

#ifdef __DEBUG__
		Logger::log(__FILE__,Logger::DEBUG,"Al horno: "+to_string(order));
#endif
        chefs.v(); //TODO: ver en que orden se deberia liberar esto
        exit(EXIT_SUCCESS);
    }
}


void Kitchen::accept_order(int order) {
    launched_process++;
    chefs.p();
    simulate_cook(order);
}

void Kitchen::accept_orders() {
    int order;
    while (requests_fifo.read_fifo(static_cast<void *>(&order), sizeof(int)) > 0) {
        max_requests.v();
        if (order == 0) {
            break;
        }

        accept_order(order);
    }

    requests_fifo.close_fifo();
    request_fifo_lock.release();


    for (size_t i = 0; i < launched_process; i++) {
        wait(0); //Waits for all chefs to finish
    }

#ifdef __DEBUG__
		Logger::log(__FILE__,Logger::DEBUG,"Amasados todos los pedidos");
#endif

    occupied_ovens_semaphore.w();
    finished_fifo.close_fifo();
    finished_fifo.remove();
}

Kitchen::Kitchen(Semaphore &chefs_semaphore, Semaphore &max_requests_semaphore, Shared_Memory<int> *ovens,
                 Semaphore &free_ovens_semaphore, Semaphore &occupied_ovens_semaphore) :
        chefs(chefs_semaphore), request_fifo_lock(REQUEST_FIFO_LOCK),
        requests_fifo(REQUEST_PIPE), max_requests(max_requests_semaphore),
        ovens(ovens), free_ovens_semaphore(free_ovens_semaphore), occupied_ovens_semaphore(occupied_ovens_semaphore),
        ovens_lock(OVEN_LOCK), finished_fifo(FINISHED_FIFO) {
    request_fifo_lock.lock();
    requests_fifo.open_fifo();
    launched_process = 0;
    finished_fifo.open_fifo();
}

void Kitchen::put_in_oven(int order, float time) {

    free_ovens_semaphore.p();
    occupied_ovens_semaphore.v();

    int oven_number = 0;

    ovens_lock.lock();
    int pizza = ovens[oven_number].read();
    while (pizza != 0) {
        oven_number++;
        pizza = ovens[oven_number].read();
    }
    ovens[oven_number].write(order);
    ovens_lock.release();

    int pid = fork();
    if (pid == 0) {
        sleep(time);
        finished_fifo.write_fifo(static_cast<void *>(&oven_number), sizeof(int));
#ifdef __DEBUG__
        Logger::log(__FILE__, Logger::DEBUG,
                    "Coccion finalizada: " + std::to_string(order) + " en horno: " + std::to_string(oven_number));
#endif
        exit(EXIT_SUCCESS);
    }
}