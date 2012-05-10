#include <process.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

using namespace std;

const char* quote(const char* in) {
    string s(in);
    if (s.find(' ') != string::npos) {
        stringstream ss;
        ss << "\"" << s << "\""; 
        return ss.str().c_str();
    }
    return in;
}

int main( int argc, char *argv[] ) {
    // figure out what directory this exe is in
    char path[MAX_PATH];
    string filename(path, GetModuleFileName(NULL, path, MAX_PATH));
    filename = filename.substr(0, filename.find_last_of('\\')+1);
    filename.append("\\predirect.config");
    
    // open the config file
    ifstream configfile(filename.c_str());
    if (configfile.is_open()) {
        // read path to jproc from config file
        string proc;
        getline(configfile, proc);
        configfile.close();

        // call our real proc .exe (or whatever else we 
        const char* new_argv[argc+1];
        for (int i = 0; i < argc; i++) {
            if (i == 0)
                new_argv[0] = quote(proc.c_str());
            else
                new_argv[i] = quote(argv[i]);
        }
        new_argv[argc] = NULL;
        return _spawnv(_P_WAIT, proc.c_str(), new_argv);
    } else {
        cout << "could not find " << filename.c_str();
    }
}
