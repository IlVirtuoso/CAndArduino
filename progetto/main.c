#include <stdlib.h>
#include <stdio.h>
#include "macro.h"
 

int main(){

    /*Codice MASTER
         1.
         creazione Table
         creazione processi
         2.
         -> WAIT for READY from PLAYER
         imposta il segnale di Alarm
         porre le bandierine su Table
            - posizione non occupata
            - in numero tale da ripartire SO_ROUND_SCORE
         -> SIGNAL START
         3. STOP
            3.1 (Caso TAKE)
                STOP il gioco e alarm 
                -> com_take()
                -> WAIT for READY from PLAYER
                -> alarm & SIGNAL START
            3.2 (caso END ROUND)
                stampa table + results
                -> torna a 2.
            3.3 (caso END MOVES)
                stampa table + results
                -> free_all() & end
    */

   /*Codice PLAYER
        1.
            creazione e posizionamento (casuale) pedine
        2.
            2.1 (inizio round)
                -> SIGNAL CREATE from MASTER
                indicazioni pedine
                -> SPEDISCI READY to MASTER
                -> WAIT for SIGNAL START from MASTER
                -> START
            2.2 (during round)
                -> SIGNAL HELP from PAWN
                    imposta nuove indicazioni per il sender
                -> SIGNAL CATCH from PAWN
                    -> SIGNAL TAKE to MASTER
                    imposta nuova tattica
                    -> WAIT for SIGNAL START from MASTER
            -> WAIT FOR SIGNAL STOP
            torna a 2.
   */

    /*Codice PAWN
        1. 
            -> WAIT for SIGNAL START from PLAYER
        2. MOVE
            - check su tattica
            - if bandiera mancante
                tattica 1:
                    tattica (auto)
                        cerca la bandiera
                    tattica (help)
                        -> SIGNAL HELP to PLAYER
                        -> WAIT for new INSTRUCTION
            - move()
                2.1 case 0 (libera)
                    libera la cella occupata
                    transisci (occupando la cella)
                    esegui nanosleep
                2.2 case 1 (flag)
                    libera la cella occupata
                    transisci (occupando la cella)
                    SIGNAL CATCH to PLAYER
                    WAIT for new INSTRUCTION
                2.3 case ? (occupata)
                    check su tattica
                        2.3.1 (tattica auto)
                            move() in altra direzione
                            if tutte le celle occupate -> WAIT & try again
                        2.3.2 (help)
                            WAIT for INSTRUCTION
                            move() in altra direzione
                            if tutte le celle occupate -> WAIT & try again


    */


    return 0;
}