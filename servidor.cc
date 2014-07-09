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
void mostrarTablero(int[100] );
int establecer_conexion(int &, int &, int &);
int leerMensaje(int, string &);
void enviarMensajeUnJugador(int , string );
void enviarMensajeDosJugadores(int ,int , string , string );
void enviarMensajeDosJugadores(int ,int , string );
void crearStringTableroJuego(vector<int> , string &);
void realizarJugada(int t1[100],int t2[100], string , string );
string convertirIntToString(int );
void split(string s, char delim, vector<string> &elementos) ;
void inicializarTableros(int tableroJuego1[100], int tableroJuego2[100], string mensaje1, string mensaje2);
void crearStringTableroJuegoPropio(int tableroJuego[100], string &aux);
void crearStringTableroJuegoDelOtro(int tableroJuego[100], string &aux);
int barcoMuerto(int tableroJuego[100], int barco);
int hayGanador(int tableroJuego1[100], int tableroJuego2[100]);
int puerto=5009;

/*
 * Declaración de variables globales que indican los estados dentro del vector que representa el tablero
 * */

#define BOTE_PATRULLA -1
#define DESTRUCTOR -2
#define SUBMARINO -3
#define BARCO_BATALLA -4
#define PORTA_AVIONES -5 
#define DISPARO_J1 -6
#define DISPARO_J2 -7
#define ACIERTO_J1 -8
#define ACIERTO_J2 -9


int main(int argc,char **argv)
{
	int descriptor = 0,  descriptorCliente1 = 0,  descriptorCliente2 = 0;
	
	if(establecer_conexion(descriptor, descriptorCliente1, descriptorCliente2) != 0){
		cout  << "TimeStamp["<<time(NULL)<<"]: Error: Ha ocurrido un error en la conexion" << endl;
		return 0;
	}
	int tableroJuego1[100];
	int tableroJuego2[100];

	string mensajeEnviar(""), mensajeLeido, mensajeLeido1, mensajeLeido2;
	bool jugando = true;
	// Se envian los numeros de los jugadores
	enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "1","2");

	leerMensaje(descriptorCliente1, mensajeLeido1);
	leerMensaje(descriptorCliente2, mensajeLeido2);

	inicializarTableros(tableroJuego1, tableroJuego2, mensajeLeido1, mensajeLeido2);

	mostrarTablero(tableroJuego1);
	mostrarTablero(tableroJuego2);

	int ganador;

	while(jugando){

		// se envian los tableros en formato de string
		crearStringTableroJuegoPropio(tableroJuego1, mensajeEnviar);
		enviarMensajeUnJugador(descriptorCliente1, mensajeEnviar);

		crearStringTableroJuegoPropio(tableroJuego2, mensajeEnviar);
		enviarMensajeUnJugador(descriptorCliente2, mensajeEnviar);

		crearStringTableroJuegoDelOtro(tableroJuego2, mensajeEnviar);
		enviarMensajeUnJugador(descriptorCliente1, mensajeEnviar);

		crearStringTableroJuegoDelOtro(tableroJuego1, mensajeEnviar);
		enviarMensajeUnJugador(descriptorCliente2, mensajeEnviar);

		// se reciben las jugadas
		leerMensaje(descriptorCliente1, mensajeLeido1);
		leerMensaje(descriptorCliente2, mensajeLeido2);

		realizarJugada(tableroJuego1,  tableroJuego2,  mensajeLeido1,  mensajeLeido2);

		ganador = hayGanador(tableroJuego1, tableroJuego2);

		if (ganador == 3){
			enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "0");
			leerMensaje(descriptorCliente1, mensajeLeido1);
			leerMensaje(descriptorCliente2, mensajeLeido2);
			// Si al menos 1 de los dos no quiere seguir jugando entonces se cambia el estado de la variable jugando
			if(mensajeLeido1 == "2" || mensajeLeido2 == "2") {
				jugando = false;
				enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"0");
			}
			else{
				enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"1");
				leerMensaje(descriptorCliente1, mensajeLeido1);
				leerMensaje(descriptorCliente2, mensajeLeido2);
				inicializarTableros(tableroJuego1, tableroJuego2, mensajeLeido1, mensajeLeido2);
				mostrarTablero(tableroJuego1);
				mostrarTablero(tableroJuego2);
			}
			
		}else if (ganador == 1){
			enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "1");
			leerMensaje(descriptorCliente1, mensajeLeido1);
			leerMensaje(descriptorCliente2, mensajeLeido2);
			// Si al menos 1 de los dos no quiere seguir jugando entonces se cambia el estado de la variable jugando
			if(mensajeLeido1 == "2" || mensajeLeido2 == "2") {
				jugando = false;
			enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"0");
			}
			else{
				enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"1");
				leerMensaje(descriptorCliente1, mensajeLeido1);
				leerMensaje(descriptorCliente2, mensajeLeido2);
				inicializarTableros(tableroJuego1, tableroJuego2, mensajeLeido1, mensajeLeido2);
				mostrarTablero(tableroJuego1);
				mostrarTablero(tableroJuego2);
			}
			// 1 gana
			
		}else if (ganador == 2){
			enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "2");
			// 2 gana
			leerMensaje(descriptorCliente1, mensajeLeido1);
			leerMensaje(descriptorCliente2, mensajeLeido2);
			// Si al menos 1 de los dos no quiere seguir jugando entonces se cambia el estado de la variable jugando
			if(mensajeLeido1 == "2" || mensajeLeido2 == "2") {
				jugando = false;
				enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"0");

			}
			else{
				enviarMensajeDosJugadores(descriptorCliente1,descriptorCliente2,"1");
				leerMensaje(descriptorCliente1, mensajeLeido1);
				leerMensaje(descriptorCliente2, mensajeLeido2);
				inicializarTableros(tableroJuego1, tableroJuego2, mensajeLeido1, mensajeLeido2);
				mostrarTablero(tableroJuego1);
				mostrarTablero(tableroJuego2);
			}
		}else{
			enviarMensajeDosJugadores(descriptorCliente1, descriptorCliente2, "3");
		}



	}

	// Se cierran los desriptores y se termina el juego.
	close(descriptor);
	close(descriptorCliente1);
	close(descriptorCliente2);
	cout << "TimeStamp["<<time(NULL) << "]: El juego ha terminado" << endl;
}


int hayGanador(int tableroJuego1[100], int tableroJuego2[100]){

	int muertoBotePatrulla1, muertoDestructor1, muertoSubmarino1, muertoBarcoBatalla1, muertoPortaAviones1;
	int muertoBotePatrulla2, muertoDestructor2, muertoSubmarino2, muertoBarcoBatalla2, muertoPortaAviones2;

	muertoBotePatrulla1 = barcoMuerto(tableroJuego1, BOTE_PATRULLA);
	muertoDestructor1 = barcoMuerto(tableroJuego1, DESTRUCTOR);
	muertoSubmarino1 = barcoMuerto(tableroJuego1, SUBMARINO);
	muertoBarcoBatalla1 = barcoMuerto(tableroJuego1, BARCO_BATALLA);
	muertoPortaAviones1 = barcoMuerto(tableroJuego1, PORTA_AVIONES);

	muertoBotePatrulla2 = barcoMuerto(tableroJuego2, BOTE_PATRULLA);
	muertoDestructor2 = barcoMuerto(tableroJuego2, DESTRUCTOR);
	muertoSubmarino2 = barcoMuerto(tableroJuego2, SUBMARINO);
	muertoBarcoBatalla2 = barcoMuerto(tableroJuego2, BARCO_BATALLA);
	muertoPortaAviones2 = barcoMuerto(tableroJuego2, PORTA_AVIONES);

	int ganador2 = muertoBotePatrulla1 | muertoDestructor1 | muertoSubmarino1 | muertoBarcoBatalla1 | muertoPortaAviones1;
	int ganador1 = muertoBotePatrulla2 | muertoDestructor2 | muertoSubmarino2 | muertoBarcoBatalla2 | muertoPortaAviones2;

	if(ganador1 == 0 && ganador2 == 0){
		return 3;
	}

	if(ganador1 == 0){
		return 1;
	}

	if(ganador2 == 0){
		return 2;
	}

	return 4;
}


/*
 * Se realiza una jugada se reciben las 2 posiciones que colocaron los jugadores y los contadores de globos se actualiza el 
 * tablero y se muestran mensajes en el servidor 
 */
void realizarJugada(int tableroJuego1[100], int tableroJuego2[100],  string mensajeLeido1, string mensajeLeido2){
		int posicion1 = atoi(mensajeLeido1.c_str()) - 1;
		int posicion2 = atoi(mensajeLeido2.c_str()) - 1;

		if (tableroJuego1[posicion2] == BOTE_PATRULLA || tableroJuego1[posicion2] == DESTRUCTOR || tableroJuego1[posicion2] == SUBMARINO || tableroJuego1[posicion2] == BARCO_BATALLA || tableroJuego1[posicion2] == PORTA_AVIONES){
			tableroJuego1[posicion2] = ACIERTO_J1;
			cout << "TimeStamp["<<time(NULL) << "]: El jugador N°1 ha acertado en la posicion "<< posicion2 << endl;
		}else{
			tableroJuego1[posicion2] = DISPARO_J1;
			cout << "TimeStamp["<<time(NULL) << "]: El jugador N°1 ha fallado en la posicion"<< posicion2 << endl;
		}
		if (tableroJuego2[posicion1] == BOTE_PATRULLA || tableroJuego2[posicion1] == DESTRUCTOR || tableroJuego2[posicion1] == SUBMARINO || tableroJuego2[posicion1] == BARCO_BATALLA || tableroJuego2[posicion1] == PORTA_AVIONES){
			tableroJuego2[posicion1] = ACIERTO_J2;
			cout << "TimeStamp["<<time(NULL) << "]: El jugador N°2 ha acertado en la posicion"<< posicion1 << endl;
		}else{
			tableroJuego2[posicion1] = DISPARO_J2;
			cout << "TimeStamp["<<time(NULL) << "]: El jugador N°2 ha fallado en la posicion"<< posicion1 << endl;
		}
}
/*
 * Se lee un mensaje que llegue en un descriptor que este utilizando el servidor
 * */
int leerMensaje(int descriptor, string &mensajeRecibido){
	char buffer[500];
	memset(buffer,0,500);
	if(read(descriptor, buffer, 500) == -1){
			cout  << "TimeStamp["<<time(NULL)<<"]: Error: no se puede leer del descriptor" << endl;
			return -5;
	}
	mensajeRecibido=buffer;
	return 0;
}
/*
 * Se envia un mensaje mediante un descriptor
 * */
void enviarMensajeUnJugador(int descriptor, string mensaje){
	write(descriptor, mensaje.c_str(), 500);
}
/*
 * Se envia dos mensajes distintosr dos descriptores distintos
 *  */
void enviarMensajeDosJugadores(int descriptor1,int descriptor2, string mensaje1, string mensaje2){
	write(descriptor1, mensaje1.c_str(), 500);
	write(descriptor2, mensaje2.c_str(), 500);
}
/*
 *  Se envia el mismo mensaje por dos descriptores distintos
 */
void enviarMensajeDosJugadores(int descriptor1,int descriptor2, string mensaje){
	write(descriptor1, mensaje.c_str(), 500);
	write(descriptor2, mensaje.c_str(), 500);
}

/*
 * Se corta un string dado un delimitador y se guarda en un vector de string
 * 
 * */
void split(string s, char delim, vector<string> &elementos) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
		 elementos.push_back(item);    
    }
}

void inicializarTableros(int tableroJuego1[100], int tableroJuego2[100], string mensaje1, string mensaje2){

	vector<string> elementos1;
	vector<string> elementos2;

	split(mensaje1, ',', elementos1);
	split(mensaje2, ',', elementos2);

	vector<string>::iterator it;
	int i = 0;
	for( it = elementos1.begin(); it != elementos1.end(); ++it){

			tableroJuego1[i] = atoi((*it).c_str());
			i++;
	}
	i = 0;
	for( it = elementos2.begin(); it != elementos2.end(); ++it){
			tableroJuego2[i] = atoi((*it).c_str());
			i++;
	}

}


int barcoMuerto(int tableroJuego[100], int barco){
	for (int i = 0; i < 100 ; i ++){
		if( tableroJuego[i] == barco) return 1;
	}
	return 0;
}

/*
 * Se crea un string el cual representa al tablero de juego que es enviado a los clientes.
 * */
void crearStringTableroJuegoPropio(int tableroJuego[100], string &aux){
	int i;
	aux = "";
	for(i = 0 ; i<100;i++){
			if(tableroJuego[i] == DISPARO_J1) aux.append("X?");
			if(tableroJuego[i] == DISPARO_J2) aux.append("X?");
			if(tableroJuego[i] == ACIERTO_J1) aux.append("O?");
			if(tableroJuego[i] == ACIERTO_J2) aux.append("O?");
			if(tableroJuego[i] == BOTE_PATRULLA) aux.append("P?");
			if(tableroJuego[i] == SUBMARINO) aux.append("S?");
			if(tableroJuego[i] == BARCO_BATALLA) aux.append("B?");
			if(tableroJuego[i] == DESTRUCTOR) aux.append("D?");
			if(tableroJuego[i] == PORTA_AVIONES) aux.append("A?");
			if(tableroJuego[i] != ACIERTO_J1 && tableroJuego[i] != ACIERTO_J2 &&  tableroJuego[i] != DISPARO_J1 && tableroJuego[i] != DISPARO_J2 && tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES)  aux.append(convertirIntToString(tableroJuego[i]).append("?"));

			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) 
				aux.append("\n");
	}
	aux.append("\n");
}


void crearStringTableroJuegoDelOtro(int tableroJuego[100], string &aux){
	int i;
	aux = "";
	for(i = 0 ; i<100;i++){
			if(tableroJuego[i] == DISPARO_J1) aux.append("X?");
			if(tableroJuego[i] == DISPARO_J2) aux.append("X?");
			if(tableroJuego[i] == ACIERTO_J1) aux.append("O?");
			if(tableroJuego[i] == ACIERTO_J2) aux.append("O?");
			if(tableroJuego[i] == BOTE_PATRULLA) aux.append(convertirIntToString(i+1)).append("?");
			if(tableroJuego[i] == SUBMARINO) aux.append(convertirIntToString(i+1)).append("?");
			if(tableroJuego[i] == BARCO_BATALLA) aux.append(convertirIntToString(i+1)).append("?");
			if(tableroJuego[i] == DESTRUCTOR) aux.append(convertirIntToString(i+1)).append("?");
			if(tableroJuego[i] == PORTA_AVIONES) aux.append(convertirIntToString(i+1)).append("?");
			if(tableroJuego[i] != ACIERTO_J1 && tableroJuego[i] != ACIERTO_J2 &&  tableroJuego[i] != DISPARO_J1 && tableroJuego[i] != DISPARO_J2 && tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES)  aux.append(convertirIntToString(tableroJuego[i]).append("?"));

			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) 
				aux.append("\n");
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
void mostrarTablero(int tableroJuego[100]){

		unsigned int i;
		cout << "   Tablero de Juego" << endl;
		cout << "----------------------------------------------------------------" << endl;
		for(i=0;i<100;i++){

			if (i < 9){
				if(tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES) cout << "|  " << tableroJuego[i] << "  ";
			}else if (i == 99){
				cout << "| " << tableroJuego[i] << " |";
			}


			else { 
				if(tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES) cout << "| " << tableroJuego[i] << "  ";
			}
			if(tableroJuego[i] == BOTE_PATRULLA) cout << "|  P  ";
			if(tableroJuego[i] == SUBMARINO) cout << "|  S  ";
			if(tableroJuego[i] == BARCO_BATALLA) cout << "|  B  ";
			if(tableroJuego[i] == DESTRUCTOR) cout << "|  D  ";
			if(tableroJuego[i] == PORTA_AVIONES) cout << "|  A  ";
			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) {
				cout << "|"<<endl;
				cout << "----------------------------------------------------------------" << endl;				
			}
		}
		cout << endl <<"----------------------------------------------------------------" << endl;
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
	direccion.sin_addr.s_addr =INADDR_ANY;//DIECCION DEL CLIENTE,,,,SE 
	 

	//AHORA SE DICE AL SISTEMA QUE COMIENCE A ATENDEER LAS LLAMADAS QUE LLEGUEN, CON EL DESCRIPTOR DEL SOCKET Y EL NUMERO MAXIMO DE CLIEATIENDE A CUALQUIERA

	if ( bind(descriptor, (struct sockaddr *)&direccion, sizeof (direccion)) == -1)
	{
		cout  << "TimeStamp["<<time(NULL)<<"]: Error 2" << endl;
		return -2;
	}
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

