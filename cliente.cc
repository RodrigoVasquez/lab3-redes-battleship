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
#include <cstdlib>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;
int iniciar_conexion(int &);
int leerMensaje(int , string &);
void enviarMensaje(int , string );
bool validarPosicion(string , string );
void solicitarPosicion(string ,string &, int);
void split(string , char , vector<string> &);
void mostrarTableroJuego(string );
bool hayGanador(string );
void preguntarContinuaElJuego(string &);
void mostrarMarcador(string ,int , int &, int &, int &);
bool continuaElJuego(string ,int );
void mostrarMensajeTriunfo(int , int , int );
bool verificarRango(int);
bool verificarPosicion(int, int[100],int,int);
string posicionarBarcos();
string colocarBarcos();
int puerto= 5009;


#define VACIO 0
#define BOTE_PATRULLA -1
#define DESTRUCTOR -2
#define SUBMARINO -3
#define BARCO_BATALLA -4
#define PORTA_AVIONES -5 

int main(int argc,char **argv)
{
	int descriptor=0;
	if (iniciar_conexion(descriptor) != 0)
	{
		return 0;
	}
	string mensajeLeido, mensajeEnviar, tableroJuego;
	
	//Se lee que numero de jugador es asignado
	cout << "Mensaje: Esperando la conexion de otro jugador..." << endl;
	leerMensaje(descriptor, mensajeLeido);
	
	cout << "Mensaje: Usted es el jugador N°"<< mensajeLeido << endl << endl;
	// Se obtiene el numero del jugador que el servidor ha asignado
	int numeroJugador = atoi(mensajeLeido.c_str());
	
	
	mensajeEnviar = posicionarBarcos();
	//enviarMensaje(descriptor);
/*

	// variables para organizar el juego
	bool jugando = true;
	int ganador, contGlobos1 = 0, contGlobos2 = 0;
	//ciclo de juego
	while(jugando){
		//Se lee el tablero en juego
		leerMensaje(descriptor, mensajeLeido);
		
		tableroJuego = mensajeLeido;
		//se muestra el tablero de juego
		mostrarTableroJuego(mensajeLeido);
		
		//enviar posicion de jugada
		solicitarPosicion(tableroJuego,mensajeEnviar, numeroJugador);
		cout << "Mensaje: Esperando que otro jugador realice su jugada" << endl;
		enviarMensaje(descriptor, mensajeEnviar);
		
		//Leer mensaje ganador
		leerMensaje(descriptor,mensajeLeido);
		// se muestra el marcador de juego
		mostrarMarcador(mensajeLeido,numeroJugador,contGlobos1, contGlobos2, ganador);
		// Si hay un ganador se muestra los mensajes de triungo y se pregunta si se quiere seguir jugando
		if(ganador==1){
			// Se muestra el mensaje de triunfo
			mostrarMensajeTriunfo(numeroJugador, contGlobos1, contGlobos2);
			// se pregunta si se quiere continuar el juego
			preguntarContinuaElJuego(mensajeEnviar);
			
			// Enviar mensaje por si continua el juego
			enviarMensaje(descriptor, mensajeEnviar);
			
			cout << "Mensaje: Esperando que otro jugador envie su respuesta" << endl;
			// Se lee un mensaje del jugador 
			leerMensaje(descriptor, mensajeLeido);
			//Se actualiza el valor de lavariable jugando dependiendo si ambos jugadores desea seguir jugando
			jugando = continuaElJuego(mensajeLeido, numeroJugador);
			// Se reinicia los cont de globos
			contGlobos1 = 0;
			contGlobos2 = 0;
		}
	}
	cout << "Mensaje: El juego ha terminado" << endl;
	// Se cierra el descriptor de comunicación con el servidor
	*/
	close(descriptor); 

}

void imprimirPosicionamientoBarco(int tableroJuego[100]){
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

string posicionarBarcos(){
	stringstream ss;
	int tablero[100];

	for (int i = 0; i < 100 ; i++) tablero[i] = i+1;

	int pos = 0;
	int orientacion = 0;
	bool posicionCorrecta = false;
	bool rangoCorrecto = false;
	imprimirPosicionamientoBarco(tablero);

	do{
		cout << "Ingrese la posicion inicial del barco del PORTA_AVIONES:" << endl;
		cin >> pos;
		rangoCorrecto = verificarRango(pos-1);
		cout << "¿Horizontal o vertical?" << endl << "1.-Vertical" << endl << "2.-Horizontal" << endl << "Ingrese una opcion: ";
		cin >> orientacion;
		posicionCorrecta = verificarPosicion(orientacion, tablero,pos-1,5);
	}while(rangoCorrecto == false || posicionCorrecta == false);
	if (orientacion == 1){
		pos = pos - 1;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 10;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 10;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 10;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 10;
		tablero[pos] = PORTA_AVIONES;
	}else{
		pos = pos - 1;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 1;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 1;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 1;
		tablero[pos] = PORTA_AVIONES;
		pos = pos + 1;
		tablero[pos] = PORTA_AVIONES;
	}

	imprimirPosicionamientoBarco(tablero);

	do{
		cout << "Ingrese la posicion inicial del barco del BOTE_PATRULLA:" << endl;
		cin >> pos;
		rangoCorrecto = verificarRango(pos-1);
		cout << "¿Horizontal o vertical?" << endl << "1.-Vertical" << endl << "2.-Horizontal" << endl << "Ingrese una opcion: ";
		cin >> orientacion;
	posicionCorrecta = verificarPosicion(orientacion, tablero,pos-1,2);
	}while(rangoCorrecto == false || posicionCorrecta == false);
	if (orientacion == 1){
		pos = pos - 1;

		tablero[pos] = BOTE_PATRULLA;
		pos = pos + 10;
		tablero[pos] = BOTE_PATRULLA;
	}else{
		pos = pos - 1;

		tablero[pos] = BOTE_PATRULLA;
		pos = pos + 1;
		tablero[pos] = BOTE_PATRULLA;
	}
		imprimirPosicionamientoBarco(tablero);

	do{
		cout << "Ingrese la posicion inicial del barco del DESTRUCTOR:" << endl;
		cin >> pos;
		rangoCorrecto = verificarRango(pos-1);
		cout << "¿Horizontal o vertical?" << endl << "1.-Vertical" << endl << "2.-Horizontal" << endl << "Ingrese una opcion: ";
		cin >> orientacion;
		posicionCorrecta = verificarPosicion(orientacion, tablero,pos-1,3);
	}while(rangoCorrecto == false || posicionCorrecta == false);
	if (orientacion == 1){
		pos = pos - 1;		
		tablero[pos] = DESTRUCTOR;
		pos = pos + 10;
		tablero[pos] = DESTRUCTOR;
		pos = pos + 10;
		tablero[pos] = DESTRUCTOR;
	}else{
		pos = pos - 1;
		tablero[pos] = DESTRUCTOR;
		pos = pos + 1;
		tablero[pos] = DESTRUCTOR;
		pos = pos + 1;
		tablero[pos] = DESTRUCTOR;
	}
		imprimirPosicionamientoBarco(tablero);

	do{
		cout << "Ingrese la posicion inicial del barco del SUBMARINO:" << endl;
		cin >> pos;
		rangoCorrecto = verificarRango(pos-1);
		cout << "¿Horizontal o vertical?" << endl << "1.-Vertical" << endl << "2.-Horizontal" << endl << "Ingrese una opcion: ";
		cin >> orientacion;
		posicionCorrecta = verificarPosicion(orientacion, tablero,pos-1,3);
	}while(rangoCorrecto == false || posicionCorrecta == false);
	if (orientacion == 1){
		pos = pos - 1;		
		tablero[pos] = SUBMARINO;
		pos = pos + 10;
		tablero[pos] = SUBMARINO;
		pos = pos + 10;
		tablero[pos] = SUBMARINO;
	}else{
		pos = pos - 1;		
		tablero[pos] = SUBMARINO;
		pos = pos + 1;
		tablero[pos] = SUBMARINO;
		pos = pos + 1;
		tablero[pos] = SUBMARINO;
	}
		imprimirPosicionamientoBarco(tablero);
	do{
		cout << "Ingrese la posicion inicial del barco del BARCO_BATALLA:" << endl;
		cin >> pos;
		rangoCorrecto = verificarRango(pos-1);
		cout << "¿Horizontal o vertical?" << endl << "1.-Vertical" << endl << "2.-Horizontal" << endl << "Ingrese una opcion: ";
		cin >> orientacion;
		posicionCorrecta = verificarPosicion(orientacion, tablero,pos-1,4);
	}while(rangoCorrecto == false || posicionCorrecta == false);
	if (orientacion == 1){
		pos = pos - 1;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 10;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 10;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 10;
		tablero[pos] = BARCO_BATALLA;
	}else{
		pos = pos - 1;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 1;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 1;
		tablero[pos] = BARCO_BATALLA;
		pos = pos + 1;
		tablero[pos] = BARCO_BATALLA;
	}
	return "";
}

/*
 * Se encarga de determinar si el juego debe continuar dado un mensaje que contiene las respuestas de ambos jugadores y
 * muestra mensajes dependiendo de cada caso.
 * */
bool verificarRango(int posicion){
	if(posicion < 0 || posicion > 100){
		cout << "Ingrese una posicion correcta" << endl;
		return false;
	}
	 return true;
}

bool verificarPosicion(int orientacion, int tableroJuego[100], int posicion, int n){
	//vertical
	if(orientacion == 1){
		int j = 0;
		for (int i = 0; i < n; i++ ){
			if (tableroJuego[posicion + j] < 0) return false; 
			j += 10;
		}
		return true;
	}
	if(orientacion == 2){
		int j = 0;
		for (int i = 0; i < n; i++ ){
			if (tableroJuego[posicion + j] < 0) return false; 
			j += 1;
		}
		return true;
	} 
	return false;
}



bool continuaElJuego(string mensaje,int numeroJugador){
	vector<string> elementos;
	mensaje.erase(remove(mensaje.begin(), mensaje.end(), '\n'), mensaje.end());
	split(mensaje,'?', elementos);
	
	int respuesta1 = atoi(elementos[0].c_str());
	int respuesta2 = atoi(elementos[1].c_str());
	
	if(numeroJugador==1){
		if(respuesta1 == 1 && respuesta2 == 2){
			cout << "Mensaje: El jugador N°2 no desea seguir jugando :(" << endl;
			return false;
			
		}else if(respuesta1 == 2 && respuesta2 == 2){
			cout << "Mensaje: Ninguno quiere volver a jugar:(" << endl;
			return false;
			
		}else if(respuesta1 == 2 && respuesta2 ==1){
			cout << "Mensaje: El jugador N°2 queria seguir jugando:(" << endl;
			return false;
		
		}else{
			cout << "Mensaje: Comienza una nueva partida" << endl;
			return true;
			
		}
	}else{
		if(respuesta1 == 1 && respuesta2 == 2){
			
			cout << "Mensaje: El jugador N°1 queria seguir jugando:(" << endl;
			return false;
			
		}else if(respuesta1 == 2 && respuesta2 == 2){
			cout << "Mensaje: Ninguno quiere volver a jugar:(" << endl;
			return false;
			
		}else if(respuesta1 == 2 && respuesta2 ==1){
			cout << "Mensaje: El jugador N°1 no desea seguir jugando :(" << endl;
			return false;
		
		}else{
			return true;
			
		}
	}		
}

/*
 * Se muestra un mensaje de triunfo dependiento del numero de jugador y los cont de globos de cada jugador.
 * */
void mostrarMensajeTriunfo(int numeroJugador, int contGlobos1, int contGlobos2){
	if(numeroJugador == 1){
		if(contGlobos1 == contGlobos2){
			cout << "Mensaje: Es un empate :)!!!!" << endl;
		}
		else if (contGlobos1 > contGlobos2)
			cout << "Mensaje: Haz ganado :)!!!!" << endl;
		else
			cout << "Mensaje: Haz perdido :(" <<endl;
	}else{
		if(contGlobos1 == contGlobos2){
			cout << "Mensaje: Es un empate :)!!!!" << endl;
		}
		else if (contGlobos1 > contGlobos2)
			cout << "Mensaje: Haz perdido :)!!!!" << endl;
		else
			cout << "Mensaje: Haz ganado :(" << endl;

	}
}
/*
 * Se encarga de obtener los mensajes de cada jugador respecto a si quieren que el juego continue
 * */
void preguntarContinuaElJuego(string &opcion){
	do{
		cout << "¿Desea seguir jugando?" << endl << "1.-Si" << endl << "2.-No" << endl << "Ingrese una opcion: ";
		cin >> opcion;
		if(opcion=="1" || opcion=="2") 
			break;
		else
			cout << "Mensaje: Opción Invalida" << endl;
	}while(1);
}

/*
 * Se encarga de mostrar el marcador de juego dado el mensaje que contiene informacion recibida del servidor
 * */
void mostrarMarcador(string marcador,int numeroJugador, int &contGlobos1, int &contGlobos2, int &ganador){
	vector<string> elementos;
	marcador.erase(remove(marcador.begin(), marcador.end(), '\n'), marcador.end());
	split(marcador,'?', elementos);
	
	int contGlobos1Ant = contGlobos1;
	int contGlobos2Ant = contGlobos2;
	contGlobos1 = atoi(elementos[1].c_str());
	contGlobos2 = atoi(elementos[2].c_str());
	
	if(numeroJugador == 1){
		if (contGlobos1 > contGlobos1Ant)
			cout << "Mensaje: Haz acertado a un globo :)!!!!" << endl;
		else
			cout << "Mensaje: Haz Fallado :(" << endl;
		if(contGlobos2 > contGlobos2Ant)
			cout << "Mensaje: El jugador N°2 ha reventado un globo"<< endl;
		else 
			cout << "Mensaje: El jugador N°2 ha fallado"<< endl;
	}else{
		if (contGlobos2 > contGlobos2Ant)
			cout << "Mensaje: Haz acertado a un globo :)!!!!"<< endl;
		else
			cout << "Mensaje: Haz Fallado :("<< endl;
		if(contGlobos1 > contGlobos1Ant)
			cout << "Mensaje: El jugador N°1 ha reventado un globo"<< endl;
		else 
			cout << "Mensaje: El jugador N°1 ha fallado"<< endl;
	}
	cout << endl;
	cout << "::::::::::::::::::::::::::" << endl;
	cout << ":        MARCADOR        :" << endl;
	cout << ":      Jugador N°1 =" << contGlobos1 << "    :" << endl;
	cout << ":      Jugador N°2 =" << contGlobos2 << "    :" << endl;
	cout << "::::::::::::::::::::::::::" << endl;
	
	ganador =  atoi(elementos[0].c_str());
	
}
/*
 * Se encarga de determinar si hay un ganador
 * */
bool hayGanador(int ganador){
	if(ganador == 0) 
		return false;
	return true;
}
/*
 * Se lee un mensaje del descriptor y se guarda en un string
 * */
int leerMensaje(int descriptor, string &mensajeRecibido){
	char buffer[100];
	if(read(descriptor, buffer, 100) == -1){
			cout << "Error 5: No se puede leer del descriptor" << endl;
			return -5;
	}
	mensajeRecibido=buffer;
	return 0;
}
/*
 * Se envia un mensaje por el descriptor
 * 
 **/
void enviarMensaje(int descriptorCliente, string mensaje){
	write(descriptorCliente, mensaje.c_str(), 100);
}
/*
 * Se solicita la ip del servidor
 * */
void solicitarIp(string &ip){
		cout << "Ingrese direccion IP del servidor: ";
		cin >> ip;
}
/*
 * Se solicita la posicion al jugador se valida que esta este entre el rango permitido y que no este ocupada
 * */
void solicitarPosicion(string tableroJuego,string &posicion, int numeroJugador){
		do{
			cout << "Jugador N°"<< numeroJugador<<" Ingrese posicion donde desea lanzar[1-25]: ";
			cin >> posicion;
			if(validarPosicion(tableroJuego, posicion) == true) break;
		}while(true);
}
/*
 * Se valida la posicion ingresada por el usuario
 * */
bool validarPosicion(string tableroJuego, string posicion){
	
	vector<string> elementos;
	tableroJuego.erase(remove(tableroJuego.begin(), tableroJuego.end(), '\n'), tableroJuego.end());
	split(tableroJuego,'?', elementos);
	int i = 0;
	for(i=0;i<(int)elementos.size();i++){	
		
		if(elementos[i]==posicion) return true;
	}
	cout << "Mensaje: Posicion ocupada o invalida"<<endl;
	return false;
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
/*
 * Se muestra el tablero de juego y su estado dado uyn mensaje recibido desde el servidor con un formato determinado
 * */
void mostrarTableroJuego(string tableroJuego){
	vector<string> elementos;
	tableroJuego.erase(remove(tableroJuego.begin(), tableroJuego.end(), '\n'), tableroJuego.end());
	split(tableroJuego, '?', elementos);
	unsigned int i;
	cout << "    Tablero de Juego" << endl;
	cout << "--------------------------" << endl;
	for(i=0;i<elementos.size();i++){
			if(i<9|| elementos[i]=="Y" || elementos[i]=="X" || elementos[i]=="O" || elementos[i]=="-")
				cout <<"| " <<elementos[i] <<"  ";
			else
				cout <<"| " <<elementos[i] <<" ";
			if(i==24) cout << "|";
			if(i==4 || i==9 ||i==14 ||i==19) {
				cout << "|"<<endl;
				cout << "--------------------------" << endl;
				
			}
	}
	cout << endl <<"--------------------------" << endl;
	cout << endl;
}



/*
 * Inicialización de la conexión con el servidor
 * */
int iniciar_conexion(int &descriptor){
	struct sockaddr_in direccion;
	//SE CONSULTA POR DIRECCION IP DEL SERVIDOR
	string ip;
	//solicitarIp(ip);
	ip = "127.0.0.1";
	//*********** SE DEFINE LA ESTRUCTURA DEL CLIENTE Y SE CONECTA AL SERVIDOR ************************
	
	descriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (descriptor == -1)
	{
    			cout << "Error 1: No se puede crear el socket" << endl;
				return -1;
	}
	
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = inet_addr(ip.c_str());//((structin_addr*)(Host->h_addr))->s_addr;
	direccion.sin_port = htons(puerto);

	if (connect(descriptor, (struct sockaddr *)&direccion, sizeof(direccion) ) == -1)
	{
    		cout << "Error 2: No se puede conectar" << endl;
				return -2;
	}
	return 0;
}
