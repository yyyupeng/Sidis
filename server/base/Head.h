#pragma once

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <netinet/in.h>
#include <netdb.h>
#include <random>
#include <cfloat>

#define MAXLINE 4096 
#define LISTENQ 2048
#define SKIPLIST_MAXLEVEL 32
#define SKIPLIST_P 0.25
#define DBNUM 16

typedef long long LL;

using namespace std;
using namespace std::placeholders;
