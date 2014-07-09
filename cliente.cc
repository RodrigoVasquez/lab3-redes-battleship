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
void preguntarContinuaElJuego(string &);
void mostrarMensajeTriunfo(int , int , int );
void mostrarTableroJuegoPropio(string tableroJuego);
void mostrarTableroJuegoDelOtro(string tableroJuego);
string posicionarBarcos();
string colocarBarcos();
bool verificarRango(int);
bool verificarPosicion(int, int[100],int,int);
int puerto= 5009;

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
	string mensajeLeido, mensajeEnviar, tableroPropio, tableroDelOtro;
	
	//Se lee que numero de jugador es asignado
	cout << "Esperando la conexion de otro jugador..." << endl;
	leerMensaje(descriptor, mensajeLeido);
	
	cout << "Usted es el jugador N°"<< mensajeLeido << endl << endl;
	// Se obtiene el numero del jugador que el servidor ha asignado
	int numeroJugador = atoi(mensajeLeido.c_str());

	mensajeEnviar = posicionarBarcos();
	cout << "Esperando que el otro jugador posicione sus barcos...";

	enviarMensaje(descriptor, mensajeEnviar);

	// variables para organizar el juego
	bool jugando = true;
	int ganador;
	//ciclo de juego
	while(jugando){
		leerMensaje(descriptor, mensajeLeido);
		tableroPropio = mensajeLeido;
		mostrarTableroJuegoPropio(tableroPropio);

		leerMensaje(descriptor, mensajeLeido);
		tableroDelOtro = mensajeLeido;
		mostrarTableroJuegoDelOtro(tableroDelOtro);

		//Se lee el tablero en juego
		solicitarPosicion(tableroDelOtro, mensajeEnviar, numeroJugador);
		cout << "Esperando que otro jugador realice su jugada" << endl;
		enviarMensaje(descriptor, mensajeEnviar);

		// leer si alguien ha ganado
		leerMensaje(descriptor, mensajeLeido);
		ganador = atoi(mensajeLeido.c_str());

		if(ganador == 0){

			cout << "La partida termino en un empate" << endl;
			preguntarContinuaElJuego(mensajeEnviar);
			enviarMensaje(descriptor, mensajeEnviar);
			leerMensaje(descriptor,mensajeLeido);
			if(atoi(mensajeLeido.c_str()) == 0 ){
				jugando = false;
			}
			else{

				mensajeEnviar = posicionarBarcos();
				enviarMensaje(descriptor, mensajeEnviar);
			}

		}else if(ganador == 1){

			if (ganador == numeroJugador){
				cout << "Eres el Ganador de la partida!!! " << endl;
			}else{
				cout << "Eres el perdedor de la partida :c" << endl;
			}
			preguntarContinuaElJuego(mensajeEnviar);
			enviarMensaje(descriptor, mensajeEnviar);
			leerMensaje(descriptor,mensajeLeido);
			if(atoi(mensajeLeido.c_str()) == 0 ){
				jugando = false;
			}
			else{

				mensajeEnviar = posicionarBarcos();
				enviarMensaje(descriptor, mensajeEnviar);
			}


		}else if(ganador == 2){
			if (ganador == numeroJugador){
				cout << "Eres el ganador de la partida!!!" << endl;
			}else{
				cout << "Eres el perdedor de la partida :c" << endl;
			}
			preguntarContinuaElJuego(mensajeEnviar);
			enviarMensaje(descriptor, mensajeEnviar);
			leerMensaje(descriptor,mensajeLeido);
			if(atoi(mensajeLeido.c_str()) == 0 ){
				jugando = false;
			}
			else{

				mensajeEnviar = posicionarBarcos();
				enviarMensaje(descriptor, mensajeEnviar);
			}
		}

	}
	// Se cierra el descriptor de comunicación con el servidor
	
	close(descriptor); 

}

void imprimirPosicionamientoBarco(int tableroJuego[100]){
		unsigned int i;
		cout << "   Tablero de Juego" << endl;
		cout << "----------------------------------------------------------------" << endl;
		for(i=0;i<100;i++){

			if (i < 9){
				if(tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES) cout << "| " << tableroJuego[i] << "  ";
			}else if (i == 99){
				cout << "| " << tableroJuego[i] << "|";
			}


			else { 
				if(tableroJuego[i] != BOTE_PATRULLA && tableroJuego[i] != SUBMARINO && tableroJuego[i] != BARCO_BATALLA && tableroJuego[i] != DESTRUCTOR && tableroJuego[i] != PORTA_AVIONES) cout << "| " << tableroJuego[i] << " ";
			}
			if(tableroJuego[i] == BOTE_PATRULLA) cout << "| P  ";
			if(tableroJuego[i] == SUBMARINO) cout << "| S  ";
			if(tableroJuego[i] == BARCO_BATALLA) cout << "| B  ";
			if(tableroJuego[i] == DESTRUCTOR) cout << "| D  ";
			if(tableroJuego[i] == PORTA_AVIONES) cout << "| A  ";
			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) {
				cout << "|"<<endl;
				cout << "----------------------------------------------------------------" << endl;				
			}
		}
		cout << endl <<"----------------------------------------------------------------" << endl;
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
			if (tableroJuego[posicion + j] < 0) 
				
				return false; 
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

string posicionarBarcos(){

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
	stringstream ss;
	for(int i = 0; i < 100 ; i++){
		if (i==99)
			ss << tablero[i];
		else 
			ss << tablero[i] << ",";
	}

	return ss.str();
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
 * Se lee un mensaje del descriptor y se guarda en un string
 * */
int leerMensaje(int descriptor, string &mensajeRecibido){
	char buffer[500];
	if(read(descriptor, buffer, 500) == -1){
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
	write(descriptorCliente, mensaje.c_str(), 500);
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
			cout << "Jugador N°"<< numeroJugador<<" Ingrese posicion donde desea lanzar[1-100]: ";
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

void mostrarTableroJuegoPropio(string tableroJuego){
	vector<string> elementos;
	tableroJuego.erase(remove(tableroJuego.begin(), tableroJuego.end(), '\n'), tableroJuego.end());
	split(tableroJuego, '?', elementos);
	unsigned int i;
	cout << "   Tablero de Juego" << endl;
	cout << "---------------------------------------------------" << endl;
	for(i=0;i<elementos.size();i++){ 
			if(i<9)
				cout <<"| " <<elementos[i] <<"  ";
			else
			{
				if(i<99 )
					if ( elementos[i]=="O" || elementos[i]=="X" || elementos[i]=="P" || elementos[i]=="S" || elementos[i]=="B" || elementos[i]=="D" || elementos[i]=="A")
						cout <<"| " <<elementos[i] <<"  "; 
					else
						cout <<"| " <<elementos[i] <<" ";
				else
					if ( elementos[i]=="O" || elementos[i]=="X" || elementos[i]=="P" || elementos[i]=="S" || elementos[i]=="B" || elementos[i]=="D" || elementos[i]=="A")
						cout <<"| " <<elementos[i] <<"  |" << endl;
					else
						cout <<"| " <<elementos[i] <<"|" << endl;
			}
			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) {
				cout << "|"<<endl;
		cout << "---------------------------------------------------" << endl;
			}
		}
	cout << "---------------------------------------------------" << endl;
}

void mostrarTableroJuegoDelOtro(string tableroJuego){
	vector<string> elementos;
	tableroJuego.erase(remove(tableroJuego.begin(), tableroJuego.end(), '\n'), tableroJuego.end());
	split(tableroJuego, '?', elementos);
	unsigned int i;
	cout << "   Tablero de Juego" << endl;
	cout << "---------------------------------------------------" << endl;
	for(i=0;i<elementos.size();i++){ 
			if(i<9){
				cout <<"| " <<elementos[i] <<"  ";
			}else
			{
				if(i<99 )
					if (elementos[i]=="O" || elementos[i]=="X")
						cout <<"| " <<elementos[i] <<"  ";
					else
						cout <<"| " <<elementos[i] <<" ";
				else{
					if (elementos[i]=="O" || elementos[i]=="X")
						cout <<"| " <<elementos[i] <<"  |" << endl;
					else
						cout <<"| " <<elementos[i] <<"|" << endl;
					}
			}
			if(i==9 || i==19 ||i==29 ||i==39 || i==49 ||i==59 ||i==69 ||i==79 ||i==89) {
				cout << "|"<<endl;
				cout << "---------------------------------------------------" << endl;
			}
		}
		cout << "---------------------------------------------------" << endl;
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
