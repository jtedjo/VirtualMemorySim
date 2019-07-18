# VirtualMemorySim

Simulation of Virtual Memory Translation.

The segments are divided into "pages" that might not be in contiguous location. Thus a page table is necessary in order to determine
the location of a particular segment's page. 
The main memory is simulated with an array of 524,288 int (corresponding to 1024 Frames * 512 words)
Single segment table and each point to a page table(PT). The page table entry itself points to program/data pages.
Virtual Address is 32 bits: 4 unnused, 9 bits indicating the segment number, 10 bits indicating the page number, and 9 for the offset within page. 

There are 3 possible entry for ST:
	-1: PT not in main memory currently
	0: PT does not exist. Read throws error, write create blank new PT.
	>0: PT starts at this physical address
	
Entry of PT indicates:
	-1:page is not in main memory, error message generated.
	0:page does not exist
	>0: page starts at physical address
	
Bitmap keep track of frames that are currently in use and those which are unused. 1024 bits(one per frame).
An init file consisting of two lines is used. The first line consists of two values:

s1f1 s2f2 ... snfn

the s indicates the segment and f is the address of the page table in memory for that particular segment.
The second line of the init file consists of

p1s1f1 p2s2f2 ... pmsmfm

the p indicates the page of particular segment s starts at the address f.
After setting up the main memory using the init files, an input file consisting of two consecutive values:

o1VA1 o2VA2 ...

o1 can either be 0 for read or 1 for write. VA is the virtual address and it is a positive integer. The virtual address is broken into the
segment number, page number, and offset as indicates above (9,10,9).

The TLB is a table with 4 lines and use the least recently used model. The ranking goes from 0-3.
The VA is compared against the TLB searching for match, and it would update the TLB (either adding a new
VA-Physical Address pairing, updating the LRU values as necessary, or in case of no hit, there is no change to TLB).

The input folder contains example of the init file and the address to be translated after initialization.