#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/

/*You may use any global variables/structures that you like*/
void shiftArray(unsigned short *array);

/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	//The implementation of your encoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your encoder. 
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	unsigned short recent[] = {0,0,0,0,0,0,0,0};
	unsigned short curr = 0;
	unsigned short next = 0;

	int i;
	int freq = 0;
	while(1){
		
		unsigned char count = 0;

		// If buffer is empty, read first byte and write byte
		if(recent[0] == 0){

			recent[0] = readByte();
			writeBit(1);
			writeByte(curr);

		} else{

			unsigned char psn;

			// Read next byte
			if(freq){
				curr = next;
			} else{
				curr = readByte();
			}

			// Reset frequency flag
			freq = 0;

			if(curr == 300) break;

			// Check against recent characters
			for(i = 0; i < 8; i++){
				// Check if character is frequent
				if(curr == recent[i]){
					freq = 1;
					psn = i;
					break;
				}
			}
			if(freq){ // Check where there is frequency		
				writeBit(0);
				// Check for repetition
				if(curr == (next = readByte())){

					writeBit(1);
					write3Bits(psn);

					// Loop while there is repetition or count hits 7
					while(curr == (next = readByte()) && count < 7){		
						shiftArray(recent);
						recent[0] = curr;
						count++;
					}

					write3Bits(count);

				} else{ // No repetition

					writeBit(0);
					write3Bits(psn);
					shiftArray(recent);
					recent[0] = curr;

				}
			} else{  // Infrequent

				writeBit(1);
				writeByte(curr);
				shiftArray(recent);
				recent[0] = curr;

			}
			count = 0;
		}
	}

	flushWriteBuffer();
	
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
