// predirect.cpp
//

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
	filename = curdir;
	filename.append("\\");
	filename.append(exename);
	filename.append(".predirect.config");
    
    // open the config file
    ifstream configfile(filename);
    if (configfile.is_open()) {
        // read path to jproc from config file
        getline(configfile, cmd);
        configfile.close();
    } else {
        cout << "Configuration " << filename << " not found\n";
        return -1;
    }

	// see if the target file exists; if not, see if it exists in the current
	// directory (that the predirect EXE is located in)
	ifstream targetproc(cmd);
	if (targetproc.good()) {
		targetproc.close();
	}
	else {
		string cmdInCurrentDir = curdir;
		cmdInCurrentDir.append("\\");
		cmdInCurrentDir.append(cmd);
		ifstream targetproccur(cmdInCurrentDir);
		if (targetproccur.good()) {
			cmd = cmdInCurrentDir;
		}
		else {
			cout << cmdInCurrentDir << " not found\n";
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
		cout << "Error starting process: " << cmd.c_str() << "\n";
		return -1;
	}
    threadId = processInfo.hThread;
    processId = processInfo.hProcess;
    AssignProcessToJobObject(job, processId);
    ResumeThread(threadId);
    
    WaitForSingleObject(processId, INFINITE);
    return 0;
}


