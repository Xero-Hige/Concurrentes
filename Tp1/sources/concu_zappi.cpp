/**
 concu_zappi.cpp

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

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include "logger.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::stringstream;
using std::map;

int main(int argc, char** argv) {

	Logger::open_logger("run_log.log"); //TODO: Agregar opccion para sobreescribir

	Logger::log(__FILE__, Logger::INFO, "Inicio configuracion");
	map<string, int> config;

	config["Recepcionistas"]=0;
	config["Cocineras"]=0;
	config["Cadetas"]=0;
	config["Hornos"]=0;

	ifstream file;
	file.open("config.cfg", std::fstream::in);

#ifdef __DEBUG__
	Logger::log(__FILE__, Logger::INFO, "Configuracion inicial");
#endif
	while (file.good()) {
		int amount = 0;
		string concept;

		file >> amount >> concept;

		if (config.count(concept)==0){
			continue;
		}

		config[concept] = amount;

#ifdef __DEBUG__
		stringstream output;
		output << concept << " cantidad: " << amount;
		Logger::log(__FILE__, Logger::INFO, output.str());
#endif
	}

	for (std::map<string,int>::iterator it=config.begin(); it!=config.end(); ++it){
		if (it->second <= 0){
			stringstream output;
			output << it->first << " no tiene un valor asignado o es menor o igual a 0 ";
			Logger::log(__FILE__, Logger::ERROR, output.str());
			Logger::log(__FILE__,Logger::ERROR,"Configuracion fallida");
			Logger::log(__FILE__, Logger::INFO, "Cierre forzado");
			return 1;
		}
	}

	Logger::log(__FILE__, Logger::INFO, "Configuracion exitosa");

	Logger::log(__FILE__, Logger::INFO, "Inicia recepcion de pedidos");
	Logger::log(__FILE__, Logger::INFO, "Cerrada recepcion de pedidos");
}