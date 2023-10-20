#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <omp.h>
#define	NDIM 2
#define Sqr(x) ((x)*(x))

int main(int argc, char **argv){

int n, nx, ny, ngrids_X, ngrids_Y;
double first, second, x, y, dx, dy, *psi, *psinew, *rho, *secondderive;
double pi = 4.0*atan(1);


//to do in n by n grids we need to assign n+1 by n+1 

ngrids_X = 513;
ngrids_Y = 513;

psi = (double*) malloc((ngrids_X*ngrids_Y+1)*sizeof(double));
rho = (double*) malloc((ngrids_X*ngrids_Y+1)*sizeof(double));
secondderive = (double*) malloc((ngrids_X*ngrids_Y+1)*sizeof(double));
psinew = (double*) malloc((ngrids_X*ngrids_Y+1)*sizeof(double));

dx= ((2.0)/(ngrids_X-1));
dy = ((2.0)/(ngrids_Y-1));

x = 0 ;
y = -1.0;

double lambda = (1.0/(2*pi));
//Initialisation of rho

FILE *fp;
fp = fopen("rhoinitial","w");

for (ny=1;ny<=ngrids_Y;ny++){
  if(ny!=1)
  y += dy; 
  for (nx=1;nx<=ngrids_X;nx++){
  if(nx !=1)
  x += dx;

  n = (ny-1)*ngrids_X + nx; 
  
  rho[n] = (1-Sqr(0.3))/(lambda*Sqr(cosh(y/lambda) + (0.3)*cos(x/lambda)));
  if(nx == 50)
  fprintf(fp,"%lf\t %lf\t %lf\n",rho[n],x,y); 
  }
}
fclose(fp);

//Initialisation psi
for (ny=1;ny<=ngrids_Y;ny++){
  for (nx=1;nx<=ngrids_X;nx++){
  n = (ny-1)*ngrids_X + nx;
 
  psi[n] = 0.0;
  psinew[n] = 0.0;
  }
}

double sorcoeff = 1.99;
int  t_end;

t_end = 10000;

double st = omp_get_wtime();

//Calculation of potential

//for(t=1;t<=t_end;t++)

int iter = 0;
double error = 1.0;
double tol = 1.0e-12;
while(error>tol && iter < t_end)
{
double sumerr = 0.0;
x = 0 ;
y = -1.0;




//iteration of black nods

#pragma acc parallel loop gang reduction(+:sumerr) copy(psinew[1:ngrids_X*ngrids_Y], sumerr) copyin(psi[1:ngrids_X*ngrids_Y], rho[1:ngrids_X*ngrids_Y])
for (ny=2;ny<=ngrids_Y-1;ny++){
#pragma acc loop vector
  for (nx=((ny%2)+1);nx<=ngrids_X;nx+=2){
/*int  dummyx = nx;
  if(ny%2 != 0){
  nx = nx+1;
  if(dummyx == ngrids_X)
  continue;
  }*/
  
  

  n = (ny-1)*ngrids_X + nx;

  if(nx==1){
  first = psi[(ny-1)*ngrids_X+nx+1] + psi[(ny-1)*ngrids_X + ngrids_X];
  }else if(nx==(ngrids_X)){
  first = psi[(ny-1)*ngrids_X +1] + psi[(ny-1)*ngrids_X+nx-1];
  }else{
  first = psi[(ny-1)*ngrids_X+nx+1] + psi[(ny-1)*ngrids_X+nx-1];
  }

  
  second = psi[(ny)*ngrids_X+nx] + psi[(ny-2)*ngrids_X+nx];
  psinew[n] = (1 - sorcoeff)*psi[n] + sorcoeff*0.25*(first + second + Sqr(dx)*rho[n]);
  //psinew[n] =  0.25*(first + second + Sqr(dx)*rho[n]);
  //printf("black = %d %d %d\n",nx,ny,n);
  sumerr += Sqr(psinew[n] - psi[n]);
  
  }
 }





//iteration of red nods



#pragma acc parallel loop gang reduction(+:sumerr) copy(psinew[1:ngrids_X*ngrids_Y], sumerr) copyin(psi[1:ngrids_X*ngrids_Y], rho[1:ngrids_X*ngrids_Y])
for (ny=2;ny<=ngrids_Y-1;ny++){
  #pragma acc loop vector
  for (nx=(((ny+1)%2)+1);nx<=(ngrids_X);nx+=2){
  
/*******************************************
//when nx starts from 0 and ends at ngrids_X
  if(ny%2 !=0){
  nx = nx+1;
  }

  if(ny%2 ==0){
  nx = nx+2;
  if(nx == ngrids_X)
  continue;
  }*****
********************************************/

//When nx starts from 2 and ends at ngrids_X+1
//****************************************//
  /*if(ny%2 != 0){
  nx = nx-1;
  }
  
  if(ny%2 == 0 && dummyx == (ngrids_X+1))
  continue;*/
//***************************************//  


  n = (ny-1)*ngrids_X + nx;

  if(nx==1){
  first = psinew[(ny-1)*ngrids_X+nx+1] + psinew[(ny-1)*ngrids_X + ngrids_X];
  }else if(nx==(ngrids_X)){
  first = psinew[(ny-1)*ngrids_X +1] + psinew[(ny-1)*ngrids_X+nx-1];
  }else{
  first = psinew[(ny-1)*ngrids_X+nx+1] + psinew[(ny-1)*ngrids_X+nx-1];
  }

  
  second = psinew[(ny)*ngrids_X+nx] + psinew[(ny-2)*ngrids_X+nx];
  psinew[n] = (1 - sorcoeff)*psi[n] + sorcoeff*0.25*(first + second + Sqr(dx)*rho[n]);
  //psi[n] =  0.25*(first + second + Sqr(dx)*rho[n]);
  //printf("red = %d %d %d\n",nx,ny,n);
  sumerr += Sqr(psinew[n] - psi[n]);
  }
 }


#pragma acc parallel loop gang copyin(psinew[1:ngrids_X*ngrids_Y]) copyout(psi[1:ngrids_X*ngrids_Y])
for (ny=2;ny<=ngrids_Y-1;ny++){
#pragma acc loop vector
  for (nx=1;nx<=ngrids_X;nx++){

  n = (ny-1)*ngrids_X + nx;
  psi[n] = psinew[n]; 

  }
}

error = sqrt(sumerr/(ngrids_X*ngrids_Y));
printf("%lf\t%d\n",error,iter);
 iter++;
}  //end of time loop


double runtime = omp_get_wtime() - st;
 
printf(" total: %f s\n", runtime);
printf("%lf\t%d\n",error,iter);
//Printing of final psi

FILE *fppsi;
fppsi = fopen("final_psi","w");
for (ny=1;ny<=ngrids_Y;ny++){
  if(ny!=1)
  y += dy; 
  for (nx=1;nx<=ngrids_X;nx++){
  if(nx !=1)
  x += dx;

  n = (ny-1)*ngrids_X + nx;

  if(nx == 50)
  fprintf(fppsi,"%lf\t %lf\t %lf\t %d\t%d\t%d\n",psi[n], x, y, nx, ny, n);

  }
}
fclose(fppsi);


x = 0 ;
y = -1.0;

//Double derivative of psi
FILE *fp2;
fp2 = fopen("sderive","w");

for (ny=2;ny<=ngrids_Y-1;ny++){
  if(ny!=1)
  y += dy; 
  for (nx=2;nx<=ngrids_X-1;nx++){
  if(nx !=1)
  x += dx;

 n = (ny-1)*ngrids_X + nx;

 if(nx==1){
 first = psi[(ny-1)*ngrids_X+nx+1] - 2.0*psi[(ny-1)*ngrids_X+nx] + psi[(ny-1)*ngrids_X + ngrids_X];
 }else if(nx==(ngrids_X)){
 first = psi[(ny-1)*ngrids_X +1] - 2.0*psi[(ny-1)*ngrids_X+nx] + psi[(ny-1)*ngrids_X+nx-1];
 }else{
 first = psi[(ny-1)*ngrids_X+nx+1] - 2.0*psi[(ny-1)*ngrids_X+nx] + psi[(ny-1)*ngrids_X+nx-1];
 }

 if(ny==1){
 second = psi[ngrids_X+nx] - 2.0*psi[(ny-1)*ngrids_X+nx] ;
 }else if(ny==(ngrids_Y)){
 second =   - 2.0*psi[(ny-1)*ngrids_X+nx] + psi[(ngrids_Y-2)*ngrids_X+nx];
 }else{
 second = psi[(ny)*ngrids_X+nx] - 2.0*psi[(ny-1)*ngrids_X+nx] + psi[(ny-2)*ngrids_X+nx];
 }

secondderive[n] = (first/Sqr(dx) + second/Sqr(dy));
if(nx == 50)
fprintf(fp2,"%lf\t%lf\t%lf\n",-secondderive[n], x, y);
}}
fclose(fp2);

//Calculation of difference

x = 0 ;
y = -1.0;

double difference;

FILE *fp3;
fp3 = fopen("difference1","w");
for (ny=2;ny<=ngrids_Y-1;ny++){
  if(ny!=1)
  y += dy; 
  for (nx=2;nx<=ngrids_X-1;nx++){
  if(nx !=1)
  x += dx;
   
  n = (ny-1)*ngrids_X + nx;
  difference = fabs(secondderive[n]) - fabs(rho[n]);
  
  fprintf(fp3,"%lf\t %lf\t %lf\t %d\t %d\n", fabs(difference), x, y, nx, ny);
  }
 x = 0.0;
}
fclose(fp3);


free(psinew);
free(psi);
free(rho);
free(secondderive);
return 0;
}
