#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/
void shiftArray(unsigned short *array);

/*You may use any global variables/structures that you like*/

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){
	//The implementation of your decoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your decoder.
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.

	unsigned short recent[] = {0,0,0,0,0,0,0,0};
	unsigned short bitBuf;
	unsigned short byteBuf;
	unsigned short bit3Buf;
	unsigned short repeatBits;

	bitBuf = readBit(); // Read first bit
	byteBuf = readByte(); // Read first byte
	writeByte(byteBuf);
	recent[0] = byteBuf;

	int i;
	// Read to EOF
	while((bitBuf != 300) && (byteBuf != 300)){
		bitBuf = readBit(); // Read next bit

		if((bitBuf == 300) || (byteBuf == 300)) break; // break from loop if hits EOF

		if(bitBuf == 0x1){ // Decode infrequent
			byteBuf = readByte();
			writeByte(byteBuf);
			shiftArray(recent);
			recent[0] = byteBuf;
		} else { // Decode frequent

			bitBuf = readBit();

			if(bitBuf == 0x0){ // Non-repeating bytes
				bit3Buf = read3Bits();
				byteBuf = recent[bit3Buf];
				writeByte(byteBuf);
				shiftArray(recent);
				recent[0] = byteBuf;
			} else{ // Repeating bytes
				bit3Buf = read3Bits();
				repeatBits = read3Bits();
				byteBuf = recent[bit3Buf];
				
				for(i = 0; i < repeatBits; i++){
					writeByte(byteBuf);
					shiftArray(recent);
					recent[0] = byteBuf;
				}
			}
		} 
	}
	
	return 0; //exit status. success=0, error=-1
}

/* 
 * Helper function used to shift the recent buffer over by 1
 * Used when new byte is added to recent
 */
void shiftArray(unsigned short *array){
	int i;
	for(i = 7; i > 0; i--){
		array[i] = array[i - 1];
	}
}
