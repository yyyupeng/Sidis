#pragma once

#include "../base/Head.h"

class dbStatus
{
public:
    dbStatus() : dbState(kOk), msg("") {}
    dbStatus(int dbState_, const string msg_) : dbState(dbState_), msg(msg_) {}
    ~dbStatus() {}
    static dbStatus Ok() { return dbStatus(); }
    static dbStatus notFound(const string& msg) { return dbStatus(kNotFound, msg); }
    static dbStatus IOError(const string& msg) { return dbStatus(kIOError, msg); }
    string toString()
    {
        if(msg == "")
        return "+OK";
        else
        {
            const char *type;
            switch(dbState)
            {
            case kOk:
                type = "+OK";
                break;
            case kNotFound:
                type = "-notFound: ";
                break;
            case kIOError:
                type = "-IO error: ";
                break;
            default:
                break;
            }
            string res(type);
            res.append("\r\n" + msg);
            return res;
       }
    }

private:
    enum code{ kOk, kNotFound, kIOError };
    int dbState;
    std::string msg;
};

