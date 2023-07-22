g++ -std=c++17 -c -O2 helper.cpp -o helper.o
g++ -std=c++17 -O2 server.cpp helper.o -o server.o
g++ -std=c++17 -O2 client.cpp helper.o -o client.o
