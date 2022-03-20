/*************************/
/*   Alex Hromada		 */
/*   CS3411		         */
/*   Program 1	         */
/*************************/

#include "hmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
/*You may include any other relevant headers here.*/


/*Add additional data structures and globals here as needed.*/
void *free_list = NULL;

/* traverse
 * Start at the free list head, visit and print the length of each
 * area in the free pool. Each entry should be printed on a new line.
 */
void traverse(){
   /* Printing format:
	 * "Index: %d, Address: %08x, Length: %d\n"
	 *    -Index is the position in the free list for the current entry. 
	 *     0 for the head and so on
	 *    -Address is the pointer to the beginning of the area.
	 *    -Length is the length in bytes of the free area.
	 */
	if(free_list == NULL) // Return if free list is empty
	{
		printf("Free list empty\n");
		return;
	}

	void *curr = free_list;
	int i = 0;
	do // Iterate over free list and print each address and length
	{
		printf("Index: %d, Address: %p, Length: %d\n", i, curr, ((int *)curr)[0]);
		curr += ((int *)curr)[1];
		i++;
	} while((((int *)curr)[1]) != 0);
}

/* hmalloc
 * Allocation implementation.
 *    -will not allocate an initial pool from the system and will not 
 *     maintain a bin structure.
 *    -permitted to extend the program break by as many as user 
 *     requested bytes (plus length information).
 *    -keeps a single free list which will be a linked list of 
 *     previously allocated and freed memory areas.
 *    -traverses the free list to see if there is a previously freed
 *     memory area that is at least as big as bytes_to_allocate. If
 *     there is one, it is removed from the free list and returned 
 *     to the user.
 */
void *hmalloc(int bytes_to_allocate){

	if (free_list == NULL) // Allocate new block
	{
		void *ptr;
		ptr = sbrk(bytes_to_allocate + 8);

		((int *)ptr)[0] = bytes_to_allocate; // Update length field
		((int *)ptr)[1] = NULL; // Update link field

   		return ptr + 8; //placeholder to be replaced by proper return value
	}
	else // re-using blocks in free list
	{
		void *curr = free_list; // Current node in free list
		void *pred = NULL; // Predecessor node in free list
		do
		{
			if (((int *)curr)[0] >= bytes_to_allocate)
			{
				if(pred == NULL) // If pred is NULL, you are at free list head
				{
					if(((int *)free_list)[1] == NULL) // if link field is empty
					{						
						printf("Free list link field null\n");
						free_list = NULL;
					}
					else
					{
						free_list = free_list + ((int *)curr)[1]; // Update free list head to next node
					}
				}
				else  
				{
					((int*)pred)[1] = ((int*)pred)[1] + ((int *)curr)[1]; // Update linkfield to remove reused block
				}
				((int *)curr)[0] = bytes_to_allocate;  // Update length field
				((int *)curr)[1] = NULL;  // Update link field
				return curr + 8;
			}
			pred = curr;
			curr += ((int *)curr)[1];
		} while ((((int *)curr)[1]) != 0);

		void *ptr;
		ptr = sbrk(bytes_to_allocate + 8);
		((int *)ptr)[0] = bytes_to_allocate; // Update length field
		((int *)ptr)[1] = NULL; // Update link field
		return ptr + 8; 
	}

}

/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated 
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate){
	
	void *ptr = hmalloc(bytes_to_allocate);
	ptr -= 8; // Move back to metadata
	int n = ((int *)ptr)[0];  // Get length
	ptr += 8; // Move back to allocation area
	int i;
	// Set all values in allocation block to 0
	for(i = 0; i < n; i++)
	{
		((int *)ptr)[i] = 0;
	}
   	return ptr; 
}

/* hfree
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr){
	// Set pointer to front of metadata
	ptr -= 8;

	if (free_list == NULL) // If free list empty set the head to current ptr
	{
		free_list = ptr;
		((int *)free_list)[1] = NULL; // Update link field

	}
	else // Update the freelist with new head and update link field of head
	{
		void *temp = free_list;
		free_list = ptr;

		((int *)free_list)[1] = temp - ptr; // Update link field

	}
}

/* For the bonus credit implement hrealloc. You will need to add a prototype
 * to hmalloc.h for your function.*/

/*You may add additional functions as needed.*/
