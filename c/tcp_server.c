/*
 * Servidor TCP.
 *
 * Demonstra utilização da interface de sockets para implementar um servidor TCP.
 *
 * Autor: Carlos Limão
 *
 * Notas:
 *     1. Este programa compila quer em Windows, quer em UNIX/LINUX.
 *        Para isso, o #define inicial deve ser alterado em conformidade.
 *     2. Este programa espera uma ligação de um cliente (tcp_cli.c), e a
 *        posterior receção de um pedido. De seguida, envia uma resposta.
 *     3. Após a sequência descrita no passo anterior o programa termina.
 *
 * Geração do executável:
 *     Windows MSVC: Criar "C++ Empty Project"; 
 *                   Adicionar este ficheiro ao projeto; 
 *                   Alterar para "#define WIN"; 
 *                   Selecionar "Build".
 *     Windows na linha de comandos:
 *                   Alterar para "#define WIN"; 
 *                   Compilar usando "cl tcp_server.c"; 
 *                   Executar usando "tcp_server".
 *     UNIX/LINUX na linha de comandos: 
 *                   Alterar para "#define BSD"; 
 *                   Compilar usando "gcc -o tcp_server tcp_server.c"; 
 *                   Executar usando "./tcp_server".
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
 *     |  recv  | <------------- |   send  |
 *     +--------+                +---------+
 *         |                          |
 *     +--------+    resposta    +---------+
 *     |  send  | -------------> |   recv  |
 *     +--------+                +---------+
 *         |                          |
 *        FIM                        FIM
 */
#define  WIN                // WIN para compilar para Windows e BSD para compilar para UNIX/LINUX.

#include <stdio.h>          // Necessário para usar printf()
#include <string.h>         // Necessário para usar memcpy() e strcpy()
#include <stdlib.h>         // Necessário para usar exit()
#include <locale.h>         // Necessário para usar setlocale() - carateres acentuados
#ifdef WIN
#include <windows.h>        // Necessário para usar a interface de sockets no Windows
#endif
#ifdef BSD
#include <sys/types.h>      // Necessário para usar a interface de sockets no UNIX/LINUX
#include <netinet/in.h>     // Necessário para usar a interface de sockets no UNIX/LINUX
#include <sys/socket.h>     // Necessário para usar a interface de sockets no UNIX/LINUX
#include <arpa/inet.h>      // Necessário para usar a interface de sockets no UNIX/LINUX
#include <fcntl.h>          // Necessário para usar a interface de sockets no UNIX/LINUX
#include <netdb.h>          // Necessário para usar a interface de sockets no UNIX/LINUX
#include <unistd.h>         // Necessário para usar a interface de sockets no UNIX/LINUX
#endif

#ifdef WIN
 // Para evitar ter de especificar biblioteca Winsock nas propriedades do projeto.
#pragma comment(lib, "ws2_32.lib")
#endif

// Constantes - Devem ser alteradas para adaptar à situação real.
#define  PORT_NUMBER   1050    // Número do porto (arbitrário >= 1024) usado pelo servidor

int main()
{
#ifdef WIN
    WORD wVersionRequested = MAKEWORD(1, 1);      // Necessário para a inicialização da interface de sockets no Windows (winsock)
    WSADATA wsaData;                              // Necessário para a inicialização da interface de sockets no Windows (winsock)
#endif
    int                  accept_s;        // Identificador do socket usado para aceitar novas ligações.
    struct sockaddr_in   server_addr;     // Endereço Internet do servidor.
    int                  connect_s;       // Identificador do socket ligado ao cliente.
    struct sockaddr_in   client_addr;     // Endereço Internet do cliente.
    struct in_addr       client_ip_addr;  // Endereço IP do cliente.
    int                  addr_len;        // Comprimento de um endereço Internet.
    char                 out_buf[4096];   // Buffer a usar para o envio de dados.
    char                 in_buf[4096];    // Buffer a usar para a receção de dados.
    int                  retcode;         // Código de terminação devolvido pelas funções da interface de sockets.

    setlocale(LC_CTYPE, "pt_PT");

#ifdef WIN
    // Inicialização da interface de sockets no Windows
    WSAStartup(wVersionRequested, &wsaData);
#endif

    printf("SERVIDOR TCP\n\n");

    /*
     * Passo 1: Criar um socket para aceitar ligações.
     *   - AF_INET significa Address Family Internet
     *   - SOCK_STREAM significa que o socket é lido como se fosse um ficheiro.
     *   - IPPROTO_TCP significa que o protocolo a usar é o TCP (0 resultaria tb. na escolha do TCP)
     */
    accept_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (accept_s < 0) {
        printf("ERRO - socket() falhou.\n");
        exit(-1);
    }

    /*
     * Passo 2: Preencher a informação relacionada com o endereço do servidor e
     *          associá-lo ao socket usado para aceitar ligações dos clientes.
     */
    server_addr.sin_family = AF_INET;                 // Família de endereços a usar.
    server_addr.sin_port = htons(PORT_NUMBER);        // Número do porto a usar.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Escutar em qualquer endereço IP.
    retcode = bind(accept_s, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (retcode < 0) {
        printf("ERRO - bind() falhou.\n");
        exit(-1);
    }

    /*
     * Passo 3: Escutar uma ligação no socket usado para aceitar ligações.
     */
    listen(accept_s, 1);

    /*
     * Passo 4: Aceitar uma ligação. A função accept() é bloqueante e retorna devolvendo 
     * um socket, ligado ao cliente, e com o respetivo endereço (client_addr) preenchido.
     */
    printf("Aguarda ligação de um cliente ... \n");
    addr_len = sizeof(client_addr);
    connect_s = accept(accept_s, (struct sockaddr*)&client_addr, &addr_len);
    if (connect_s < 0) {
        printf("ERRO - accept() falhou.\n");
        exit(-1);
    }

    // Copiar os 4 bytes do endereço IP para uma variável do tipo in_addr (endereço IP).
    memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

    // Indicar que uma ligação foi aceite com sucesso.
    printf("Accept concluido (Endereço IP do cliente = %s  porto = %d) \n",
        inet_ntoa(client_ip_addr), ntohs(client_addr.sin_port));

    /*
     * Passo 5: Aguarda pedido do cliente no socket em que a ligação foi estabelecida.
     */
    retcode = recv(connect_s, in_buf, sizeof(in_buf), 0);
    if (retcode < 0) {
        printf("ERRO - recv() falhou.\n");
        exit(-1);
    }

    printf("Recebido do cliente: %s \n", in_buf);

    /*
     * Passo 6: Enviar resposta para o cliente no socket em que a ligação foi estabelecida.
     */
    strcpy(out_buf, "Esta e' uma resposta do SERVIDOR para o CLIENTE.");
    retcode = send(connect_s, out_buf, strlen(out_buf) + 1, 0);
    if (retcode < 0) {
        printf("ERRO - send() falhou.\n");
        exit(-1);
    }

    /*
     * Passo 7: Fechar os sockets accept_s e connect_s
     */
#ifdef WIN
    retcode = closesocket(accept_s);
    if (retcode < 0) {
        printf("ERRO - closesocket() falhou.\n");
        exit(-1);
    }

    retcode = closesocket(connect_s);
    if (retcode < 0) {
        printf("ERROR - closesocket() falhou.\n");
        exit(-1);
    }
#endif

#ifdef BSD
    retcode = close(accept_s);
    if (retcode < 0) {
        printf("ERRO - close() falhou.\n");
        exit(-1);
    }

    retcode = close(connect_s);
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
