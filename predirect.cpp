// predirect.cpp
// by Lawrence McAlpin (admin@lmcalpin.com)
//
// Simply delegates execution to another executable indicated by a config file.  The
// config file should be named with the name of the redirecting executable (the one you
// get when you compile this solution) suffixed by ".predirect.config".  The contents
// should simply be the name of the ACTUAL executable you want to run.
// 
// For example, if you want to alias "C:\programming\languages\jruby.exe" as "ruby.exe",
// compile this solution, rename the resulting predirect.exe as ruby.exe, and create
// a ruby.exe.predirect.config text file with "C:\programming\languages\jruby.exe" as
// the sole contents.
//
// In other words, this is a poor man's symlink.  For executables only.  
// 
// Obviously, real symlinks are superior, but we're using Windows, and symlinks aren't
// supported consistently, so here we are.

#include "stdafx.h"
#include <process.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

using namespace std;

HANDLE job = 0;
HANDLE processId = 0;
HANDLE threadId = 0;

const char* quote(const char* in) {
    string s(in);
    if (s.find(' ') != string::npos) {
        stringstream ss;
        ss << "\"" << s << "\""; 
        return ss.str().c_str();
    }
    return in;
}

void exitHandler() {
    if (threadId) CloseHandle(threadId);
    if (processId) CloseHandle(processId);
    if (job) CloseHandle(job);
}

int main( int argc, char *argv[] ) {
    // create a handler in case something fails and the process is killed
    atexit(exitHandler);
    
    // figure out what directory this exe is in
    char path[MAX_PATH];
    string filename(path, GetModuleFileName(NULL, path, MAX_PATH));
    string cmd;
    string curdir = filename.substr(0, filename.find_last_of('\\')+1);
	string exename = filename.substr(filename.find_last_of('\\') + 1, filename.length() - curdir.length());
	filename = curdir.append(exename).append(".predirect.config");
    
    // open the config file
    ifstream configfile(filename);
    if (configfile.is_open()) {
        // read path to jproc from config file
        getline(configfile, cmd);
        configfile.close();
    } else {
        cerr << "Configuration " << filename << " not found\n";
        return -1;
    }

	// see if the target file exists; if not, see if it exists in the current
	// directory (that the predirect EXE is located in)
	ifstream targetproc(cmd);
	if (targetproc.good()) {
		targetproc.close();
	}
	else {
		string cmdInCurrentDir = curdir.append(cmd);
		ifstream targetproccur(cmdInCurrentDir);
		if (targetproccur.good()) {
			cmd = cmdInCurrentDir;
		}
		else {
			cerr << cmdInCurrentDir << " not found\n";
			return false;
		}
	}

	// pass command line arguments to the child process
	std::stringstream argBuilder;
	for (int i = 0; i < argc; i++) {
        if (i == 0)
            argBuilder << quote(cmd.c_str());
        else
            argBuilder << " " << quote(argv[i]);
    }
	std::string args = argBuilder.str();

	// set up child process
    STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
    PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(PROCESS_INFORMATION));
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeliBajeli;
	memset(&jeliBajeli, 0, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
    
    startupInfo.cb = sizeof(STARTUPINFO);
    job = CreateJobObject(NULL, NULL);
    jeliBajeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(job, JobObjectExtendedLimitInformation, &jeliBajeli, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
    
    // call our real proc .exe
    if (!CreateProcess(cmd.c_str(), (LPSTR)args.c_str(), 0, 0, TRUE, CREATE_SUSPENDED, 0, NULL, &startupInfo, &processInfo)) {
		cerr << "Error starting process: " << cmd.c_str() << "\n";
		return -1;
	}
    threadId = processInfo.hThread;
    processId = processInfo.hProcess;
    AssignProcessToJobObject(job, processId);
    ResumeThread(threadId);
    
    WaitForSingleObject(processId, INFINITE);
    return 0;
}


