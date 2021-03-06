/**
 Copyright 2015
 Debora Martin debbie1new.world@gmail.com
 Gaston Martinez gaston.martinez.90@gmail.com
 Bruno Merlo Schurmann bruno290@gmail.com

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

#ifndef KITCHEN_H
#define KITCHEN_H

#include <stddef.h>
#include <string>

#include "pipe.h"
#include "lock_file.h"
#include "semaphore.h"
#include "reader_fifo.h"
#include "writer_fifo.h"
#include "shared_memory.h"
#include "locknames.h"
#include "pipenames.h"

class Kitchen {

private:
    Semaphore chefs;
    size_t launched_process;

    Lock_File request_fifo_lock;
    ReaderFifo requests_fifo;
    Semaphore max_requests;

    Shared_Memory<int> *ovens;
    Semaphore free_ovens_semaphore;
    Semaphore occupied_ovens_semaphore;
    Lock_File ovens_lock;

    WriterFifo finished_fifo;

private:
    void accept_order(int order);

    void simulate_cook(int order);

    void put_in_oven(int order, float time);

public:
    Kitchen(Semaphore &chefs_semaphore, Semaphore &max_requests_semaphore, Shared_Memory<int> *ovens,
            Semaphore &free_ovens_semaphore, Semaphore &occupied_ovens_semaphore);

    void accept_orders();
};

#endif /* KITCHEN_H */
