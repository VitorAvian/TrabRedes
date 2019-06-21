**Estufa Inteligente
Requisitos
1. Sensoriamento:** 
   - Todos os sensores têm um identificador único. 
   - Os sensores devem se conectar ao Gerenciador e se identificar. 
   - Após receber confirmação, os sensores deverão enviar sua leitura a cada 1s ao Gerenciador.
**2. Atuadores:**
   - Todos os atuadores têm um identificador único.
   - Os atuadores devem se conectar ao Gerenciador e se identificar.        
   - Os atuadores poderão ser ligados ou desligados pelo Gerenciador. 
**3. Gerenciador:** 
   - O Gerenciador deverá aceitar a conexão de sensores e atuadores do sistema. 
   - O Gerenciador deve receber as leituras de todos os sensores do sistema e armazenar o último valor recebido. 
   - O Gerenciador deve ligar ou desligar os atuadores caso os valores das leituras de sensores indicarem que as variáveis estão fora dos valores máximo e mínimo configurados. 
   - O Gerenciador deve ser capaz de fornecer ao Cliente a última leitura de cada sensor do sistema quando receber uma requisição.
**4. Monitoramento:**
   - O Cliente deve ser capaz de requisitar a última leitura de qualquer sensor do sistema ao Gerenciador.
________________
**Descrição do Protocolo**
O sistema possuirá 3 sensores para a medida da temperatura interna, umidade do solo e nível de CO2, cada um com um identificador de 2 bits. Também contará com 4 atuadores, aquecedor, resfriador, sistema de irrigação e injetor de CO2, cada um com um identificador de 2 bits. O sistema também irá possuir um gerenciador com 8 portas, que iram ser conectadas as todas as partes do sistema. E por fim, haverá um cliente com um identificador de 4 bits.
Os dados trocados entre as mensagens possuirão um header fixo, com um campo de dado variável.

________________
**Mensagens**  

**Mensagens de sensores para gerenciador**
1. Solicitar_conexão:
   - Identificador (obrigatório)
2. Enviar_dados:
   - Dados da mensagem (obrigatório)

Essa sessão cumpre os requisitos 1.2 e 1.3 através da solicitação e envio dos dados dos sensores de temperatura, umidade do solo e nível de CO2 e seus respectivos dados.

**Mensagens do gerenciador para sensores**
1. Confirmar_conexão:
   - Confirmação (obrigatório).
2. Receber_dados:
   - Dados da mensagem (obrigatório).
        
        Essa sessão cumpre os requisitos 3.1 e 3.2 através da confirmação da conexão solicitada pelos sensores e o recebimento dos dados enviados, que depois serão armazenados.

**Mensagens do atuadores para gerenciador**
1. Solicitar_conexão:
   - Identificador(obrigatório).

Essa sessão cumpre o requisito 2.2, através da solicitação da conexão pelos atuadores.

**Mensagens do gerenciador para atuadores**
1. Confirmar_conexão:
   - Confirmação(obrigatório).
2. Mudar_estado:
   - Estado de mudança (Ligar/desligar) (obrigatório).

        Essa sessão cumpre os requisitos 2.2 e 2.3 através da confirmação da conexão gerenciador-atuador e da modificação do estado do atuador, seja ligando-o ou desligando-o.
        
**Mensagens do cliente para o gerenciador**
1. Solicitar_conexão:
   - Identificador (obrigatório)
   - Identificador do sensor (opcional)
2. Requisitar_dados:
   - Dado solicitado (obrigatório)
        
        Essa sessão cumpre o requisito 4.1 através do requerimento da última leitura de um determinado sensor ou de todos eles. 
        
**Mensagens do gerenciador para o cliente**
1. Confirmar_conexão:
   - Confirmação (obrigatório).
2. Recuperar_dados
   - Dados do sensor (obrigatório).
3. Enviar_dados:
   - Dados da mensagem (obrigatório).


        Essa sessão cumpre o requisito 3.4 por meio da confirmação da conexão, da recuperação de dados dos sensores e do envio dos mesmos.
