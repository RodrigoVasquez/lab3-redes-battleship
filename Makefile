all:	
	g++ -o servidor servidor.cc -Wall
	g++ -o cliente cliente.cc -Wall
clean:
	rm servidor
	rm cliente

