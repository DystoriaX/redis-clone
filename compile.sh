g++ -std=c++17 -c -O2 helper.cpp -o helper.o
g++ -std=c++17 -c -O2 parser.cpp -o parser.o
g++ -std=c++17 -c -O2 data.cpp -o data.o
g++ -std=c++17 -c -O2 store.cpp -o store.o
g++ -std=c++17 -c -O2 cmd.cpp -o cmd.o
g++ -std=c++17 -O2 server.cpp helper.o parser.o data.o store.o cmd.o -o server.o
g++ -std=c++17 -O2 client.cpp helper.o parser.o data.o store.o cmd.o -o client.o
