/**
 *		Segunda Parte - Trabalho de Redes
 *		   Implementação do Protocolo
 *	  	   Código do Sensor de Umidade
 *			Vitor Avian nUSP: 10295392
 **/

/**
 *		Descrição:
 *			Código que simula a comunicação do sensor de Umidade com o
 *		Gerenciador, ao se conectar, o sensor informa ao gerenciador
 *		a ultima leitura da umidade.
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
#define PORT 4242

// Tamanho do buffer
#define TAM_BUFFER 1024

// Endereço do servidor
#define SERVER_ADDR "127.0.0.1"

// Definir o id do sensor
#define ID "0001"

char umidade[3];

// Função para simular uma leitura de umidade randomica
void ultimaLeitura(){
	int temp1, temp2;
	temp1 = rand() % 5;
	temp2 = rand() % 10;
	umidade[0] = temp1 + '0';
	umidade[1] = temp2 + '0';
	umidade[2] = '\n';
}

// Execução do protocolo
int main(int argc, char *argv[]) {
	
	// Struct do socket do servidor
	struct sockaddr_in servidor;
	
	// Descrição do file do socket
	int sockfd;
	
	int tam = sizeof(servidor);
	int stam;
	
	// Buffer de receber
	char buffer_in[TAM_BUFFER];
	// Buffer de enviar
	char buffer_out[TAM_BUFFER];
	
	cout<<"Ligando sensor..."<<endl;
	
	// Criar um socket para o sensor
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erro na criação do socket do sensor:");
		return EXIT_FAILURE;
	}
	cout<<"Socket do sensor criado com fd: "<<sockfd<<endl;
	
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
	
	// Header contendo o ID do sensor
	char header[] = ID;
	// Mandar o header para o servidor identificar quem está se conectando
	send(sockfd, header, strlen(header), 0);
	
	char responder = '1';
	// Começa a troca de mensagens com o servidor até a mensagem de parar
	while (true) {
		
		// Limpando os buffer antes de receber e mandar mensagens
		memset(buffer_in, 0x0, TAM_BUFFER);
		memset(buffer_out, 0x0, TAM_BUFFER);
		
		// Mandar a última leitura até o gerenciador não querer mais
		if(responder == '1'){
			cout<<"Lendo a umidade... "<<endl;
			ultimaLeitura();
			strcpy(buffer_out, umidade);
			cout<<umidade<<endl;
			
			// Manda a mensagem para o servidor pelo socket
			send(sockfd, buffer_out, strlen(buffer_out), 0);
			responder = '0';
		}
		
		// Recebe a mensagem de resposta do servidor
		stam = recv(sockfd, buffer_in, TAM_BUFFER, 0);
		cout<<"Resposta do servidor: "<<buffer_in<<endl;
		// Ao receber uma resposta "tchau", finalizar a conexão
		if(strcmp(buffer_in, "Tchau\n") == 0)
			break;
		else if(strcmp(buffer_in, "Sim\n") == 0){
			responder = '1';
		}
	}
	
	// Fechar a conexão com o servidor
	close(sockfd);
	cout<<"Conexão finalizada."<<endl;
	
	return 0;
}
