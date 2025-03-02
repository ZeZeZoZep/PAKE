int invert_single(int *b,int *e,int k,int q){
    int s=0;
    for(int i=k-1; i>=0; i--){
        unsigned int x=(unsigned int)round(b[i]-pow(2,i)*s) % q;
            // Estrai il bit x_{k-1} (il bit più significativo)
        unsigned int x_k_minus_1 = (x >> (k - 1)) & 1;

        // Estrai il bit x_{k-2}
        unsigned int x_k_minus_2 = (x >> (k - 2)) & 1;

        // Calcola x_{k-1} ⊕ x_{k-2}
        unsigned int result = x_k_minus_1 ^ x_k_minus_2;
        s=s+pow(2,k-1-i)*result;
        int temp=x;
        while(!(temp>=-q/4 && temp<q/4)) if (temp>0) temp-=q/2; else temp += q/2;
        e[i]=temp;
    }
    return s;
    
}
