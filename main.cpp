#include <iostream>
#include "network/Service.h"
int main() {
    networking::Service network(25565);
    network.start();
    return 0;
}