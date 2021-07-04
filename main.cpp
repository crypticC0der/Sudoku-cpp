#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
using namespace std;
/*
void nextGenerationSquare(int x1, int y1,float* rgb)
{
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glBegin(GL_QUADS);
	float xd = 1.0 / (LENGTH+BUFFER);
	float yd = 1.0 / (HEIGHT+BUFFER);
	float xf=x1- (LENGTH+BUFFER/2.0)/2.0;
	float yf=y1- (HEIGHT+BUFFER/2.0)/2.0;
	xf = xf/(LENGTH/2.0 + BUFFER/2.0) +xd*(BUFFER)/2.0;
	yf = yf/(HEIGHT/2.0 + BUFFER/2.0) +yd*(BUFFER)/2.0;
	xf += xd;
	yf += yd;
	xd=xd*SCALE;
	yd=yd*SCALE;
    glVertex2d(xf + xd, yf + yd);
    glVertex2d(xf + xd, yf - yd);
    glVertex2d(xf - xd, yf - yd);
    glVertex2d(xf - xd, yf + yd);

    glEnd();
}
*/

const int gridUnit = 3;
int gridLen;
int gridSize;
int* grid;

void drawString(float x, float y, char *string) {
	glRasterPos2f(x,y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);  // Updates the position
	}
}

void DrawGrid(){
	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_QUADS);
	float halfSq = (float)(2)/(gridLen);
	for(int i=0;i<gridLen;i++){
		for(int j=i%2;j<gridLen;j+=2){
			float a = (float)(i<<1)/gridLen -1;
			float b = (float)(j<<1)/gridLen -1;
			glVertex2d(a,b);
			glVertex2d(a+halfSq,b);
			glVertex2d(a+halfSq,b+halfSq);
			glVertex2d(a,b+halfSq);
		}
	}
	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_QUADS);

	for(int i=1;i<gridUnit;i++){
		float k = (float)(i<<1)/gridUnit -1;
		glVertex2d(1,k+0.01);
		glVertex2d(-1,k+0.01);
		glVertex2d(-1,k-0.01);
		glVertex2d(1,k-0.01);
		glVertex2d(k+0.01,1);
		glVertex2d(k+0.01,-1);
		glVertex2d(k-0.01,-1);
		glVertex2d(k-0.01,1);
	}

	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	for(int i =-gridLen+2;i<gridLen;i+=2){
		float k = (float)(i)/gridLen;
		glVertex2d(1,k);
		glVertex2d(-1,k);
		glVertex2d(k,1);
	  	glVertex2d(k,-1);
	}
	glEnd();
	for(int i=0;i<gridLen;i++){
		for(int j=0;j<gridLen;j++){
			glRasterPos2f((float)(i*2 +1)/gridLen -1,(float)(j*2 +1)/gridLen -1);
			int gval = grid[i*gridLen+j];
			
			int high=1;
			while(gval/high>0){high*=10;}
			high/=10;
			while(gval!=0){
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,48+gval/high);
				gval=gval%high;
				high/=10;
			}
		}
	}
}
void disInit(){
	glClearColor(1, 1, 1, 0); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	DrawGrid();
	//nextGeneration();
	glFlush();
}



void run(){
}

bool numberAlreadyThere(int x,int y,int n,int* gridL){
	bool good=true;
	int bx=x-x%3;
	int by=y-y%3;
	for(int i=0;i<gridUnit;i++){
		for(int j=0;j<gridUnit;j++){
			good&=n!=gridL[y*gridLen+(i*3 +j)];
			good&=n!=gridL[(i*3+j)*gridLen+x];
			good&=n!=gridL[(by+i)*gridLen + (bx+j)];
		}
	}
	return !good;
}

bool generateGrid(){
	grid = new int[gridSize];
	int wrong=10000;
	for(int i=0;i<gridLen;i++){
		for(int j=0;j<gridLen;j++){
			int n;
			do{
				n=rand()%gridLen +1;
				wrong--;
				if(wrong<0){return false;}
			}while(numberAlreadyThere(j,i,n,grid));
			grid[i*gridLen +j]=n;
			cout<<n<<" at "<<i<<" , "<<j<<endl;
		}
	}
	return true;
}

bool genPos(int* gridL,int** gridPos,int*gridposlen,bool*changed,int* complen){
	bool* posible=new bool[gridLen];
	*complen=0;
	int poslen=0;
	for(int i=0;i<gridLen;i++){
		for(int j=0;j<gridLen;j++){
			poslen=0;
			for(int n=0;n<gridLen&&gridL[i*gridLen +j]==0;n++){
				if(!numberAlreadyThere(j,i,n+1,gridL)){
					poslen++;
					posible[n]=true;
				}
			}
			if(poslen==0&&gridL[i*gridLen+j]==0){
				return false;
			}
			gridPos[i*gridLen +j]=new int[poslen];
			gridposlen[i*gridLen +j]=poslen;
			int k=0;
			for(int n=0;n<gridLen;n++){
				if(posible[n]){
					gridPos[i*gridLen +j][k]=n+1;
					k++;
				}
				posible[n]=false;
			}
			if(poslen==1){
				gridL[i*gridLen +j]=gridPos[i*gridLen +j][0];
				gridposlen[i*gridLen+j]=0;
				free(gridPos[i*gridLen +j]);
				*changed=true;
			}
			if(gridL[i*gridLen +j]!=0){(*complen)++;}
		}
	}
	//free(posible);
	return true;
}

bool solve(int* gridL){
	int** gridPos;
	int* gridposlen;
	gridPos=new int*[gridSize];
	gridposlen=new int[gridSize];
	bool* changed=new bool[1];
	int* complen = new int[1];
	*changed=true;

	cout << "1" << endl;
	//generate posibilites
	//cout<<"generating posses"<<endl;
	while(*changed!=0){
		*changed=false;
		if(!genPos(gridL,gridPos,gridposlen,changed,complen)){
			free(gridPos);
			free(gridposlen);
			free(complen);
			free(gridL);
			free(changed);
			return false;
		}
		if((*complen)==gridSize){grid=gridL;return true;}
	}

	cout << "2" << endl;
	//guess until right lmao
	bool failing=false;
	int n_indx=0;
	while(!failing){
		int* newGrid = new int[gridSize];
		unsigned int mini=-1;
		int minlen=gridLen+5;
		for(int i=0;i<gridSize;i++){
			if(gridposlen[i]<=minlen && gridposlen[i]!=0&&gridL[i]==0){
				minlen=gridposlen[i];
				mini=i;
			}
			newGrid[i]=gridL[i];
		}
		int n=gridPos[mini][n_indx];
		cout << "testing for n="<<n<<" at position "<< mini/gridLen<< " , " <<mini%gridLen<<" with "<<1/(float)(minlen)<<" posibility"<<endl;
		newGrid[mini]=n;
		cout<<"a"<<endl;
		if(solve(newGrid)){
			//win
			return true;
		}else{
			//loose
			n_indx++;
			failing=n_indx>=minlen;
			cout << "failed test" << endl;
		}
	}
	return false;
}

int main(int argc, char** argv) {
	srand (time(NULL));
	gridLen=gridUnit*gridUnit;
	gridSize=gridLen*gridLen;
	//while(!generateGrid()){}
	int* cgrid=new int[gridSize];
	if(!solve(cgrid)){
		cout<<"help"<<endl;
	}
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(600,600);   // Set the window's initial width & height
    glutCreateWindow("Sudoku"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
