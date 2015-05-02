/**
 logger.cpp

 Copyright 2014 Gaston Martinez Gaston.martinez.90@gmail.com

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

#include "logger.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

using std::string;
using std::stringstream;
using std::setw;
using std::setfill;
using std::fstream;
using std::endl;

std::ofstream Logger::file_stream; //Declaration of static member
Lock_File Logger::lock = Lock_File();

string get_date() {
	time_t t = time(0);

	stringstream date;

	struct tm * now = localtime(&t);
	date << setfill('0');

	date << setw(2) << now->tm_mday << "/" << setw(2) << now->tm_mon + 1 << "/"
			<< setw(4) << now->tm_year + 1900 << "   " << setw(2)
			<< now->tm_hour << ":" << setw(2) << now->tm_min << ":" << setw(2)
			<< now->tm_sec;

	return date.str();
}

/*
 CONF_FILE="$CONFDIR/installer.conf" #Hay que poner el que corresponda
 TRIM_LOG_SIZE=50

 # Gets the process calling the logger, returns the correspondant log file
 getFilePath() {
 CALLER="$1"
 #LOGDIR=$(getEnvVarValue LOGDIR)
 # LOGEXT=$(getEnvVarValue LOGEXT)
 echo "$LOGDIR/$CALLER.$LOGEXT"
 }

 # Trims the log file to the last TRIM_LOG_SIZE lines
 trimLogFile() {
 FILE="$1"
 AUX_FILE="$1.aux"
 DATE=$(date +"%d/%m/%Y %H:%M:%S")
 echo "$DATE - Log excedido" > "$AUX_FILE"
 tail --lines="$TRIM_LOG_SIZE" "$FILE" >> "$AUX_FILE"
 rm "$FILE"
 mv "$AUX_FILE" "$FILE"
 }

 # Obtains the file name and LOGSIZE
 FILE=$(getFilePath "$1")

 touch "$FILE"
 LOGSIZE=$(getEnvVarValue LOGSIZE)
 # Checks for trimming
 FILE_LINES=$(wc -l < "$FILE")
 if [ "$FILE_LINES" -gt "$LOGSIZE" ]
 then
 trimLogFile "$FILE"
 fi
 # Logs
 if [ "$#" -lt 2 ]
 then
 echo -e "\nUso: logging comando mensaje [tipo_mensaje]\n\n\tTipo de mensaje puede ser INFO, WAR o ERR.\n\tSi se omite, por defecto es INFO.\n"
 exit -1
 elif [ "$#" -eq 2 ]
 then
 log "$1" "$2" "INFO"
 else
 log "$1" "$2" "$3"
 fi
 exit 0
 */

void Logger::initialize_log() {
	file_stream << "--Inicio de ejecucion--" << endl;
	file_stream << get_error_flag(INFO) << "- " << get_date() << " -"
			<< get_error_flag(INFO) << endl;
}

string Logger::get_error_flag(error_type_t error_level) {
	switch (error_level) {
	case ERROR:
		return "ERR";
	case WARNING:
		return "WAR";
	case INFO:
	default:
		return "INFO";
	}
}

void Logger::log(string caller, error_type_t error_type, string error_message) {

	string error_flag = get_error_flag(error_type);
	string date = get_date();

	lock.lock();
	file_stream << date << "-" << "File: " << caller << " " << error_flag
			<< ": " << error_message << endl;
	lock.release();
}

void Logger::open_logger(std::string log_file) {

	lock = Lock_File(log_file.c_str());
	file_stream.open(log_file.c_str(), std::ofstream::out | std::ofstream::app);
	lock.lock();
	initialize_log();
	lock.release();
}

void Logger::close_logger() {
	lock.lock();
	file_stream << get_error_flag(INFO) << "- " << get_date() << " -"
			<< get_error_flag(INFO) << endl;
	file_stream << "--Fin de ejecucion--" << endl;
	file_stream.close();
	lock.release();
}
