#include <unistd.h>
#include "bitsy.h"
/* Add any global variables or structures you need.*/
unsigned char writeBuf = 0;
unsigned char readBuf = 0;
int writeCount = 7;
int readCount = -1;

/* readByte
 * Abstraction to read a byte.
 * Relys on readBit.
 */
unsigned short readByte(){ 
	/* This function should not call read() directly.
	 * If we are buffering data in readBit, we dont want to skip over
	 * that data by calling read again. Instead, call readBit and 
	 * construct a byte one bit at a type. This also allows us to read
	 * 8 bits that are not necessarily byte alligned. */

	//Do NOT call read here. Instead rely on readBit.
	
	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	
	int i;
	unsigned short byte = 0;
	for(i = 7; i >= 0; i--){
		byte = (readBit() << i) | byte;
	}
	if(byte > 255) return 300;
	return byte; 
}

/* readByte
 * Abstraction to read 3 bits.
 * Relys on readBit.
 */
unsigned short read3Bits(){ 
	/* This function should not call read() directly.
	 * If we are buffering data in readBit, we dont want to skip over
	 * that data by calling read again. Instead, call readBit and 
	 * construct a byte one bit at a type. This also allows us to read
	 * 8 bits that are not necessarily byte alligned. */

	//Do NOT call read here. Instead rely on readBit.
	
	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	
	int i;
	unsigned short bit3Buf = 0;
	for(i = 2; i >= 0; i--){
		bit3Buf = (readBit() << i) | bit3Buf;
	}
	if(bit3Buf > 255) return 300;
	return bit3Buf;
}

/* readBit
 * Abstraction to read a bit.
 * */
unsigned short readBit(){
	/* This function is responsible for reading the next bit on the
	 * input stream from stdin (fd = 0). To accomplish this, keep a 
	 * byte sized buffer. Each time read bit is called, use bitwise
	 * operations to extract the next bit and return it to the caller.
	 * Once the entire buffered byte has been read the next byte from 
	 * the file. Once eof is reached, return a unique value > 255
	 *
	 */	

	//You will need to call read here

	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	unsigned short bit;
	if(readCount < 0){
		readBuf = 0;
		if(read(0, &readBuf, sizeof(readBuf)) == 0)
			return 300;
		readCount = 7;
	}
	bit = (readBuf >> readCount) & 0x1;
	readCount--;

	
	return bit; 
}

/* writeByte
 * Abstraction to write a byte.
 */
void writeByte(unsigned char byt){
	/* Use writeBit to write each bit of byt one at a time. Using writeBit
	 * abstracts away byte boundaries in the output.*/

	//Do NOT call write, instead utilize writeBit()
	int i;
	for(i = 7; i >= 0; i--){
		writeBit((byt >> i) & 0x1);
	}
}

/* write3Bit
 * Abstraction to write 3 bits.
 */
void write3Bits(unsigned char byt){
	/* Use writeBit to write each bit of byt one at a time. Using writeBit
	 * abstracts away byte boundaries in the output.*/

	//Do NOT call write, instead utilize writeBit()
	int i;
	for(i = 2; i >= 0; i--){
		writeBit( (byt >> i) & 0x1);
	}
}

/* writeBit
 * Abstraction to write a single bit.
 */
void writeBit(unsigned char bit){
	/* Keep a byte sized buffer. Each time this function is called, insert the 
	 * new bit into the buffer. Once 8 bits are buffered, write the full byte
	 * to stdout (fd=1).
	 */

	//You will need to call write here eventually.
	writeBuf = (bit << writeCount) | writeBuf;
	writeCount--;
	if(writeCount < 0){
		write(1, &writeBuf, sizeof(writeBuf));
		writeBuf = 0;
		writeCount = 7;
	}

}

/* flushWriteBuffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flushWriteBuffer(){
	/* This will be utilized when finishing your encoding. It may be that some bits
	 * are still buffered and have not been written to stdout. Call this function 
	 * which should do the following: Determine if any buffered bits have yet to be 
	 * written. Pad remaining bits in the byte with 1s. Write byte to stdout
	 */
	if(writeBuf){
		int i;
		for(i = writeCount; i >= 0; i++){
			writeBuf = (0x1 << writeCount) | writeBuf;
		}
		write(1, &writeBuf, sizeof(writeBuf));
		writeBuf = 0;
		writeCount = 7;
	}


}
