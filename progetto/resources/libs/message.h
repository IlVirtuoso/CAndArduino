/*struttura del messaggio*/
typedef struct {
    long messageType;
    char message[1024];
}message;