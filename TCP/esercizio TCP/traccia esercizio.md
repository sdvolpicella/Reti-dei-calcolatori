Progettare ed implementare un’applicazione TCP client/server, conforme al seguente protocollo applicativo:

    - Il client richiede la connessione al server.
    - Stabilita la connessione, il server invia al client la stringa "connessione avvenuta".
    - Il client stampa la stringa ricevuta; dopodiché, legge due stringhe di caratteri dallo standard input e le invia al server.
    - Il server legge le stringhe inviate dal client e:
       - le visualizza sullo standard output;
       - converte la prima stringa ricevuta tutta in MAIUSCOLO, la seconda tutta in minuscolo;
       - invia le nuova stringhe al client;
       - rimane in ascolto!
    -Il client legge la risposta inviata dal server.
       - visualizza la risposta sullo standard output;
       - chiude la connessione corrente;
       - termina il processo.
