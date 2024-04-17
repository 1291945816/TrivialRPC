//
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include "net/common.h"


int main(){
    std::cout << get_local_ip();
}