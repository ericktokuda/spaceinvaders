// Space Invaders 

// Invaders: - Eric K. Tokuda
//           - Pedro T. E. de Andrade


// Comments: The program supports very well changes in global
// variables. The code is supposed to generate the
// readjustments automatically.
// The most amusing constants to change are as follows: t_tempo
// (30 facilitates), t_ini_tempo (30 hinders), tx_dec_block_tempo (11
// 	   hampers a lot!), ovni_tempo ("speed" of the UFO), ovni_rand_tempo
// (n000 does it take n secs to pass), columns_atirando_lim
// (in max ini_num root), ini_num (perfect square), stars
// (in the max 10000 thousand, I think). All work was done in less than 4 days.
// We could not then work a bit more on the graphic part of the ship
// or the UFO. A bug that happens is the acceleration of shots of the ship and the
// how much of the enemy, but it is momentary and rare. There was no time to
// analyze exactly why (P.S.:Remediado). There may be
// a small bug also in the rare case of victory and defeat to the same
// time, when you no longer have more life to spend.
// For example, imagine the work that made giving shields
// which are not all the same and, even more, triangular. We did not know
// at the time how to use transparency, so we do not see stars or the
// shield in the circle black inside the enemy O. 
// And the UFO flying at the end of the game was purposeful.
// The dimensions of the window can also be changed and there is a reset
// even the abagence of the enemy's movement. You can check the variables
// are comp and alt, the first of the constants list.
               
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
/*#include "gfont.h"*/
 

typedef struct {
  float x, y, vida;

} ponto;

//CONSTANTS

float comp=400, alt=600; //screen

int n_vidas=1, ganha=0, perde=0; 

int iniciado=0; //no easy explanation... :P

float espaco_ini=20, lado/*nave&ini*/=25, lado_esc=45, ladinho_esc, espaco_esc=100, topo_esc;  

float margem=10, margem2=2, margem3, margem_x_lp, 
margem_y_lp=25, margem_x_lv, margem_y_lv=25; //margins

float n_x, n_y, morto=0; 

float t_alt=10, t_comp=2, t_x, t_y, atirado=0; //shoot-enemy 

float t_tempo=50, t_tempo_ini=50, estado_tempo=500, bloco_tempo=400, ovni_tempo=30, atira_ini_tempo=300, ressucita_tempo=1500, tx_dec_bloco_tempo=10, cenario_tempo=100, ovni_rand_tempo=20000; //tempos TimerFunc, 't_' se refere a tiro  

int rumo=0, rumo_lim, ida_vinda=1; //da movimentacao do bloco de inimigos

float mov_X=4; //cte animação do inimigo X 

int estado=0; //estado animação dos inimigos (booleana)

int veloc_bloco=10;

// Enemy attributes

//A partir de agora 1000 = ini_num = quant. de inimigos (quadrado perfeito) e 100 = sqrt(ini_num) = ini_col_num = quant. de inimigos por coluna

ponto bloco[100][100], esc[3][7]; //coords e vida de inimigos e escudos 

int coluna_ini[100]; //inimigos vivos por coluna

int atirado_ini[100];//colunas com tiro ativo (booleana)

int t_ini_x[100], t_ini_y[100];//coords. tiros das colunas
int colunas_atirando=0,colunas_atirando_lim=4; 

int ini_num=32, ini_vivos, ini_col_num;

int linha_toca_solo;//indice da linha viva mais abaixo / so muda no max quando morre um inimigo

//FIM de ATRIBUTOS DO INIMIGO

int estrelas=200; //num. de estrelas no cenário
int ceu_x[10000],ceu_y[10000]; //10000=estrelas definido anteriormente, guardam as posições das estrelas

int acelera_cenario=0,tx_acel_cenario=5,nave_em_tela=1;//da guca da nave quando vence

float ovni_x, ovni_y, ovni_raio=13, cte_x=2, cte_y=1, ovni_elipse_rx, ovni_elipse_ry, ovni_passando=0, veloc_ovni=10, cor; //atributos do ovni

//do letreiro
int pontuacao=0, letra_tam=16, letra_esp=3;
char p[30];
float cte_vidas=0.75;


/*Protótipos*/

int interseccao(int x1,int y1,int a1,int l1,int x2,int y2,int a2,int l2);
void ativa_ovni();
void atira_ini();
void move_tiro();
void move_tiro_ini(int coluna);
void inicia();
void move_bloco();
void move_ovni();
void muda_estado();
void verifica_acerto();
void verifica_acerto_ini(int coluna);
void ressucita();
void sorteia_cenario();
void movimenta_cenario(); 
void vitoria(); //encerramento: movimento vertical da nave

void circulo(int raio, int xc, int yc);
void elipse(int raio, int xc, int yc, int ctex, int ctey);
void nave (int nx, int ny, float ladoo);
void tiro(int tx, int ty);
void inimigo(int x, int y, int tipo, int estado);
void escudos();
void cenario();
void ovni();
void letreiro();

void display();

void setas(int tec, int x, int y);
void barra(unsigned char tecla,int x, int y);


//PROCEDIMENTOS 
void displayText( float x, float y, int r, int g, int b, const char *string ) {
	int j = strlen( string );
 
	glColor3f( r, g, b );
	glRasterPos2f( x, y );
	int i =0;
	for( ; i < j; i++ ) {
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
	}
}
int interseccao(int x1,int y1,int a1,int l1,int x2,int y2,int a2,int l2) {

if( (x1<=x2+l2) && (x1+l1>=x2) && (y1<=y2+a2) && (y1+a1>=y2) ) 
  return(1);
else
  return(0);

} 


void ativa_ovni() {

if (cor==1) cor=-1; else cor=1;

ovni_passando=1;

glutTimerFunc(ovni_tempo,move_ovni,0);

}


void atira_ini() {

int i,j,r;

if (perde) return;

if(morto) {
 glutTimerFunc(rand()%1500,atira_ini,0);
 return;
}

if (colunas_atirando==colunas_atirando_lim) { 
  glutTimerFunc(rand()%1500,atira_ini,0);
  return;
}  
r=rand()%ini_col_num;
if(coluna_ini[r] && !atirado_ini[r]) {
  atirado_ini[r]=1;
  colunas_atirando++;
  for(i=ini_col_num-1;i>=0;i--)
    if(bloco[i][r].vida) {
      t_ini_x[r]=bloco[i][r].x+lado/2-t_comp/2;
      t_ini_y[r]=bloco[i][r].y+lado;
      break;
    }
  glutPostRedisplay();
  verifica_acerto_ini(r);
  glutTimerFunc(t_tempo,move_tiro_ini,r);     
}
glutTimerFunc(rand()%100,atira_ini,0);


}


void move_tiro_ini(int coluna) {

if(!atirado_ini[coluna])
  return;

t_ini_y[coluna]+=10;
glutPostRedisplay();

if(t_ini_y[coluna]>=alt) {
  atirado_ini[coluna]=0;
  colunas_atirando--;
  return;  
}

glutTimerFunc(t_tempo_ini,move_tiro_ini,coluna);


}

void move_tiro() {

if (!atirado) 
  return;
t_y-=20;
glutPostRedisplay();
if (t_y+t_alt<=-t_alt) {
  atirado=0; 
  return;
}

glutTimerFunc(t_tempo,move_tiro,0);


}


void inicia() { //pos inicial dos inimigos e escudos

int i,j,inverte=0,translada=0;

for(i=0;i<=ini_col_num-1;i++)
  for(j=0;j<=ini_col_num-1;j++) {
    bloco[i][j].x=margem+j*lado+j*espaco_ini; 
    bloco[i][j].y=margem3+2*ovni_elipse_ry+margem+i*lado+i*espaco_ini;
    bloco[i][j].vida=1;
  }

for(i=0;i<=2;i++)
  for(j=0;j<=6;j++)
    esc[i][j].vida=0.9;


//regioes de impacto do escudo  

for(i=0;i<=2;i++) {
  
  if (i==1) 
    inverte=translada=0;
  if ((i==0) || (i==2))
    inverte=1;
  if (i==0)
    translada=-1;
  if (i==2)
    translada=1;

  esc[i][0].x=comp/2-ladinho_esc/2+(translada*espaco_esc);
  esc[i][0].y=alt-4*margem-lado-lado_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc)));

  esc[i][1].x=comp/2-ladinho_esc+(translada*espaco_esc);
  esc[i][1].y=alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc)));

  esc[i][2].x=comp/2+(translada*espaco_esc);
  esc[i][2].y=alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc)));

  esc[i][3].x=comp/2-lado_esc/2+(translada*espaco_esc);
  esc[i][3].y=alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc)));

  esc[i][4].x=comp/2-lado_esc/2+ladinho_esc/2+(translada*espaco_esc);
  esc[i][4].y=alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc)));

  esc[i][5].x=comp/2+(translada*espaco_esc);
  esc[i][5].y=alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc)));

  esc[i][6].x=comp/2+ladinho_esc+(translada*espaco_esc);
  esc[i][6].y=alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc+ladinho_esc)));

}


}

void move_bloco(){

int i,j,n;

for(n=0;n<=rumo_lim-1;n++)
  if (rumo==n) 
    for(i=0;i<=ini_col_num-1;i++)
      for(j=0;j<=ini_col_num-1;j++)
        bloco[i][j].x+=(ida_vinda)*veloc_bloco; 
//EXPERIMENTAR glutPostRedisplay();
if (rumo==rumo_lim) {
  for(i=0;i<=ini_col_num-1;i++)
    for(j=0;j<=ini_col_num-1;j++)
      bloco[i][j].y+=veloc_bloco; 

  //verifica se inimigos tocaram o solo => derrota
  if(bloco[linha_toca_solo][0].y+lado>alt-margem-lado) 
    perde=1;
  
}
  
  
rumo++;
rumo%=rumo_lim+1;
if (rumo==0) ida_vinda*=-1;

glutPostRedisplay();
glutTimerFunc(bloco_tempo,move_bloco,0);

}


void move_ovni() {

if(!ovni_passando) return;

ovni_x-=veloc_ovni;
glutPostRedisplay();

if(ovni_x+2*cte_x*ovni_raio<0) {
  ovni_passando=0;
  ovni_x=comp;
  ovni_y=margem3;
  glutTimerFunc(rand()%abs(ovni_rand_tempo),ativa_ovni,0); //rechamar ovni
  return;
}

glutTimerFunc(ovni_tempo,move_ovni,0);

}


void muda_estado() {

estado++;
estado%=2;
glutPostRedisplay();
glutTimerFunc(estado_tempo,muda_estado,0); 
}


void verifica_acerto() {

int i,j,k,l,quebrou_dentro=0;

  //acerto em inimigos
  for(i=0;i<=ini_col_num-1;i++)
    for(j=0;j<=ini_col_num-1;j++)
      if (bloco[i][j].vida && interseccao(t_x,t_y,t_alt,t_comp,bloco[i][j].x,  
      bloco[i][j].y,lado,lado) ) {
        bloco[i][j].vida=0;
        coluna_ini[j]--;
        ini_vivos--;
        for(k=ini_col_num-1;k>=0;k--) { //busca para linha_toca_solo
          for(l=0;l<=ini_col_num-1;l++) 
            if(bloco[k][l].vida) { 
              linha_toca_solo=k; 
              quebrou_dentro=1; 
              break; 
           }
          if (quebrou_dentro)
            break;
        }
               
        bloco_tempo-=tx_dec_bloco_tempo;
        pontuacao+=100; 
        atirado=0;
        if (ini_vivos==0) {
          ganha=1;
          vitoria(); //encerramento
        }
        return;
      }
  //acerto nos escudos
  for(i=0;i<=2;i++)
    for(j=0;j<=6;j++)
      if ( (esc[i][j].vida>0)  && (interseccao   (t_x,t_y,t_alt,t_comp,esc[i][j].x,           
        esc[i][j].y,ladinho_esc,ladinho_esc)) ) {
          esc[i][j].vida-=0.4;
          atirado=0;
          return;
      } 

  //acerto no ovni
  if(interseccao(t_x,t_y,t_alt,t_comp,ovni_x,           
        ovni_y,2*ovni_elipse_ry,2*ovni_elipse_rx)) {  
    ovni_passando=0;
    atirado=0;
    pontuacao+=2000;
    glutTimerFunc(rand()%abs(ovni_rand_tempo),ativa_ovni,0);
    ovni_x=comp;
    ovni_y=margem3;
    return;
  }

if (t_y+t_alt<=-t_alt) 
  return;

glutTimerFunc(t_tempo,verifica_acerto,0);  


}


void verifica_acerto_ini(int coluna) {

int i,j;

if(atirado_ini[coluna]) {
  //acerto nos escudos
    for(i=0;i<=2;i++)
      for(j=0;j<=6;j++)
        if ( (esc[i][j].vida>0)  && (interseccao(esc[i][j].x,           
        esc[i][j].y,ladinho_esc,ladinho_esc,t_ini_x[coluna],t_ini_y[coluna],t_alt,t_comp)) ) {
           esc[i][j].vida-=0.4;
           glutPostRedisplay(); //ATENCAO
           atirado_ini[coluna]=0;
           colunas_atirando--;
           return;
        }
    //acerto na nave
    if (interseccao(n_x,n_y,lado,lado,t_ini_x[coluna],t_ini_y[coluna],t_alt,t_comp)) {
      n_vidas--;
      morto=1;
      if (n_vidas==0)
        perde=1;
      glutPostRedisplay();
      atirado_ini[coluna]=0;
      colunas_atirando--;
      glutTimerFunc(ressucita_tempo,ressucita,0);
      return;
    }
}

if (t_ini_y[coluna]>=alt) 
  return;

glutTimerFunc(t_tempo_ini,verifica_acerto_ini,coluna);

}


void ressucita() {
  
 morto=0;
 n_x=comp/2-lado/2;
 n_y=alt-margem-lado; 
 glutPostRedisplay();

}


void sorteia_cenario() {

int i,rx,ry;

for(i=0;i<estrelas;i++) {
  ceu_x[i]=rand()%abs(comp);
  ceu_y[i]=rand()%abs(alt);
}


}


void movimenta_cenario() {

int i;  

if (ganha && nave_em_tela) tx_acel_cenario=20;

if (ganha && !nave_em_tela) tx_acel_cenario=5;

for(i=0;i<estrelas;i++) {
  ceu_x[i]+=0;
  ceu_y[i]+=tx_acel_cenario;
  ceu_x[i]%=abs(comp);
  ceu_y[i]%=abs(alt);
}

glutPostRedisplay();

glutTimerFunc(cenario_tempo,movimenta_cenario,0);

 
}


void vitoria() {

n_y-=9;
glutPostRedisplay();

if (n_y+lado<0) nave_em_tela=0;

glutTimerFunc(10,vitoria,0);

}


//DESENHOS


void circulo(int raio, int xc, int yc) {
float i;

glBegin(GL_POLYGON);
for (i=0.0;i<2*M_PI;i+= (2.0*M_PI/60) )
glVertex2f(xc+raio*cos(i),yc+raio*sin(i));
glEnd();

} 


void elipse(int raio, int xc, int yc, int ctex, int ctey) {
float i;

glBegin(GL_POLYGON);
for (i=0.0;i<2*M_PI;i+= (2.0*M_PI/60) )
glVertex2f(xc+ctex*raio*cos(i),yc+ctey*raio*sin(i));
glEnd();

} 


void nave (int nx, int ny, float ladoo) {


glColor3f(1,1,0);
glBegin(GL_TRIANGLES);
glVertex2f(nx+ladoo/2,ny);
glVertex2f(nx,ny+ladoo);
glVertex2f(nx+ladoo,ny+ladoo);
glEnd();


}


void tiro(int tx , int ty) {

glColor3f(0.2,0.5,0.6);
glBegin(GL_QUADS);
glVertex2f(tx,ty);
glVertex2f(tx+t_comp,ty);
glVertex2f(tx+t_comp,ty+t_alt);
glVertex2f(tx,ty+t_alt);
glEnd();


}


void inimigo(int x, int y, int tipo, int estado) {  
int i,j;

if (tipo==1) { //X
  glColor3f(0.1,0.4,0.9);
  glBegin(GL_LINES);
  if (estado) 
    for (i=0;i<4;i++) {
      glVertex2f(x+margem2+i,y+margem2);
      glVertex2f(x+lado-margem2+i,y+lado-margem2);
      glVertex2f(x+margem2+i,y+lado-margem2);
      glVertex2f(x+lado-margem2+i,y+margem2);
    }
  else //estado=0
    for (i=0;i<4;i++) {
      glVertex2f(x+margem2+i+mov_X,y+margem2);
      glVertex2f(x+lado-margem2+i-mov_X,y+lado-margem2);
      glVertex2f(x+margem2+i+mov_X,y+lado-margem2);
      glVertex2f(x+lado-margem2+i-mov_X,y+margem2);
    }
  glEnd();
}
else { //O
  if (estado) {
    glColor3f(1,0,0);
    circulo(lado/2-margem2,x+lado/2,y+lado/2);
    glColor3f(0,0,0);
    circulo(lado/2-margem2-4,x+lado/2,y+lado/2);
  } 
  else { //estado=0
    glColor3f(1,0,0);
    circulo(lado/2-margem2-1,x+lado/2,y+lado/2);
    glColor3f(0,0,0);
    circulo(lado/2-margem2-4,x+lado/2,y+lado/2);
  }   
}

}


void escudos() {

int i,inverte,translada;

//Escudos: 7 partes

for(i=0;i<=2;i++) {

  if (i==1) 
    inverte=translada=0;
  if ((i==0) || (i==2))
    inverte=1;
  if (i==0)
    translada=-1;
  if (i==2)
    translada=1;

if(esc[i][0].vida>0){
 glColor3f(0.9,0.9,0.9);
glBegin(GL_TRIANGLES);
 glColor3f(esc[i][0].vida,esc[i][0].vida,esc[i][0].vida);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc))));
 glColor3f(0.9,0.9,0.9);
glVertex2f(comp/2-ladinho_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc))));
glVertex2f(comp/2+ladinho_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc))));
glEnd();
}

if(esc[i][1].vida>0){
 glColor3f(0.9,0.9,0.9);
glBegin(GL_QUADS);
glVertex2f(comp/2-ladinho_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc)))); 
 glColor3f(esc[i][1].vida,esc[i][1].vida,esc[i][1].vida);
glVertex2f(comp/2-ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
 glColor3f(0.9,0.9,0.9);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc))));
glEnd();
}

if(esc[i][2].vida>0){
 glColor3f(0.9,0.9,0.9);
glBegin(GL_QUADS);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc))));
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
 glColor3f(esc[i][2].vida,esc[i][2].vida,esc[i][2].vida);
glVertex2f(comp/2+ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
glVertex2f(comp/2+ladinho_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+ladinho_esc))));
glEnd();
}

if(esc[i][3].vida>0){
 glColor3f(0.9,0.9,0.9);
glBegin(GL_TRIANGLES);
glVertex2f(comp/2-ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
 glColor3f(esc[i][3].vida,esc[i][3].vida,esc[i][3].vida);
glVertex2f(comp/2-lado_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
 glColor3f(0.9,0.9,0.9);
glVertex2f(comp/2-ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
glEnd();
}

if(esc[i][4].vida>0){
 glColor3f(0.9,0.9,0.9);
glBegin(GL_QUADS);
glVertex2f(comp/2-ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
 glColor3f(esc[i][4].vida,esc[i][4].vida,esc[i][4].vida);
glVertex2f(comp/2-ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
 glColor3f(0.9,0.9,0.9);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));


glEnd();
}

if(esc[i][5].vida>0){
glColor3f(0.9,0.9,0.9);
glBegin(GL_QUADS);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
 glColor3f(esc[i][5].vida,esc[i][5].vida,esc[i][5].vida);
glVertex2f(comp/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
 glColor3f(0.9,0.9,0.9);
glVertex2f(comp/2+ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
 glColor3f(esc[i][5].vida,esc[i][5].vida,esc[i][5].vida);
glVertex2f(comp/2+ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
glEnd();
}

if(esc[i][6].vida>0){
glColor3f(0.9,0.9,0.9);
glBegin(GL_TRIANGLES);
glVertex2f(comp/2+ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+2*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+2*ladinho_esc))));
glVertex2f(comp/2+ladinho_esc+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
 glColor3f(esc[i][6].vida,esc[i][6].vida,esc[i][6].vida);
glVertex2f(comp/2+lado_esc/2+(translada*espaco_esc),alt-4*margem-lado-lado_esc+3*ladinho_esc+(inverte*2*((alt-4*margem-lado-lado_esc/2)-(alt-4*margem-lado-lado_esc+3*ladinho_esc))));
glEnd();
}

}


}


void cenario() {

int i;

glDisable(GL_LINE_SMOOTH);

glColor3f(1,1,0);
for(i=0;i<=estrelas;i++) {
  glBegin(GL_LINES);
  glVertex2f(ceu_x[i]-0.5,ceu_y[i]);
  glVertex2f(ceu_x[i]+0.5,ceu_y[i]);
  glVertex2f(ceu_x[i],ceu_y[i]+0.5);
  glVertex2f(ceu_x[i],ceu_y[i]+0.5);
  glEnd();
}

} 


void ovni() {

int i;


for(i=0;i<=90;i++) {
  glColor3f(0.1+cor*i*0.01 , 0, 0.1-cor*i*0.01);
  elipse(ovni_raio-i*0.1,ovni_x+ovni_elipse_rx,ovni_y                           +cte_y*ovni_elipse_ry,cte_x,cte_y);
}

}


void pontos() {

glColor3f(1,1,0);
sprintf(p,"%d",pontuacao);
/*gfText(p,margem_x_lp,margem_y_lp,letra_tam,letra_esp);*/

}

void vidas() {

if (perde) return;

switch (n_vidas) {

  case(3):
    nave(margem_x_lv,margem_y_lv,lado*cte_vidas);
    nave(margem_x_lv+lado+margem,margem_y_lv,lado*cte_vidas);
    nave(margem_x_lv+2*(lado+margem),margem_y_lv,lado*cte_vidas);
  break;
  case(2):
    nave(margem_x_lv,margem_y_lv,lado*cte_vidas);
    nave(margem_x_lv+lado+margem,margem_y_lv,lado*cte_vidas);
  break;
  case(1):
    nave(margem_x_lv,margem_y_lv,lado*cte_vidas);
  break;
}

}

//DISPLAY

void display() {

int i,j;

glClear(GL_COLOR_BUFFER_BIT);
glDisable(GL_LINE_SMOOTH);


cenario();

pontos();
vidas();

/*if(ganha) gfText("Victory",comp/2-70,alt/2,1.3*letra_tam,letra_esp);*/
/*if(perde && !ganha) gfText("Game Over",comp/2-90,alt/2,1.3*letra_tam,letra_esp);*/
if(ganha) displayText(170,70, 100, 100, 100,"Victory");
if(perde && !ganha) displayText(140,70, 100, 100, 100,"Game Over");

if(ovni_passando) ovni();

if (!iniciado) { 
  iniciado=1;
  inicia();
  movimenta_cenario();
  glutTimerFunc(estado_tempo,muda_estado,0); 
  glutTimerFunc(bloco_tempo,move_bloco,0);
  glutTimerFunc(atira_ini_tempo,atira_ini,0);
  glutTimerFunc(rand()%abs(ovni_rand_tempo),ativa_ovni,0);
}

if ((!morto) && (n_vidas>=0) && (!perde))
  nave(n_x,n_y,lado);

escudos();

for(i=0;i<=ini_col_num-1;i++)
  if(atirado_ini[i])
    tiro(t_ini_x[i],t_ini_y[i]);

if (atirado) tiro(t_x,t_y);

if (!perde)
  for(i=0;i<=ini_col_num-1;i++)
    for(j=0;j<=ini_col_num-1;j++)
      if (bloco[i][j].vida)
        inimigo(bloco[i][j].x,bloco[i][j].y,(i)%2,estado);   

glutSwapBuffers();

}


//TECLADO

void setas(int tec, int x, int y) {

if (morto || perde) return;

if (n_x-margem>0) 
  if (tec==GLUT_KEY_LEFT)
    n_x-=7;

if (n_x+margem<comp-lado)
  if (tec==GLUT_KEY_RIGHT)
    n_x+=7;  

glutPostRedisplay();


}


void barra(unsigned char tecla,int x, int y){

if (morto || perde || ganha) return;

if (tecla==' ')
  if (!atirado) {
    t_x=n_x+lado/2-t_comp/2;
    t_y=n_y-t_alt; 
    atirado=1; 
    verifica_acerto();
    glutPostRedisplay();
    glutTimerFunc(t_tempo,move_tiro,0);
  }


}


//MAIN
int main(int argc, char **argv) {
  
  int i;

  srand(time(0)); //randomizar cenário e colunas

  sorteia_cenario();

  n_x=comp/2-lado/2;
  n_y=alt-margem-lado;
  ladinho_esc=lado_esc/3;
  topo_esc=alt-4*margem-lado-lado_esc;
  
  margem3=3*margem_y_lp+letra_tam;
  
  ini_vivos=ini_num;
  ini_col_num=sqrt(ini_num);
  for(i=0;i<=ini_col_num-1;i++) {
    coluna_ini[i]=ini_col_num;
    atirado_ini[i]=0;
  }
    
  ovni_x=comp;
  ovni_y=margem3;
  
  ovni_elipse_rx=cte_x*ovni_raio;
  ovni_elipse_ry=cte_y*ovni_raio;
 
  rumo_lim=(comp-(lado*ini_col_num+espaco_ini*(ini_col_num-1))-margem)/veloc_bloco-1;

  linha_toca_solo=ini_col_num-1;

  margem_x_lp=(0.25)*comp;
  margem_x_lv=(0.55)*comp;

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
  glutCreateWindow("Pseudo-Space Invaders"); 
  glutReshapeWindow(comp,alt);
  glViewport(0.0,0.0,comp,alt);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0,comp,alt,0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glutDisplayFunc(display);
  glutSpecialFunc(setas);
  glutKeyboardFunc(barra);
  glutMainLoop();

  return(0);


}








