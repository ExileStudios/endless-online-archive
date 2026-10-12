#ifndef LoginsH
#define LoginsH

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

struct mySQLdb;

struct Asocketblock;
struct Asocketvip;
struct Logins;

struct Asocketblock
{
    String address;
    int count;
};

struct Asocketvip
{
    String name;
    String value;
};

// The connection guard: an ip that just connected is held off for a while; reserved names get a pass.
struct Logins
{
    mySQLdb *mysql_controls;
    TList *login_list;
    TList *reserved_names;
    String field_0xc;

    Logins(mySQLdb *mysql);
    ~Logins();
    void AddReservedName(String name);
    void Tick();
    bool HandleAddress(String address);
    void AddLogin(String address);
    void SetReservedIp(String name, String ip);
    bool IsReservedIp(String ip);
};

#endif
