#include "reader_fifo.h"

ReaderFifo::ReaderFifo(const std::string filename) : Fifo(filename) {
}

ReaderFifo::~ReaderFifo() {
}

void ReaderFifo::open_fifo() {
	fd = open(name.c_str(), O_RDONLY);

	if (fd == -1) {
		std::cerr << "Error al abrir el archivo para lectura" << std::endl;
	}
}

ssize_t ReaderFifo::read_fifo(void* buffer,const ssize_t buffsize) const {
	return read(fd,buffer, buffsize);
}
