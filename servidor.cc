#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <stack>
#include <list>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <sstream>

using namespace std;

void inicializarTablero(vector<int> &);
void mostrarTablero(vector<int> );
int establecer_conexion(int &, int &, int &);
int leerMensaje(int, string &);
void enviarMensajeUnJugador(int , string );
void enviarMensajeDosJugadores(int ,int , string , string );
void enviarMensajeDosJugadores(int ,int , string );
void crearStringTableroJuego(vector<int> , string &);
void realizarJugada(vector<int> &, string , string , int &, int &);
void mostrarMensajeTriunfo(int , int );
string convertirIntToString(int );
bool quedanGlobos(vector<int> );
int numeroAleatorio();
int puerto=5009;

/*
 * Declaración de variables globales que indican los estados dentro del vector que representa el tablero
 * */

#define VACIO  0
#define JUGADOR_1  -1
#define JUGADOR_2  -2
#define GLOBO -3
#define DARDO_MISMO_LUGAR -4
#define GLOBO_REVENTADO -5


int main(int argc,char **argv)
{
	int descriptor = 0,  descriptorCliente1 = 0,  descriptorCliente2 = 0;
	
	if(establecer_conexion(descriptor, descriptorCliente1, descriptorCliente2) != 0){
		cout  << "TimeStamp["<<time(NULL)<<"]: Error: Ha ocurrido un error en la conexion" << endl;
		return 0;
	}
	vector<int> tableroJuego(25);
	inicializarTablero(tableroJuego);
	
	string mensajeEnviar(""), mensajeLeido, mensajeLeido1, mensajeLeido2;
	int contadorGlobosJ1=0, contadorGlobosJ2=0;
	bool jugando = true;
	// Se envian los numeros de los jugadores
	enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "1","2");
	while(jugando){
		
		mostrarTablero(tableroJuego);
		// Se envia el tablero de juego
		crearStringTableroJuego(tableroJuego,mensajeEnviar);
		enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, mensajeEnviar);

		//leer posicion de ambos jugadores
		leerMensaje(descriptorCliente1, mensajeLeido1);
		leerMensaje(descriptorCliente2, mensajeLeido2);
		
		//Se realiza la jugada y cambian los valores
		realizarJugada(tableroJuego,mensajeLeido1, mensajeLeido2, contadorGlobosJ1, contadorGlobosJ2);
		mensajeEnviar = "";
		// Si quedan no quedan globos se crea un mensaje con los valores a a los clientes y se muestra un mensaje de triunfo
		if(quedanGlobos(tableroJuego)==false){
			mensajeEnviar.append("1").append("?").append(convertirIntToString(contadorGlobosJ1)).append("?").append(convertirIntToString(contadorGlobosJ2));
			mostrarMensajeTriunfo( contadorGlobosJ1,  contadorGlobosJ2);
		}else{
		// Se crea un mensaje con el estatus del juego hasta el momento
			mensajeEnviar.append("0").append("?").append(convertirIntToString(contadorGlobosJ1)).append("?").append(convertirIntToString(contadorGlobosJ2));
		}
		// se envia el mensaje
		enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, mensajeEnviar);
		
		//Se recibe la respuesta por si los dos jugadores quieren seguir jugando.
		if(quedanGlobos(tableroJuego) == false){
			leerMensaje(descriptorCliente1, mensajeLeido1);
			leerMensaje(descriptorCliente2, mensajeLeido2);
			// Si al menos 1 de los dos no quiere seguir jugando entonces se cambia el estado de la variable jugando
			if(mensajeLeido1 == "2" || mensajeLeido2 == "2") {
				jugando = false;
			}
			mensajeEnviar = "";
			mensajeEnviar.append(mensajeLeido1).append("?").append(mensajeLeido2);	
			enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2,mensajeEnviar);
			// Si reinicia el tablero y los contadores de globos reventados.
			inicializarTablero(tableroJuego);
			contadorGlobosJ1=0;
			contadorGlobosJ2=0;
			
		}
	}
	// Se cierran los desriptores y se termina el juego.
	close(descriptor);
	close(descriptorCliente1);
	close(descriptorCliente2);
	cout << "TimeStamp["<<time(NULL) << "]: El juego ha terminado" << endl;
}
/*
 * Se reciben los contadores de los globos reventados de ambos jugadores y se muestra un mensaje de triunfo
 * */
void mostrarMensajeTriunfo(int contadorGlobosJ1, int contadorGlobosJ2){
		if(contadorGlobosJ1 == contadorGlobosJ2){
				cout << "TimeStamp["<<time(NULL) << "]: La partida ha terminado en empate" << endl;	
					
		}else if(contadorGlobosJ1 > contadorGlobosJ2){
				cout << "TimeStamp["<<time(NULL) << "]: Ha ganado el jugador N°1" << endl;	
					
		}else{
				
				cout << "TimeStamp["<<time(NULL) << "]: Ha ganado el jugador N°2" << endl;
		}
}
/* Se recibe el tablero de juego y se retorna true o false dependiendo si quedan globos
 * 
 */
bool quedanGlobos(vector<int> tableroJuego){
		vector<int>::iterator it;
		for( it = tableroJuego.begin(); it != tableroJuego.end(); ++it){
				if ((*it) == GLOBO) 
					return true;
		}
		return false;
}

/*
 * Se realiza una jugada se reciben las 2 posiciones que colocaron los jugadores y los contadores de globos se actualiza el 
 * tablero y se muestran mensajes en el servidor 
 */
void realizarJugada(vector<int> &tableroJuego, string mensajeLeido1, string mensajeLeido2, int &contadorGlobosJ1, int &contadorGlobosJ2){
		int posicion1 = atoi(mensajeLeido1.c_str()) - 1;
		int posicion2 = atoi(mensajeLeido2.c_str()) - 1;
		
		if(posicion1 == posicion2){
			if(tableroJuego[posicion1] == VACIO){
				tableroJuego[posicion1] = DARDO_MISMO_LUGAR;
			}
			if (tableroJuego[posicion1] == GLOBO){ 
				tableroJuego[posicion1] = GLOBO_REVENTADO;
				cout << "TimeStamp["<<time(NULL) << "]: El jugador N°1 ha reventado un globo" << endl;
				cout << "TimeStamp["<<time(NULL) << "]: El jugador N°2 ha reventado un globo" << endl;
				contadorGlobosJ1++;
				contadorGlobosJ2++;		
			}
		}else{
			if(tableroJuego[posicion1] == GLOBO){
				contadorGlobosJ1++;
				cout << "TimeStamp["<<time(NULL) << "]: El jugador N°1 ha reventado un globo" << endl;
				tableroJuego[posicion1] = GLOBO_REVENTADO;
			}
			if(tableroJuego[posicion2] == GLOBO){
				contadorGlobosJ2++;
				cout << "TimeStamp["<<time(NULL) << "]: El jugador N°2 ha reventado un globo" << endl;
				tableroJuego[posicion2] = GLOBO_REVENTADO;
			}
			if(tableroJuego[posicion1] == VACIO){
				tableroJuego[posicion1] = JUGADOR_1;
			}
			if(tableroJuego[posicion2] == VACIO){
				tableroJuego[posicion2] = JUGADOR_2;
			}
		}
}
/*
 * Se lee un mensaje que llegue en un descriptor que este utilizando el servidor
 * */
int leerMensaje(int descriptor, string &mensajeRecibido){
	char buffer[100];
	memset(buffer,0,100);
	if(read(descriptor, buffer, 100) == -1){
			cout  << "TimeStamp["<<time(NULL)<<"]: Error 5 no se puede leer del descriptor" << endl;
			return -5;
	}
	mensajeRecibido=buffer;
	return 0;
}
/*
 * Se envia un mensaje mediante un descriptor
 * */
void enviarMensajeUnJugador(int descriptor, string mensaje){
	write(descriptor, mensaje.c_str(), 100);
}
/*
 * Se envia dos mensajes distintosr dos descriptores distintos
 *  */
void enviarMensajeDosJugadores(int descriptor1,int descriptor2, string mensaje1, string mensaje2){
	write(descriptor1, mensaje1.c_str(), 100);
	write(descriptor2, mensaje2.c_str(), 100);
}
/*
 *  Se envia el mismo mensaje por dos descriptores distintos
 */
void enviarMensajeDosJugadores(int descriptor1,int descriptor2, string mensaje){
	write(descriptor1, mensaje.c_str(), 100);
	write(descriptor2, mensaje.c_str(), 100);
}

/*
 * Se inicializa el tablero de juego colocando aleatoriamente los globos en este.
 * 
 * */
void inicializarTablero(vector<int> &tableroJuego){
		vector<int>::iterator it;
		int cantidadGlobos = 0;
		int aleatorio;
		for( it = tableroJuego.begin(); it != tableroJuego.end(); ++it){
				(*it) = 0;
		}
		srand(time(NULL));
		while(cantidadGlobos < 5){
			aleatorio = numeroAleatorio();
			if(tableroJuego[aleatorio] != GLOBO){
				tableroJuego[aleatorio] = GLOBO;
				cantidadGlobos++;
			}
		}
}
/*
 * Se crea un string el cual representa al tablero de juego que es enviado a los clientes.
 * */
void crearStringTableroJuego(vector<int> tableroJuego, string &aux){
	int i;
	aux = "";
	for(i = 0 ; i<25;i++){
			if(i > 9){
				if(tableroJuego[i] == VACIO) aux.append(convertirIntToString(i+1)).append("?");
				if(tableroJuego[i] == GLOBO) aux.append(convertirIntToString(i+1)).append("?");
			}else{
				if(tableroJuego[i] == VACIO) aux.append(convertirIntToString(i+1)).append("?");
				if(tableroJuego[i] == GLOBO) aux.append(convertirIntToString(i+1)).append("?");
			}
			if(tableroJuego[i] == JUGADOR_1) aux.append("X?");
			if(tableroJuego[i] == JUGADOR_2) aux.append("Y?");
			if(tableroJuego[i] == GLOBO_REVENTADO) aux.append("O?");
			if(tableroJuego[i] == DARDO_MISMO_LUGAR) aux.append("-?");
			if(i==4 || i==9 ||i==14 ||i==19) aux.append("\n");
	}
	aux.append("\n");
}
/*
 * Se convierte un entero en int
 * */
string convertirIntToString(int i){
	stringstream ss;
	ss << i;
	return ss.str();
}
/*
 * Se muestra el tablero de juego como mensaje en el servidor
 * 
 * */
void mostrarTablero(vector<int> tableroJuego){

		unsigned int i;
		cout << "   Tablero de Juego" << endl;
		cout << "---------------------" << endl;
		for(i=0;i<tableroJuego.size();i++){
			if(tableroJuego[i] == VACIO) cout << "|   ";
			if(tableroJuego[i] == GLOBO) cout << "| O ";
			if(tableroJuego[i] == JUGADOR_1) cout << "| X ";
			if(tableroJuego[i] == JUGADOR_2) cout << "| Y ";
			if(tableroJuego[i] == GLOBO_REVENTADO) cout << "| O ";
			if(tableroJuego[i] == DARDO_MISMO_LUGAR) cout << "| - ";
			if(i==24) cout << "|";
			if(i==4 || i==9 ||i==14 ||i==19) {
				cout << "|"<<endl;
				cout << "---------------------" << endl;				
			}
		}
		cout << endl <<"---------------------" << endl;
		cout << endl;
}

/*
 * Se genera un numero aleatorio de 1-25 para poder llenar el tablero
 * */
int numeroAleatorio(){
	return rand() % 25;
}

/*
 * Funcion que se encarga de realizar la conexion con dos clientes.
 * */
int establecer_conexion(int &descriptor, int &descriptorCliente1, int &descriptorCliente2){
//SE ABRE EL DESCRIPTOR DEL SOCKET(SE CREA EL SOCKET)
	struct sockaddr_in direccion, cliente1, cliente2; 
	descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (descriptor == -1)
	{
    		cout  << "TimeStamp["<<time(NULL)<<"]: Error 1" << endl;
		return -1;
	}
	//AHORA SE AVISA AL SISTEMA OPERATIVO QUE SE TIENE ABIERTO EL SOCKET Y QUE SE VA A ATENDER EL SERVICIO POR EL PUERTO DADO
	direccion.sin_family = AF_INET;//TIPO DE CONEXION A ORDENADORES DE CUALQUIER TIPO
	direccion.sin_port = htons(puerto);
	direccion.sin_addr.s_addr =INADDR_ANY;//DIECCION DEL CLIENTE,,,,SE ATIENDE A CUALQUIERA

	if ( bind(descriptor, (struct sockaddr *)&direccion, sizeof (direccion)) == -1)
	{
		cout  << "TimeStamp["<<time(NULL)<<"]: Error 2" << endl;
		return -2;
	} 

	//AHORA SE DICE AL SISTEMA QUE COMIENCE A ATENDEER LAS LLAMADAS QUE LLEGUEN, CON EL DESCRIPTOR DEL SOCKET Y EL NUMERO MAXIMO DE CLIENTES A ENCOLAR
	if (listen(descriptor, 1) == -1)
	{
		cout  << "TimeStamp["<<time(NULL)<<"]: Error 3" << endl;
		return -3;
	}

	//CON ESTO SE ESPERA Y ATIENDE A CUALQUIER CLIENTE QUE LLEGUE, CON LOS DATOS QUE SE SACAN DEL CLIENTE DE LA LLAMADA ACCEPT
	socklen_t longitudCliente1 = sizeof(cliente1);

	descriptorCliente1 = accept(descriptor, (struct sockaddr*)&cliente1, &longitudCliente1);
	if (descriptorCliente1 == -1)
	{
    		cout  << "TimeStamp["<<time(NULL)<<"]: Error 4" << endl;
		return -4;
	} 
	//CON ESTO SE ESPERA Y ATIENDE A CUALQUIER CLIENTE QUE LLEGUE, CON LOS DATOS QUE SE SACAN DEL CLIENTE DE LA LLAMADA ACCEPT
	
	socklen_t longitudCliente2 = sizeof(cliente2);

	descriptorCliente2 = accept(descriptor, (struct sockaddr*)&cliente1, &longitudCliente2);
	if (descriptorCliente2 == -1)
	{
    		cout  << "TimeStamp["<<time(NULL)<<"]: Error 4" << endl;
		return -4;
	}
	return 0;

}

