#include <iostream>
#include "WebServer.h"

using namespace std;

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        try {
            port = stoi(argv[1]);
        } catch (...) {
            port = 8080;
        }
    }

    cout << "==========================================================" << endl;
    cout << "   MeoLogi - He Thong Quan Ly Don Hang & Kho Logistics   " << endl;
    cout << "          Do An PBL 2 - Cong Nghe Thong Tin              " << endl;
    cout << "==========================================================" << endl;

    string dataDir = "data";
    if (argc > 2) {
        dataDir = argv[2];
    }

    WebServer server(port, "web", dataDir);
    server.start();

    return 0;
}
