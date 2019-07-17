#include <iostream>
#include <fstream>
#include <string.h>
#include <bitset>
#include <deque>
#include <algorithm>
#define PM_SIZE 524288
#define FRAME_SIZE 512
#define TLB_SIZE 4

using namespace std;


class TLB{
	class tlbEntry{
		public:
		
		int lru = 3; //least recently accessed page. 0 is least and 3 most recent
		int segmentPage = 0;
		int physicalAddress = 0;
		public: 
			tlbEntry(int virtualAddress, int pa);
	};
	private:
		deque <tlbEntry *> entries;
	public:
		~TLB();
		void addEntry(int virtualAddress, int pa);
		void printTLB();
		bool checkTLB(int virtualAddress, int& outAddress);
	
};
class VirtualMemory{
	private:
	bitset<PM_SIZE/FRAME_SIZE> bitMap;
	public:	
	TLB tlb;
	VirtualMemory();
	void printBitMap(int number);
	//initialize segment table
	void initializeST(int * mainMemory, int segment, int value);
	void initializePT(int * mainMemory, int page, int segment, int address);
	//segment, page, and offset is OUT parameter
	bool translateVA(int virtualAddress, int& segment, int& page, int& offset);
	
	//returns -1 if page fault, return 0 if error (for read), return 1 otherwise
	int read(int * mainMemory, int segment, int page, int offset, int & outAddress);
	int write(int * mainMemory, int segment, int page, int offset, int & outAddress);
	

};
