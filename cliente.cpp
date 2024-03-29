/**
 *		Segunda Parte - Trabalho de Redes
 *			Implementação do Protocolo
 *			  	Código do Cliente
 *			Vitor Avian nUSP: 10295392
 **/

/**
 *		Descrição:
 *			Código que simula a comunicação do Cliente com o
 *		Gerenciador, ao se conectar, o cliente informa que operação
 *		ele quer que o gerenciador retorne. Nessa implementação a unica
 *		informação que ele precisa são os ultimos dados da leitura
 *		que o cliente pode pedir pela mensagem "analise".
 **/

#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// Port do servidor
#define PORT 5050

// Tamanho do buffer
#define TAM_BUFFER 1024

// Endereço do servidor
#define SERVER_ADDR "127.0.0.1"

// Definir o id do cliente
#define ID "1000"

struct Dados{
	char temperatura[3];
	char umidade[3];
	char co2[3];
	char maxTemp[3];
	char minTemp[3];
	char maxUmi[3];
	char minUmi[3];
	char maxCO[3];
	char minCO[3];
};

// Execução do protocolo
int main(int argc, char *argv[]) {
	
	// Struct do socket do servidor
	struct sockaddr_in servidor;
	
	// Struct para receber os dados do servidor
	struct Dados d;
	
	// Descrição do file do socket
	int sockfd;
	
	int tam = sizeof(servidor);
	int stam;
	
	// Buffer de receber
	char buffer_in[TAM_BUFFER];
	// Buffer de enviar
	char buffer_out[TAM_BUFFER];
	
	cout<<"Ligando cliente..."<<endl;
	
	// Criar um socket para o cliente
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erro na criação do socket do cliente:");
		return EXIT_FAILURE;
	}
	cout<<"Socket do cliente criado com fd: "<<sockfd<<endl;
	
	// Definindo as propriedades da conexão
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(PORT);
	servidor.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	memset(servidor.sin_zero, 0x0, 8);
	
	
	// Tentativa de conexão com o servidor
	if (connect(sockfd, (struct sockaddr*) &servidor, tam) == -1) {
		perror("Não foi possível se conectar com o servidor.");
		return EXIT_FAILURE;
	}
	
	// Recebe a primeira mensagem do servidor
	if ((stam = recv(sockfd, buffer_in, TAM_BUFFER, 0)) > 0) {
		buffer_in[stam + 1] = '\0';
		cout<<"Servidor diz: "<<buffer_in<<endl;
	}
	
	// Header contendo o ID do cliente
	char header[] = ID;
	// Mandar o header para o servidor identificar quem está se conectando
	send(sockfd, header, strlen(header), 0);
	
	// Começa a troca de mensagens com o servidor até a mensagem de parar
	while (true) {
		
		// Limpando os buffer antes de receber e mandar mensagens
		memset(buffer_in, 0x0, TAM_BUFFER);
		memset(buffer_out, 0x0, TAM_BUFFER);
		
		// Digitar a mensagem que deseja mandar, "analise"
		cout<<"Mande a mensagen desejada: ";
		fgets(buffer_out, TAM_BUFFER, stdin);
		
		// Manda a mensagem para o servidor pelo socket
		send(sockfd, buffer_out, strlen(buffer_out), 0);
		
		// Recebe a mensagem de resposta do servidor
		stam = recv(sockfd, buffer_in, TAM_BUFFER, 0);
		cout<<"Resposta do servidor: "<<buffer_in<<endl;
		if(strcmp(buffer_out, "analise\n") == 0){ // Receber a resposta do gerenciador e mostrar para o cliente
			memcpy(d.temperatura, buffer_in, 2);
			d.temperatura[2] = '\0';
			memcpy(d.umidade, buffer_in+2, 2);
			d.umidade[2] = '\0';
			memcpy(d.co2, buffer_in+4, 2);
			d.co2[2] = '\0';
			cout<<"Última leitura dos sensores:"<<endl;
			cout<<"Temperatura: "<<d.temperatura<<"ºC"<<endl;
			cout<<"Umidade: "<<d.umidade<<"g/m"<<endl;
			cout<<"CO2: "<<d.co2<<"%"<<endl;
		}
		// Ao receber uma resposta "tchau", finalizar a conexão
		if(strcmp(buffer_in, "Tchau\n") == 0)
			break;
	}
	
	// Fechar a conexão com o servidor
	close(sockfd);
	cout<<"Conexão finalizada."<<endl;
	
	return 0;
}
