/* 
	Greg Wills
	Operating Systems - Assignment 3
	Designing a Virtual Memory Manager
*/

#include <stdio.h>
#include <stdlib.h>

void readFromDisk()
{
	FILE *disk = fopen("BACKING_STORE.bin","r");
    if(disk == NULL)
    {
        printf("Error opening disk file\n");
        return -1;
    }

}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Too few arguments");
        return -1;
    }

    unsigned char mask = 0xFF;
    unsigned char offset;
    unsigned char pageNum;

    FILE *address_file = fopen(argv[1],"r");
	if(address_file == NULL)
	{
		printf("Error opening address file\n");
		return 0;
	}
	char address[10];
	int logical_address;
	while( fgets(address, 10, address_file) != NULL)
	{
		logical_address = atoi(address);
		printf("Address: %d\t",logical_address);
		pageNum = (logical_address>>8)&mask;
		printf("Page #: %d\t",pageNum);
		offset = logical_address & mask;
		printf("Offset: %d\n",offset);
	}
	
	fclose(address_file);	
	
    
   return 0; 
}
