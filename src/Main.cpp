#include "../include/virtualmemory.h"


//each "frame" is 512 words, and each "word" is 32 bits (4 bytes).

using namespace std;


int main(int argc, char *argv[]){
	
	//initialize to 0, which is default for most of the lookups
	//main memory is an array of 524,288 integers (1024 frames x 512 words)
	int * mainMemory;
	mainMemory = new int[PM_SIZE];
	for(int i = 0; i < PM_SIZE; i++){
		mainMemory[i] = 0;
	}
	VirtualMemory memoryHandler; //memory handler
	TLB tlb;
	//FILE HANDLING PORTION
	if(argc != 3){
		cout << "Error: usage = " << argv[0] << " init.txt input1.txt\n";
		exit(1);
	}
	char * init = argv[1];
	char * inputVA = argv[2];
	//char outputTLB[20]; //holds the TLB file name
	string lineInput;
	//strcpy(outputTLB, outputFile);
	int i = 0;
	//find where the nulltermination is at
	/*
	for( i = 0 ; i < 20; i++){
		if(outputTLB[i] == '.'){
			break;
		}
	}
	outputTLB[i-1] = '2';
	* */
	ifstream inputStream;//to read
	ifstream vaStream;
	ofstream outStream;//to write
	ofstream outStreamTLB; //to write TLB content
	inputStream.open(init, ios:: in);
	vaStream.open(inputVA, ios:: in);
	outStream.open("182914141.txt", ios::out | ios::trunc);
	outStreamTLB.open("182914142.txt", ios::out | ios::trunc);
	
	//END OF FILE HANDLING LOGIC
	
	//CREATE THE PHYSICAL MEMORY FROM FILE INFORMATION
	//PARSE THE INPUT FOR PAIRS
	getline(inputStream, lineInput);
	while(lineInput[0] != '\n'){
		int firstSpace = lineInput.find(' ');
		int input = 0;
		int value = 0;
		if(firstSpace == string::npos){
			break; //no more "pairs!"
		}
		input = stoi(lineInput);
		lineInput = lineInput.substr(firstSpace+1);
		
		//no secondary value
		if(lineInput.length() < 1){
			break;
		}
		value = stoi(lineInput);
		memoryHandler.initializeST(mainMemory, input, value);
		//memoryHandler.printBitMap(value/FRAME_SIZE);
		if(lineInput.find(' ') == string::npos){
			break;
		}
		else{
			lineInput = lineInput.substr(lineInput.find(' ')+1);
		}
		
	}
	
	
	//PARSE THE INPUTS FOR TRIPLES
	getline(inputStream, lineInput);
	while(lineInput[0] != '\n'){
		int firstSpace = lineInput.find(' ');
		int secondSpace = lineInput.find(' ', firstSpace);
		int pageNum = 0;
		int segmentNum = 0;
		int address = 0;
		//no more "triples" (incase of leftover 1 or 2 in input)
		if(firstSpace == string::npos || secondSpace == string::npos){
			break;
		}
		pageNum = stoi(lineInput);
		lineInput = lineInput.substr(firstSpace+1);
		segmentNum = stoi(lineInput);
		lineInput = lineInput.substr(lineInput.find(' ')+1);
		//no third value
		if(lineInput.length() < 1){
			break;
		}
		address = stoi(lineInput);
		
		memoryHandler.initializePT(mainMemory, pageNum, segmentNum, address);
		if(lineInput.find(' ') == string::npos){
			break;
		}
		else{
			lineInput = lineInput.substr(lineInput.find(' ')+1);
		}
		
		
	}
	//END OF PHYSICAL MEMORY CREATION
	
	//TRANSLATION PROCESS
	getline(vaStream, lineInput);
	while(lineInput[0] != '\n'){
		int mode = 0;
		mode = stoi(lineInput);
		int firstSpace = lineInput.find(' ');
		lineInput = lineInput.substr(firstSpace+1);
		//TRANSLATE VA GIVEN
		int virtualAddress = stoi(lineInput);
		int segment = 0;
		int page = 0;
		int offset = 0;
		int physicalAddress = 0; //temporary holding
		memoryHandler.translateVA(virtualAddress, segment, page, offset);
		//READ MODE
		if(mode == 0){
			if(tlb.checkTLB(virtualAddress, physicalAddress)){
				tlb.printTLB();
				outStreamTLB << "h ";
				cout <<"Entry found in TLB for segment :" << segment << " page:"<< page <<"\n";
				cout << "it is located at : " << physicalAddress << endl;
				outStream << physicalAddress+offset << " ";
				outStreamTLB << physicalAddress+offset<< " ";
			}
			//not found
			else{
				//cout << "=======+READ MODE+=========\n";
				outStreamTLB << "m ";
				int readSuccess = memoryHandler.read(mainMemory, segment, page, offset, physicalAddress);
				//READ MODE
				if(readSuccess == 1){
					tlb.addEntry(virtualAddress, physicalAddress);
					outStream << physicalAddress+offset << " ";
					outStreamTLB << physicalAddress+offset<< " ";
					//cout << "output address(found read) = " << physicalAddress << endl;
				}
				else if(readSuccess == 0){
					outStream << "err ";
					outStreamTLB << "err ";
					//cout << "err : page does not exist" << endl;
					//this is for "error" for read
				}
				else{
					outStream << "pf ";
					outStreamTLB << "pf ";
					//cout << "pf\n";
					//this is for "page fault"
				}
			}
		}
		else{
		//WRITE MODE
			//cout << "=======+WRITE MODE+=========\n";
			if(tlb.checkTLB(virtualAddress, physicalAddress)){
				cout <<"Entry found in TLB!\n";
				outStreamTLB << "h ";
				cout << "it is located at : " << physicalAddress << endl;
				outStream << physicalAddress+offset << " ";
				outStreamTLB << physicalAddress+offset<< " ";
			}
			//not found
			else{
				outStreamTLB << "m ";
				int writeSuccess = memoryHandler.write(mainMemory, segment, page, offset, physicalAddress);
				if(writeSuccess < 0){
						outStream <<"pf ";
						outStreamTLB << "pf ";
				}
				else{
					tlb.addEntry(virtualAddress, physicalAddress);
					outStream << physicalAddress+offset << " ";
				outStreamTLB << physicalAddress+offset<< " ";
				}
			}
			//memoryHandler.printBitMap(physicalAddress/FRAME_SIZE);
		}
		//need to check if there's a hanging space at the end!
		if(lineInput.find(' ') == string::npos){
			break;
		}
		else{
			lineInput = lineInput.substr(lineInput.find(' ')+1);
		}
	}
	
	//CLOSING OF STREAMS
	//for checking the segment table
	/*
	for(int i = 0 ; i < 512; i++){
		cout << mainMemory[i] << " ";
	}
	* */
	
	cout<< endl;
	inputStream.close();
	outStream.close();
	outStreamTLB.close();
	vaStream.close();
	delete[](mainMemory);
	return 0;
}
