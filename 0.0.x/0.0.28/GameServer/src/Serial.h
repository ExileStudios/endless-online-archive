#ifndef SerialH
#define SerialH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>



struct SerialKey;

// The server's licence, from config/serial.ini, and the registered name in pub/dvf001.evf.
struct SerialKey
{
    TStringList *ini_file;
    String key_base;
    String key_base_copy;
    String serial_code;
    String unlock_code;
    String reg_name;
    bool valid;
    int counter;

    SerialKey();
    ~SerialKey();
    void LoadIni(String path);
    String ReadKey(String key, String default_value);
    void CloseIni();
    String GetKeyBaseCopy();
    String GetUnlockCode();
    String GetRegName();
    String GetDisplayCode();
    bool IsValid();
    void Validate();
    String DecodeString(String src);
    int GetCounter();
    void SetCounter(int value);
};


#endif
