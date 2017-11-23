#include <allegro.h>
#include <math.h>
#include <time.h>
#include <string>
#include <iostream>


using namespace std;

/**
 * creazione di una finestra x * y,
 * dove x = lunghezza della finestra 
 * y= altezza
 * 
 * per avere un centro (partenza cellula malata), la finestra deve avere
 * x e y dispari
 * 
 * x e y rappresentano anche la dimensione della matrice di rappresentazione delle cellule della matrice 
 * 
 * 
 * NUMERO DI ELEMENTI= x*y
 */ 
const int x= 500;
const int y= 500;



/**
 * Costanti che determinano il tipo 
 * della cellula, seguite dai codici colore associati
 */

//cellula in stato normale
const int NORMAL_CELL=0;
int col_n; //colore normale

//cellula con tumore maligno
const int CANCER_CELL=1;
int col_cc; //colore cancer cell

//cellula morta
const int DEATH_CELL=2;
int col_dc; //colore death cell

//cella vuota
const int EMPTY_CELL=3;
int col_ec; //colore empty cell



/**
 * seguono le probabilità riguardo i processi della cellula
 */
const float Ps= 0.8; //survive su uno di quegli articoli la probabilita' era 0.6
const float Pp= 0.5;//proliferation

//NUMERO MASSIMO DI DIVISIONI PER CELLULE NORMALI
const int max_n= 5; //numero esempio, rappresentativo

//MACRO
#define PROBABILITA (float)((float)(rand()%100)/(float)(100));


/**
 * PASSI DA SEGUIRE:
 * -matrice inizializzata con cellule normali o spazi vuoti
 * -al centro della matrice vi è un unica cellula tumorale
 * -ogni cellula potrebbe morire con probabilità maggiore di ps
 * --se sopravvive, potrebbe riprodursi con probabilità Pp,solo se non ha superato il limite massimo di riproduzioni per una cellula (non tumorale), e solo se vicino a celle libere o se e' tumorale
 * ---se si riproduce si sposta accanto (scelta casuale su vicini liberi, o anche non liberi se tumorale)
 * --se muore la cella conta come vuota (ma colore diverso)
 * -il ciclo riparte da punto 3
 * 
 */



/**
 * struttura che rappresenta i pixel dello schermo
 * 
 */
BITMAP *applicazione; 
//numero di iterazioni della parte principale del programma
int iterazioni=0;

/**
 * matrice rappresentativa dell'evoluzione dell'
 * evoluzione delle cellule
 * ovviamente la dimensione è y*x, dove y è il numero di righe (rappresentante poi i pixel verticali) e x il numero di colonne (rappresentanti i pixel in orizzontale)
 */
int  cellula[y][x];

/**
 * questa matrice rappresenta le volte in cui la cellula 
 * i,j generica (ammesso che esista) si + riprodotta
 * 
 */
int riproduzione[y][x];

/**
 * questa funzione disegna i pixel della struttura BITMAP basandosi sulle celle della matrice cellule
 * 
 */
void disegna(){
    for (int i=0;i<y;i++){
        for(int j=0;j<x;j++){
            switch(cellula[i][j]){
               case NORMAL_CELL : putpixel(applicazione, j,i, col_n);break;

               case CANCER_CELL : putpixel(applicazione, j, i, col_cc); break;

               case DEATH_CELL : putpixel(applicazione, j,i, col_dc); break;

               default : putpixel(applicazione, j, i, col_ec);
            }//switch
        }//for j
    }//for i
    //il colore 255,255,255 e' bianco, l'altro il nero
    textprintf_ex(applicazione,font,0,0,makecol(255,255,255),makecol(0,0,0), "iterazioni: %i",iterazioni);
    blit(applicazione,screen,0,0,0,0,x,y);
}//disegna 




void crea_matrice(){
    int i,j;
    for(i=0; i< y; i++){
        for(j=0; j<x; j++){
            float p= PROBABILITA;
            if(p<0.5){
                //probabilita' cella vuota
                cellula[i][j]=EMPTY_CELL;
            }
            else {
                //probabilita' cella occupata
                cellula[i][j]=NORMAL_CELL;
            }

            //in ogni caso nella matrice delle riproduzioni, mettiamo 0
            riproduzione[i][j]=0;
        }//j
    }//i

    //la cella centrale sara' per definizione maligna
    //indice riga cella centrale
    i=(int)(y/2);
    //indice colonna cella centrale
    j=(int)(x/2);

    cellula[i][j]=CANCER_CELL;
    
}//crea atrice

void sdoppiaNormale(int i, int j){

    if (riproduzione[i][j]>max_n) return; //numero riproduzioni massime raggiunto

    int k=0,h=0;

    for(k=i-1; k<= i+1 ; k++){
        for(h=j-1; h<= j+1; h++){
            /**
             * questi due for scorrono la cella (i,j) e i suoi 8 vicini secondo la vicinanza di moore
             */

            if(h<0 || k<0 || k>=y || h>=x || (h==i && k==j)){
                //do nothing
            }else {
                if(cellula[k][h]==DEATH_CELL || cellula[k][h]==EMPTY_CELL){
                    cellula[k][h]=cellula[i][j];
                    riproduzione[i][j]++;
                    riproduzione[k][h]=0;
                    return ;
                }//cellula incriminata
            }//ifelse
        }//for h
    }//for k
}//sdoppiaNormale

void sdoppiaCancerogena(int i,int j){
    int vettoreCelluleBuone[2*8];
    int c=0;

    for(int k=i-1; k<= i+1 ; k++){
        for(int h=j-1; h<= j+1; h++){
            /**
             * questi due for scorrono la cella (i,j) e i suoi 8 vicini secondo la vicinanza di moore
             */

            if(h<0 || k<0 || k>=y || h>=x || (h==i && k==j)){
                //do nothing
            }else {
                if(cellula[k][h]==DEATH_CELL || cellula[k][h]==EMPTY_CELL){
                    cellula[k][h]=cellula[i][j];
                    return ;
                }//cellula incriminata
                else{
                    //potenziale cellula da mangiare
                    vettoreCelluleBuone[c]=k;
                    c++;
                    vettoreCelluleBuone[c]=h;
                    c++;
                }
            }//ifelse
        }//for h
    }//for k
    //nessuna cella libera o contenente cellule morte
    //dobbiamo mangiare una cellula vicina
    
    int scelta= rand() % (c/2);
    

    int k=vettoreCelluleBuone[scelta*2];
    int h=vettoreCelluleBuone[scelta*2+1];

    //se siamo qua nessuna cella e' stata mangiata
    //usiamo la memoria e mangiamo l'ultima cella
    cellula[k][h]=cellula[i][j];
}

int main(){
    crea_matrice();

    srand(time(0)); //crea una sequenza di numeri casuali



    allegro_init();
    install_keyboard();
    install_mouse();
    set_gfx_mode( GFX_AUTODETECT_WINDOWED, x , y, x, y);
    show_mouse(screen);
    applicazione=create_bitmap(x,y);

    //creazione colori:
    //per la cellula in stato normale e' stato pensato il colore verde
    col_n= makecol(0,255,0);
    //per cc e' stato pensato il colore giallo
    col_cc=makecol(255,255,0);
    //per dc e' stato pensato il colore nero
    col_dc=makecol(0,0,0);
    //per ec e' stato pensato il colore bianco
    col_ec=makecol(255,255,255);

    disegna();
   
    while(! key[KEY_ESC]){

        for(int i=0; i< y; i++){ //RIGHE
            for (int j=0 ; j< x ; j++){//COLONNA

                //fase 1: sopravvivenza
                if(cellula[i][j]==NORMAL_CELL || cellula[i][j]==CANCER_CELL ){
                    float pr=PROBABILITA;
                    if(pr > Ps){
                        cellula[i][j]=DEATH_CELL;
                    }
                }//if cella sopravvissuta


                //fase 2: riproduzione
                float pr=PROBABILITA;

                if(pr <= Pp){
                    //cellula si sdoppia
                    if(cellula[i][j]==NORMAL_CELL){
                        sdoppiaNormale(i,j);
                    }else if(cellula[i][j]==CANCER_CELL){
                        sdoppiaCancerogena(i,j);
                    }
                }//if cella sdoppiata

            }//for j
        }//for i

        iterazioni++;
        
        disegna(); //disegna la matrice
        rest(10); //dorme per mezzo secondo = non affatica la cpu

    }//while

    
    return 0;
}END_OF_MAIN();

