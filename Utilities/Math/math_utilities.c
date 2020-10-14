// Realizzato da Taiello R. e Pellegrino M.
unsigned long getMCD (unsigned long a, unsigned long b) {
    while(a!=b){
        if(a>b) {
            a-=b;
        } else {
            b-=a;
        }
    }
    return a;
}

