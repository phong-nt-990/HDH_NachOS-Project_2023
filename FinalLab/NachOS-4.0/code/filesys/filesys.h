// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
	// We define two separate implementations of the file system.
	// The "STUB" version just re-defines the Nachos file system
	// operations as operations on the native UNIX file system on the machine
	// running the Nachos simulation.

	// The other version is a "real" file system, built on top of
	// a disk simulator.  The disk is simulated using the native UNIX
	// file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H


// temporary===================
#define FILESYS_STUB // this help to debug, delete it after complete

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#include "debug.h" 	
#include <string.h>

#ifdef FILESYS_STUB // Temporarily implement file system calls as
#define FILE_MAX 20
typedef int OpenFileID;
// calls to UNIX, until the real file system
// implementation is available
// #define CONSOLE_IN 0
// #define CONSOLE_OUT 1
// #define MODE_READWRITE 0
// #define MODE_READ 1
// #define MODE_WRITE 2


// File Descriptor Table using to save fileid and socket id
struct FD_Table
{
	OpenFile *fileID;
	int socketID;
	// int FD_flag;
};




class FileSystem
{
private:
	// char** openedFileName = new char*[FILE_MAX];
	string openedFileName[FILE_MAX];
public:
	FD_Table* table = new FD_Table[FILE_MAX];
    // OpenFile** openFile = new OpenFile*[FILE_MAX];

	// FileSystem() {}
	FileSystem()
	{
    	int fd_index = 0;
		//fileOpenMode = new int[FILE_MAX];
		// fileOpenMode[CONSOLE_IN] = MODE_READ;
		// fileOpenMode[CONSOLE_OUT] = MODE_WRITE;
		for(int i=0;i<FILE_MAX;i++)
		{
			// fd_index = i
			// openFile[i] = NULL;
			table[i].fileID = NULL;
			table[i].socketID = 0;
		}
		this->Create("stdin",0);
		this->Create("stdout",0);
		
		table[fd_index++].fileID = this->Open("stdin");
		table[fd_index++].fileID = this->Open("stout");


		for (int i = 0; i < FILE_MAX; i++)
		{
			openedFileName[i] = "";
		}
		
		// openFile[fd_index++] = this->Open("stdin");
		// openFile[fd_index++] = this->Open("stout");
	}

	~FileSystem()
	{
		for (int i = 0; i < FILE_MAX; ++i)
		{
			// if (openFile[i] != NULL) delete openFile[i];
			if (table[i].fileID != NULL) delete table[i].fileID;
		}
		// for (int i = 0; i < FILE_MAX; ++i)
		// {
		// 	// if (openFile[i] != NULL) delete openFile[i];
		// 	if (this->openedFileName[i] != NULL) delete openedFileName[i];
		// }
		delete[] table;
		// delete[] openedFileName;
	}

	bool Create(char *name, int initialSize)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor, name);
	}

	int FindFreeSlot()
	{
		for(int i = 2; i < FILE_MAX; i++)
		{
			// if(openFile[i] == NULL) return i;	
			if(table[i].fileID == NULL) return i;	
			// DEBUG(dbgSys, "\n ds");	
		}
		return -1;
	}
	OpenFile* Open(char *name, int type)
	{	
		int fileDescriptor;
		if(type==0){
			fileDescriptor = OpenForReadWrite(name, FALSE);
		}
		else if(type==1){
			fileDescriptor = OpenForRead(name, FALSE);
		}
		// freeSlot = FindFreeSlot();
		// openedFileName[freeSlot] = name;
		// else{
		// 	fileDescriptor = -1;
		// }
		if (fileDescriptor == -1) return NULL;
		// fd_index++;
		// openfile[fd_index++] = OpenFile(fileDescriptor,type);
		return new OpenFile(fileDescriptor,type, name);
	}
	int checkIfFileOpened(char* filename)
	{
		string tmp;
		string filenameStr(filename);
		for (int i = 0; i < FILE_MAX; i++) {
			tmp = this->openedFileName[i];
			// printf("a");
			// printf(tmp);
			// printf("a");
			if ((tmp != "") && (tmp == filenameStr)) {
				// The filename is found in the array
				// kernel->machine->WriteRegister(2, -1);
				// DEBUG(dbgSys, "\n File was opened.");
				// delete[] filename;
				// move_program_counter();
				// return;
				// DEBUG(dbgSys, "index error" << i);
				return 1;
			}
		}
		return 0;
	}

	bool Close(int fid)
	{
		if (this->table[fid].fileID) // neu mo file thanh cong
		{
			delete this->table[fid].fileID; // Xoa vung nho luu tru file
			this->table[fid].fileID = NULL; // Gan vung nho NULL
			this->openedFileName[fid] = "";
			// DEBUG(dbgSys, "Close file successfully.");
			// kernel->machine->WriteRegister(2, 0);
			// break;
			return 1;
		}
		return 0;
	}
	
	char* getFilenameInOpenSet(int index)
	{
		string data = this->openedFileName[index];
		const int length = data.length(); 
		char* res = new char[length + 1];
		strcpy(res, data.c_str());
		return res;
		// return stringTochar(data);
	}
	void addFilenameToOpenset(int index,char* filename)
	{
		string filenameStr(filename);
		this->openedFileName[index] = filenameStr;
	}
	//getter, setter ha
	// OpenFile* Open(char *name, int type)
	// {	
	// 	int fileDescriptor = OpenForReadWrite(name, FALSE);
	// 	if (fileDescriptor == -1) return NULL;
	// 	//index++;
	// 	return new OpenFile(fileDescriptor, type);
	// }

	bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem
{
public:
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)


	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

	int FindFreeSlot();
private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
