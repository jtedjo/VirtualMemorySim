#include "../include/virtualmemory.h"
VirtualMemory:: VirtualMemory(){
	bitMap[0] = 1; //for the ST which is "frame 1"
}
void VirtualMemory:: printBitMap(int number){
	cout << "[" <<number << "]" << ": "<< bitMap[number] << " "; 
}

void VirtualMemory::initializeST(int * mainMemory, int index, int value){
	mainMemory[index] = value;
	bitMap[value/FRAME_SIZE] = 1; //"value" is the PT table location.
	bitMap[(value/FRAME_SIZE)+1] = 1; //PT takes 2 frame. so Value + Value+1
	//segment, page, and offset is OUT parameter
	//returns true if successful (address is valid), otherwise false
}

void VirtualMemory:: initializePT(int * mainMemory, int page, int segment, int address){
	int ptAddress = mainMemory[segment];
	//page fault? It is currently not in main memory
	if(ptAddress == -1){
	
	}
	//write it/create new content? the particular segment does not exist
	else if(ptAddress == 0){
		
	}
	mainMemory[ptAddress + page] = address; 
	bitMap[address/FRAME_SIZE] = 1;
	
}
bool VirtualMemory::translateVA(int virtualAddress, int& segment, int& page, int& offset){
		//4 leading bits = no info
		//9 bits = segment number
		//10 bits = page number
		//9 bits = offset number
		int va = (virtualAddress & 268435455); //removing the first 4 bits
		segment = ((va & 0x0ff80000) >> 19);
		page = ((va & 0x00007fe00) >> 9);
		offset = (va & 0x000001ff);
		return true;
}
int VirtualMemory:: read(int * mainMemory, int segment, int page, int offset, int & outAddress){
		if(mainMemory[segment] < 0){
			return -1;
		}
		else if(mainMemory[segment] == 0){
			return 0;
		}
		else{
			int pageAddress = mainMemory[mainMemory[segment] + page];
			if(pageAddress < 0){
				return -1;
			}
			else if(pageAddress == 0){
				return 0;
			}
			else{
				outAddress = pageAddress;
				return 1;
			}
		}
		return 0;
	}

int VirtualMemory:: write(int * mainMemory, int segment, int page, int offset, int & outAddress){
		//pf
		if(mainMemory[segment] < 0){
			return -1;
		}
		//create the new blank PT
		else if(mainMemory[segment] == 0){
			if(bitMap.all()){
				return 0;
			}
			//check if any consecutive "frames" (require 2) is free
			int pageTable = 0;
			for(int i = 0 ; i < bitMap.size() - 1 ; i++){
				if(bitMap[i] == 0 && bitMap[i+1] == 0){
					pageTable = i * FRAME_SIZE;
					mainMemory[segment] = pageTable;
					bitMap[i] = 1;
					bitMap[i+1] = 1;
					break;
				}
			}
			if(pageTable == 0){
				return 0;
			}
			int pageAddress = 0;
			for(int i = 0 ; i < bitMap.size() ; i ++){
				if(bitMap[i] == 0){
					pageAddress = i * FRAME_SIZE;
					mainMemory[pageTable + page] = pageAddress;
					bitMap[i] = 1;
					outAddress = pageAddress;
					break;
				}
			}
			if(pageAddress == 0){;
				return 0;
				//should reset the bitmap for page table allocation as well?
			}
		}
		else{
			int pageAddress = mainMemory[mainMemory[segment] + page];
			if(pageAddress < 0){
				return -1;
			}
			//if page table exist but page does not exist yet...
			if(pageAddress == 0){
				int pageTable = mainMemory[segment];
				for(int i = 0 ; i < bitMap.size() ; i ++){
					if(bitMap[i] == 0){
						pageAddress = i * FRAME_SIZE;
						mainMemory[pageTable + page] = pageAddress;
						bitMap[i] = 1;
						//page address + offset after page creation
						outAddress = pageAddress;
						break;
					}
				}
				if(pageAddress == 0){
					return 0;
				//should reset the bitmap for page table allocation as well?
				}
			}
			else{
				outAddress = pageAddress;
				return 1;
			}
		}
		return 0;
}
TLB:: ~TLB(){
	while(!entries.empty()){
		tlbEntry * toDelete = entries.front();
		entries.pop_front();
		delete(toDelete);
	}
}
TLB:: tlbEntry :: tlbEntry(int virtualAddress, int pa){
	lru = 3;
	segmentPage = (virtualAddress & 0xFFFFE00); //only segment and page
	physicalAddress = pa;
}
void TLB:: addEntry(int virtualAddress, int pa){
	//does not have 4 yet...
	if(entries.size() < TLB_SIZE){
		tlbEntry * entry = new tlbEntry(virtualAddress, pa);
		entries.push_front(entry);
	}
	//then remove the least recently accessed...
	else{
		sort(entries.begin(), entries.end(), [](tlbEntry* one, tlbEntry* two){return one->lru > two->lru;});
		cout << "sort before adding..." << endl;
		printTLB();
		tlbEntry * remove = entries.back();
		delete(remove);
		entries.pop_back();
		tlbEntry * entry = new tlbEntry(virtualAddress, pa);
		entries.push_front(entry);
	}
	for(int i = 1; i < entries.size(); i++){
		(entries[i]->lru)--;//decrement the LRU of all the others
	}
}
void TLB:: printTLB(){
	for(int i = 0 ; i < entries.size(); i++){
		cout << " segment :" << ((entries[i]->segmentPage & 0x0ff80000) >> 19) << " page: " << ((entries[i]->segmentPage & 0x00007fe00) >> 9);
		cout << " located at : "<< entries[i]->physicalAddress << " lru :"<< entries[i]->lru << endl;
	}
}
//1 for found 0 for not
bool TLB:: checkTLB(int virtualAddress, int& returnAddress){
	int index = 0;
	int segmentPage = (virtualAddress & 0xFFFFE00);
	for(index = 0; index < entries.size(); index++){
		if(segmentPage == entries[index]->segmentPage){
			break;
		};//decrement the LRU of all the others
	}
	//it was found
	if(index < entries.size()){
		//decrement all
		if(entries[index]->lru != 3){
			for(int i = 0; i < entries.size(); i++){
				if((entries[i]->lru) > 0){
					(entries[i]->lru)--;//decrement the LRU of all the others
				}
			}
			(entries[index]->lru) = 3;
		}
		returnAddress = (entries[index]->physicalAddress);
		cout << "return address :"<< returnAddress << endl;
		//else just keep it same since you're accessing most recently seen anyway
		return 1;
	}
	return 0;
	
	//it wasn't found
}
