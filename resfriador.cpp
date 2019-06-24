/**
 *		Segunda Parte - Trabalho de Redes
 *			Implementação do Protocolo
 *			Código do Atuador Resfriador
 *			Vitor Avian nUSP: 10295392
 **/

/**
 *		Descrição:
 *			Código que simula a comunicação do atuador Resfriador com o
 *		Gerenciador, ele se conecta ao gerenciador e espera uma mensagem
 *		dizendo se ele deve ser desligado ou ligado caso precise.
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

// Definir o id do atuador
#define ID "0100"

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
	
	cout<<"Ligando cliente..."<<endl;
	
	// Criar um socket para o atuador
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
	
	// Header contendo o ID do atuador
	char header[] = ID;
	// Mandar o header para o servidor identificar quem está se conectando
	send(sockfd, header, strlen(header), 0);
	
	int funcao = 2;
	// Começa a troca de mensagens com o servidor até a mensagem de parar
	while (true) {
		
		// Limpando os buffer antes de receber e mandar mensagens
		memset(buffer_in, 0x0, TAM_BUFFER);
		
		// Recebe a mensagem de resposta do servidor caso deve ligar ou desligar o resfriador
		stam = recv(sockfd, buffer_in, TAM_BUFFER, 0);
		cout<<"Resposta do servidor: "<<buffer_in<<endl;
		funcao = buffer_in[0] - '0';
		if(funcao == 1){ // Simular a operação de ligar ou desligar e informar o gerenciador
			cout<<"Ligando resfriador..."<<endl;
			send(sockfd, "Resfriador ligado.", 18, 0);
		}
		else if(funcao == 0){
			cout<<"Desligando resfriador..."<<endl;
			send(sockfd, "Resfriador desligado.", 21, 0);
		}
		else
			break;
		// Ao receber uma resposta "tchau", finalizar a conexão
		if(strcmp(buffer_in, "Tchau\n") == 0)
			break;
	}
	
	// Fechar a conexão com o servidor
	close(sockfd);
	cout<<"Conexão finalizada."<<endl;
	
	return 0;
}
