/*
 * Cliente TCP.
 * 
 * Demonstra utilização da interface de sockets para implementar um cliente TCP.
 * 
 * Autor: Carlos Limão
 * 
 * Notas:
 *     1. Este programa compila quer em Windows, quer em UNIX/LINUX.
 *        Para isso, o #define inicial deve ser alterado em conformidade.
 *     2. Este programa pressupõe que o programa servidor (tcp_serv.c) está a
 *        executar na máquina cujo endereço IP está especificado em IP_ADDRESS.
 *     3. Este programa estabelece uma ligação ao servidor e envia-lhe um 
 *        pedido. Depois disso espera uma resposta do servidor.
 *     4. Após a sequência descrita no passo anterior o programa termina.
 * 
 * Geração do executável:
 *     Windows MSVC: 
 *        Criar "C++ Empty Project"; 
 *        Adicionar este ficheiro ao projeto; 
 *        Alterar para "#define WIN"; 
 *        Selecionar "Build".
 *     Windows na linha de comandos: 
 *        Alterar para "#define WIN"; 
 *        Compilar usando "cl tcp_cli.c"; 
 *        Executar usando "tcp_cli".
 *     UNIX/LINUX na linha de comandos: 
 *        Alterar para "#define BSD"; 
 *        Compilar usando "gcc -o tcp_cli tcp_cli.c"; 
 *        Executar usando "./tcp_cli".
 * 
 * Utilização das funções da interface de sockets no cliente e no servidor:
 * 
 *      Servidor                  Cliente
 *
 *     +--------+                +--------+
 *     | socket |                | socket |
 *     +--------+                +--------+
 *         |                          |
 *     +--------+                     |
 *     |  bind  |                     |
 *     +--------+                     |
 *         |                          |
 *     +--------+                     |
 *     | listen |                     |
 *     +--------+                     |
 *         |                          |
 *     +--------+  est. ligação  +---------+
 *     | accept | <------------- | connect |
 *     +--------+                +---------+
 *         |                          |
 *     +--------+     pedido     +---------+
 *     |  send  | <------------- |   recv  |
 *     +--------+                +---------+
 *         |                          |
 *     +--------+    resposta    +---------+
 *     |  recv  | -------------> |   send  |
 *     +--------+                +---------+
 *         |                          |
 *        FIM                        FIM
 */
#define  WIN              // WIN para compilar para Windows e BSD para compilar para UNIX/LINUX.

#include <stdio.h>        // Necessário para usar printf()
#include <string.h>       // Necessário para usar memcpy() e strcpy()
#include <stdlib.h>       // Necessário para usar exit()
#include <locale.h>       // Necessário para usar setlocale() - carateres acentuados
#ifdef WIN
#include <windows.h>      // Necessário para usar a interface de sockets no Windows
#endif
#ifdef BSD
#include <sys/types.h>    // Necessário para usar a interface de sockets no UNIX/LINUX
#include <netinet/in.h>   // Necessário para usar a interface de sockets no UNIX/LINUX
#include <sys/socket.h>   // Necessário para usar a interface de sockets no UNIX/LINUX
#include <arpa/inet.h>    // Necessário para usar a interface de sockets no UNIX/LINUX
#include <fcntl.h>        // Necessário para usar a interface de sockets no UNIX/LINUX
#include <netdb.h>        // Necessário para usar a interface de sockets no UNIX/LINUX
#include <unistd.h>       // Necessário para usar a interface de sockets no UNIX/LINUX
#endif

#ifdef WIN
// Para evitar ter de especificar a biblioteca Winsock nas propriedades do projeto.
#pragma comment(lib, "ws2_32.lib")
#endif

// Constantes - Devem ser alteradas para adaptar à situação real.
#define  PORT_NUMBER          1050   // Número do porto usado pelo servidor
#define  IP_ADDRESS    "127.0.0.1"   // Endereço IP do servidor

int main()
{
#ifdef WIN
	WORD wVersionRequested = MAKEWORD(1, 1);      // Necessário para a inicialização da interface de sockets no Windows (winsock)
	WSADATA wsaData;                              // Necessário para a inicialização da interface de sockets no Windows (winsock)
#endif
	int                  client_s;        // Identificador do socket usado pelo cliente.
	struct sockaddr_in   server_addr;     // Endereço Internet do servidor.
    struct in_addr       server_ip_addr;  // Endereço IP do servidor
	char                 out_buf[4096];   // Buffer a usar para o envio de dados.
	char                 in_buf[4096];    // Buffer a usar para a receção de dados.
	int                  retcode;         // Código de terminação devolvido pelas funções da interface de sockets.

    setlocale(LC_CTYPE, "pt_PT");

#ifdef WIN
    // Inicialização da interface de sockets no Windows
	WSAStartup(wVersionRequested, &wsaData);
#endif

    printf("CLIENTE TCP\n\n");

    /*
     * Passo 1: Criação de um socket a usar pelo cliente
     *   - AF_INET significa Address Family Internet
     *   - SOCK_STREAM significa que o socket é lido como se fosse um ficheiro.
     *   - IPPROTO_TCP significa que o protocolo a usar é o TCP (0 resultaria tb. na escolha do TCP)
     */
    client_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_s < 0) {
        printf("ERRO - socket() falhou.\n");
        exit(-1);
    }

    /*
     * Passo 2: Preencher a informação relacionada com o endereço do servidor e
     *          estabelecer uma ligação ao servidor usando a função connect().
     *          A função connect() é bloqueante. Isto é, só retorna depois de 
     *          estabelecida uma ligação ou depois de timeout.
     */
    server_addr.sin_family = AF_INET;                    // Família de endereços a usar.
    server_addr.sin_port = htons(PORT_NUMBER);           // Número do porto a usar.
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS); // Endereço IP a usar.
    retcode = connect(client_s, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (retcode < 0) {
        printf("ERRO - connect() falhou.\n");
        exit(-1);
    }

    // Copiar os 4 bytes do endereço IP para uma variável do tipo in_addr (endereço IP).
    memcpy(&server_ip_addr, &server_addr.sin_addr.s_addr, 4);

    // Indicar que a ligação foi estabelecida com sucesso.
    printf("Connect concluido (Endereco IP do servidor = %s  porto = %d) \n",
        inet_ntoa(server_ip_addr), ntohs(server_addr.sin_port));

    /*
     * Passo 3: Enviar pedido para o servidor.
     */
    strcpy(out_buf, "Este é um pedido enviado do CLIENTE para o SERVIDOR.");
    retcode = send(client_s, out_buf, strlen(out_buf) + 1, 0);
    if (retcode < 0) {
        printf("ERRO - send() falhou.\n");
        exit(-1);
    }

    /*
     * Passo 4: Receber resposta do servidor.
     */
    retcode = recv(client_s, in_buf, sizeof(in_buf), 0);
    if (retcode < 0)    {
        printf("ERRO - recv() falhou.\n");
        exit(-1);
    }

    // Mostrar mensagem recebida
    printf("Recebido do servidor: %s \n", in_buf);

    /*
     * Passo 5: Fechar o socket cliente.
     */
#ifdef WIN
    retcode = closesocket(client_s);
    if (retcode < 0) {
        printf("ERRO - closesocket() falhou.\n");
        exit(-1);
    }
#endif
#ifdef BSD
    retcode = close(client_s);
    if (retcode < 0) {
        printf("ERRO - close() falhou.\n");
        exit(-1);
    }
#endif

#ifdef WIN
    // Terminar utilização da interface winsock.
    WSACleanup();
#endif

    // Terminar devolvendo código de terminação 0.
    return(0);
}
