#include "rpc/RPCServer.h"

int add(int a,int b){
    return a+b;
}
int sub(int a,int b){
    return a-b;
}


int main(){
    RPCServer server;
    server.registerMethod("add", add);
    server.registerMethod("sub", sub);
    server.start(8081);
    server.run();



    return 0;
}