/* 
	Greg Wills
	Operating Systems - Assignment 3
	Designing a Virtual Memory Manager
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

const int BUFFER_SIZE = 256;
const int PHYSICAL_MEM_SIZE = 256;
const int PAGE_TABLE_SIZE = 256;
const int TLB_SIZE = 16;

void getPage(int logical_address, char* pageTable, char* physMem, int* openFrame, char* TLBPageNum, char* TLBFrameNum, int *tlbindex, int* tlbhits, int* pagefaults)
{
    unsigned char mask = 0xFF;
    unsigned char offset;
    char pageNum;
	int value = 0;
	int newFrame = 0;
	int frame = 0;
	bool tlbhit = false;
	//printf("\nVirtual Address: %d\t",logical_address);
	printf("\nLA:    %d\t",logical_address);

	pageNum = (logical_address>>8)&mask;
	//printf("Page:  %d\t",pageNum);

	offset = logical_address & mask;
	//printf("Offset: %d\t",offset);

	// Check if logical address is in TLB
	int i;
	for(i=0; i< TLB_SIZE; i++)
	{
		//printf("\nTLBPage[%d]: %d   TLBFrame[%d]: %d",i,TLBPageNum[i],i, TLBFrameNum[i]);
		if(TLBPageNum[i]==pageNum && TLBPageNum[i] != -1)
		{
			tlbhit = true;
			(*tlbhits)++;
			frame = TLBFrameNum[i];
			printf("tlbhit\t\t");
		}
	}
	//printf("\n");
	// Check if page is in pageTable
	if (tlbhit == false)
	{
		if(pageTable[pageNum] != -1){
			printf("page hit\t");
		}
		else{
			(*pagefaults)++;
			printf("page fault\t");
			newFrame = readFromDisk(pageNum, physMem, openFrame);
			pageTable[pageNum] = newFrame;
		}
		TLBPageNum[(*tlbindex)] = pageNum;
		TLBFrameNum[(*tlbindex)] = pageTable[pageNum];
		(*tlbindex) ++;
		(*tlbindex) = (*tlbindex) % TLB_SIZE;
		frame = pageTable[pageNum];
	}
	int ind = ((unsigned char)frame*PHYSICAL_MEM_SIZE)+offset;
	value = *(physMem+ind);
	printf("Physical Address: %d\t Value=%d", ind, value);
}

int readFromDisk(int pageNum, char* physMem, int* openFrame)
{
	FILE *disk = fopen("BACKING_STORE.bin","r");
    if(disk == NULL)
    {
        printf("Error opening disk file\n");
        exit(1);
    }

	char address[BUFFER_SIZE];
	memset(address, 0, sizeof(address));

	// get the starting point of the binary file with offset
	if (fseek(disk, pageNum * PHYSICAL_MEM_SIZE, SEEK_SET) != 0)
		printf("error with fseek\n");
	// read binary information starting from offset	
	if (fread(address, sizeof(char), PHYSICAL_MEM_SIZE, disk)==0)
		printf("error with fread\n");
	
	int i;
	for (i=0; i < PHYSICAL_MEM_SIZE; i++)
	{
		*((physMem + i + (*openFrame)*PHYSICAL_MEM_SIZE)) = address[i];
	}
	(*openFrame)++;

	fclose(disk); // close BAKCING_STORE.bin

	return (*openFrame)-1;
}

int main(int argc, char* argv[])
{
	// Error checking
    if(argc < 2)
    {
        printf("Not enough arguments...exiting\n");
        return -1;
    }

	// open file full of virtual addresses
    FILE *address_file = fopen(argv[1],"r");
	if(address_file == NULL)
	{
		printf("Error opening address file\n");
		return 0;
	}

	unsigned char pageTable[PAGE_TABLE_SIZE];
	memset(pageTable, -1, sizeof(pageTable));
	
	char physMem[PHYSICAL_MEM_SIZE][PHYSICAL_MEM_SIZE];

	unsigned char TLBPageNum[TLB_SIZE];
	unsigned char TLBFrameNum[TLB_SIZE];
	int tlbindex = 0;
	memset(TLBPageNum, -1, sizeof(TLBPageNum));
	memset(TLBFrameNum, -1, sizeof(TLBFrameNum));

	char buffer[10];
	int openFrame = 0;
	int logical_address;
	
	int totAddr = 0;
	int tlbhits = 0;
	int pagefaults = 0;
	double tlbHitRate = 0;
	double pageFaultRate = 0;

	while( fgets(buffer, 10, address_file) != NULL)
	{
		logical_address = atoi(buffer);
		getPage(logical_address, pageTable, (char*)physMem, &openFrame, TLBPageNum, TLBFrameNum, &tlbindex, &tlbhits, &pagefaults);
		totAddr++;
	}
	
	fclose(address_file);	
	printf("\ntlbhits: %d, pagefaults: %d\n", tlbhits, pagefaults);
	pageFaultRate = (double)pagefaults / (double)totAddr;
	tlbHitRate = (double)tlbhits / (double)totAddr;
	printf("pfRate: %.3lf, tlbhitRate: %.3lf\n", pageFaultRate, tlbHitRate);
	return 0; 
}
