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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <string>
#include <fstream>
#include "lock_file.h"

typedef char error_type_t;

class Logger {

private:
    static std::ofstream file_stream;
    static Lock_File lock;

public:
    static const error_type_t INFO = 0;
    static const error_type_t WARNING = 1;
    static const error_type_t ERROR = 2;
    static const error_type_t DEBUG = 3;

private:
    static std::string get_error_flag(error_type_t error_level);

    static void initialize_log();

public:
    static void open_logger(std::string log_file);

    static void log(std::string caller, error_type_t error_type,
                    std::string error_message);

    static void close_logger();
};

#endif /* LOGGER_H_ */
