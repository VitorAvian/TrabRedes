/**
 *		Segunda Parte - Trabalho de Redes
 *			Implementação do Protocolo
 *			  Código do Gerenciador
 *			Vitor Avian nUSP: 10295392
 **/

/**
 *		Descrição:
 *			Código que simula a comunicação do Gerenciador com todas
 *		as partes.
 *		Conexão com sensores:
 *			Ao se conectar com sensores, o gerenciador vai aguardar
 *		uma mensagem sobre a última leitura do sensor e armazenar
 *		na sua estrutura de dados. Depois o gerenciador vai ter a opção
 *		de pedir uma nova leitura ou encerrar a comunicação.
 *		Conexão com atuadores:
 *			Ao se conectar com atuadores, o gerenciador verifica nos seus
 *		dados a última leitura do atributo do atuador e se ela estiver fora
 *		dos maximos e minimos, informa ao atuador se ele deve ser ligado ou desligado.
 *		Depois o gerenciador encerra a conexão.
 *		Conexão com o cliente:
 *			Ao se conectar com o cliente, o gerenciador espera para saber que
 *		mensagem o cliente vai mandar, ao ouvir "analise", o gerenciador
 *		manda para o cliente todas as últimas leituras que ele tem armazenado.
 *		Daqui o cliente pode continuar pedindo novas leituras ou encerrar a conexão
 *		com "finalizar" ou encerrar todas as conexões com "finalizar tudo".
 **/

#include <iostream>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

// Port do servidor
#define PORT 5050

// Tamanho do buffer
#define TAM_BUFFER 1024

// Struct para armazenar os dados dos sensores e os maximos e minimos
struct Dados{
	char temperatura[3];
	char umidade[3];
	char co2[3];
	int maxTemp;
	int minTemp;
	int maxUmi;
	int minUmi;
	int maxCO;
	int minCO;
};

// Função para iniciar os dados
struct Dados iniciarDados(){
	struct Dados aux;
	strcpy(aux.temperatura, "00");;
	strcpy(aux.umidade, "00");
	strcpy(aux.co2, "00");
	aux.maxTemp = 30;
	aux.minTemp = 15;
	aux.maxUmi = 30;
	aux.minUmi = 10;
	aux.maxCO = 50;
	aux.minCO = 10;
	return aux;
}

// Execução do protocolo
int main(void) {
	
	// Estruturas do cliente e do servidor
	struct sockaddr_in cliente, servidor;
	
	// Estrutura que armazenará os dados recebidos dos sensores
	struct Dados d;
	d = iniciarDados();
	
	// Arquivos de servidor e cliente
	int servidorfd, clientefd;
	
	// Criação do buffer
	char buffer_in[TAM_BUFFER];
	char buffer_out[TAM_BUFFER];
	
	cout<<"Ligando gerenciador"<<endl;
	
	// Criando um socket de IPv4
	servidorfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servidorfd == -1) {
		perror("Não foi possível criar um socket para o gerenciador:");
		return EXIT_FAILURE;
	}
	cout<<"Socket do gerenciador criado com fd: "<<servidorfd<<endl;
	
	// Definindo as propriedades do socket do gerenciador
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(PORT);
	memset(servidor.sin_zero, 0x0, 8);
	
	// Implementando o caso de o socket já estar sendo em uso
	int sim = 1;
	if(setsockopt(servidorfd, SOL_SOCKET, SO_REUSEADDR,
				  &sim, sizeof(int)) == -1) {
		perror("Erro na opção de socket:");
		return EXIT_FAILURE;
	}
	
	// Conectar o socket a um port
	if(bind(servidorfd, (struct sockaddr*)&servidor, sizeof(servidor)) == -1 ) {
		perror("Erro ao conectar o socket:");
		return EXIT_FAILURE;
	}
	
	// Iniciar as operações de espera e troca de mensagens até acabar as conexões
	while(true){
		// Começar a esperar conexões
		if(listen(servidorfd, 1) == -1) {
			perror("Erro na hora de escutar:");
			return EXIT_FAILURE;
		}
		cout<<"Escutando no port "<<PORT<<endl;
		
		// Aceitar a conexão com o cliente
		socklen_t cliente_len = sizeof(cliente);
		if ((clientefd=accept(servidorfd,
							 (struct sockaddr *) &cliente, &cliente_len )) == -1) {
			perror("Erro ao aceitar conexão:");
			return EXIT_FAILURE;
		}
		
		// Coloca no buffer uma mensagem dizendo que a conexão foi estabelecida
		strcpy(buffer_in, "Conexão estabelecida!\n\0");
		
		char header[5];
		// Manda a mensagem do buffer para o cliente
		if (send(clientefd, buffer_in, strlen(buffer_in), 0)) {
			// Se a mensagem foi recebida iniciar troca de mensagens
			cout<<"Componente conectado."<<endl;
			
			// Receber o header com o id do componente para poder identifica-lo
			memset(buffer_in, 0x0, TAM_BUFFER);
			int tam_mensagem;
			if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
				buffer_in[tam_mensagem] = '\0';
				strcpy(header, buffer_in);
				cout<<"ID do componente: "<<buffer_in<<endl;
				// Identificar o id dos componentes
				if(strcmp(header, "1000") == 0){
					cout<<"Cliente identificado."<<endl;
				}
				else if(strcmp(header, "0000") == 0){
					cout<<"Sensor de temperatura identificado."<<endl;
				}
				else if(strcmp(header, "0001") == 0){
					cout<<"Sensor de umidade identificado."<<endl;
				}
				else if(strcmp(header, "0010") == 0){
					cout<<"Sensor de nível de CO2 identificado."<<endl;
				}
				// Se for um atuador, pegar a última leitura do seu atributo e decidir se o
				// atuador precisa ser ligado ou desligado
				else if(strcmp(header, "0011") == 0){
					cout<<"Aquecedor identificado."<<endl;
					int aux;
					aux = (d.temperatura[0] - '0')*10;
					aux += d.temperatura[1] - '0';
					cout<<aux<<endl;
					// Mandar para o atuador o resultado
					if(aux < d.minTemp)
						send(clientefd, "1", 1, 0);
					else
						send(clientefd, "0", 1, 0);
					
					// Receber a mensagem do atuador para saber se foi executado
					if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
						buffer_in[tam_mensagem] = '\0';
						cout<<"Mensagem do componente: "<<buffer_in<<endl;
					}
					
					// Ao acabar ir para a parte do código para finalizar a comunicação
					send(clientefd, "Tchau\n", 6, 0);
					goto acabar;
				}
				else if(strcmp(header, "0100") == 0){
					cout<<"Resfriador identificado."<<endl;
					int aux;
					aux = (d.temperatura[0] - '0')*10;
					aux += d.temperatura[1] - '0';
					cout<<aux<<endl;
					if(aux > d.maxTemp)
						send(clientefd, "1", 1, 0);
					else
						send(clientefd, "0", 1, 0);
					
					if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
						buffer_in[tam_mensagem] = '\0';
						cout<<"Mensagem do componente: "<<buffer_in<<endl;
					}
					
					send(clientefd, "Tchau\n", 6, 0);
					goto acabar;
				}
				else if(strcmp(header, "0101") == 0){
					cout<<"Irrigador identificado."<<endl;
					int aux;
					aux = (d.umidade[0] - '0')*10;
					aux += d.umidade[1] - '0';
					cout<<aux<<endl;
					if(aux < d.minUmi)
						send(clientefd, "1", 1, 0);
					else
						send(clientefd, "0", 1, 0);
					
					if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
						buffer_in[tam_mensagem] = '\0';
						cout<<"Mensagem do componente: "<<buffer_in<<endl;
					}
					
					send(clientefd, "Tchau\n", 6, 0);
					goto acabar;
				}
				else if(strcmp(header, "0111") == 0){
					cout<<"Injetor de CO2 identificado."<<endl;
					int aux;
					aux = (d.co2[0] - '0')*10;
					aux += d.co2[1] - '0';
					cout<<aux<<endl;
					if(aux < d.minCO)
						send(clientefd, "1", 1, 0);
					else
						send(clientefd, "0", 1, 0);
					
					if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
						buffer_in[tam_mensagem] = '\0';
						cout<<"Mensagem do componente: "<<buffer_in<<endl;
					}
					
					send(clientefd, "Tchau\n", 6, 0);
					goto acabar;
				}
				else
					cout<<"Componente não identificado."<<endl;
			}
			cout<<"Esperando pela mensagem do componente..."<<endl;
			
			// Começa a comunicação entre eles até que uma mensagem de fim seja recebida
			do {
				// Limpar o buffer antes de receber a mensagem
				memset(buffer_in, 0x0, TAM_BUFFER);
				memset(buffer_out, 0x0, TAM_BUFFER);
				// Recebendo a mensagem do cliente
				if((tam_mensagem = recv(clientefd, buffer_in, TAM_BUFFER, 0)) > 0) {
					buffer_in[tam_mensagem - 1] = '\0';
					cout<<"Mensagem do componente: "<<buffer_in<<endl;
				}
				
				// A mensagem "finalizar" fecha a conexão e a mensagem "finalizar tudo" fecha o gerenciador
				if(strcmp(buffer_in, "finalizar") == 0 or strcmp(buffer_in, "finalizar tudo") == 0) {
					send(clientefd, "Tchau\n", 6, 0);
				} else {
					// Mensagem do gerenciador para o cliente com as últimas informações
					// armazenadas dos sensores
					if(strcmp(header, "1000") == 0 and strcmp(buffer_in, "analise") == 0){
						strcpy(buffer_out, d.temperatura);
						memcpy(buffer_out+2, d.umidade, 2);
						memcpy(buffer_out+4, d.co2, 2);
						buffer_out[6] = '\0';
						send(clientefd, buffer_out, strlen(buffer_out), 0);
						
					}
					// Receber a mensagem dos sensores e mandar se deseja continuar recebendo
					else if(strcmp(header, "0000") == 0){
						strcpy(d.temperatura, buffer_in);
						cout<<"Última temperatura atualizada para "<<d.temperatura<<"ºC"<<endl;
						cout<<"Deseja receber mais leituras de temperatura? (Sim ou Tchau) "<<endl;
						fgets(buffer_out, TAM_BUFFER, stdin);
						send(clientefd, buffer_out, strlen(buffer_out), 0);
						if(strcmp(buffer_out, "Tchau\n") == 0)
							break;
					}
					else if(strcmp(header, "0001") == 0){
						strcpy(d.umidade, buffer_in);
						cout<<"Última umidade atualizada para "<<d.umidade<<"g/m"<<endl;
						cout<<"Deseja receber mais leituras de umidade? (Sim ou Tchau) "<<endl;
						fgets(buffer_out, TAM_BUFFER, stdin);
						send(clientefd, buffer_out, strlen(buffer_out), 0);
						if(strcmp(buffer_out, "Tchau\n") == 0)
							break;
					}
					else if(strcmp(header, "0010") == 0){
						strcpy(d.co2, buffer_in);
						cout<<"Último nível de CO2 atualizado para "<<d.co2<<"%"<<endl;
						cout<<"Deseja receber mais leituras de CO2? (Sim ou Tchau) "<<endl;
						fgets(buffer_out, TAM_BUFFER, stdin);
						send(clientefd, buffer_out, strlen(buffer_out), 0);
						if(strcmp(buffer_out, "Tchau\n") == 0)
							break;
					}
					else{
						send(clientefd, "Mensagem incompreendida.", 24, 0);
					}
				}
				
			} while(strcmp(buffer_in, "finalizar") and strcmp(buffer_in, "finalizar tudo"));
			
			// Parte do código que finaliza a conexão com os componentes
			acabar:
			// Fechar a conexão com o cliente
			close(clientefd);
		}
		// Se receber a mensagem "finalizar tudo" o servidor se prepara para fechar o socket
		if(strcmp(buffer_in, "finalizar tudo") == 0)
			break;
	}
	
	// Fechar o socket local do gerenciador
	close(servidorfd);
	cout<<"Conexão finalizada com sucesso."<<endl;
	
	return 0;
}
