#include <iostream>
#include <fstream>
#include <thread>
#include "CPU6502.h"

using namespace std;

int main() {
    Cartridge cart("nestest.nes");
    RAM ram(&cart);
    CPU6502 cpu(&ram);
    ofstream log("log.txt");

    if(log.is_open()) {
        for(int i = 0;i < 10000;i++) {
            //cout<<cpu.debug();
            cpu.clock();
            log<<cpu.debug();
        }

        log.close();
    }
    else {
        cout << "Could not open log" << endl;
    }

    return 0;
}