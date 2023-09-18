#include "AEFileWriter.hpp"
#include <iostream>

int main() {
	AEFileWriter myfw;
	myfw.open("hello.txt");
	myfw.writeString("My name is: ArtyK;\n");
	myfw.write(18);
	myfw.writeString(" is my age;\n");
	myfw.writeByte(0xAE);
	myfw.writeString(" is my favourite byte;\n");
	myfw.write(myfw.isOpen());
	myfw.writeString(" -> is file open?;\n");
	myfw.writeString("Total write requests up to this point: ");
	myfw.write(myfw.getTotalWrites());
	myfw.flushFile();
	myfw.closeFile();
	return 0;
}