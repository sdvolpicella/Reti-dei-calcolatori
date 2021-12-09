Progettare ed implementare un'applicazione TCP client/server, dove il server è una calcolatrice remota che risponde alle richieste del client nel seguente modo:

    1. Il client legge da riga di comando l'indirizzo e il numero di porta del server (es. 60000), dopodiché richiede
       la connessione al server.
    
    2. Ricevuta la connessione, il server visualizza sullo standard output l'indirizzo e il numero di porta del 
       client, scrivendo "Connection established with xxx.xxx.xxx.xxx:yyyyy".
    
    3. Stabilita la connessione, il client legge l'operazione da compiere (usando i caratteri +, x, -, /, rispettivamente 
       per Addizione, Moltiplicazione, Sottrazione e Divisione) e due numeri interi dallo standard input (es. + 23 45) ed 
       invia al server quanto letto da tastiera.
    
    4. Il server legge quanto inviato dal client, esegue l'operazione richiesta e invia il risultato al client (es. 68).
    
    5. Il client legge la risposta inviata dal server e la visualizza sullo standard output.
    
    6. Il client legge dallo standard input la successiva operazione da compiere.
    
    7. Se invece di un'operazione è inserito il carattere =, il client chiude la connessione con il server e termina qui il
       suo processo; altrimenti, torna al punto 3.
    
    8. Il server non termina mai il suo processo e deve essere in grado di accettare una coda massima di 5 client 
       (parametro qlen).
