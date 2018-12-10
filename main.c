#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "conio.h"
// #include "windows.h"

typedef enum{false,true} bool;

char exitLab = 'O';
char player = 64;
char wall = '|';

int mapCounter = 0;
int minDifficulty = 0;
int windowSize = 20;
bool placedExit = false;

int strToInt(char *str);
void printMap(int **map,int size);
void printMatMap(int **map,int size);
void printMapCentered(int **map,int size,int width);
int loadMap(int **map, char *path,int mapDim);
int readDimension(char *path);
int genMapRecursive(int** map,int size,int x, int y,int** prec);
int **generate_map(int size);
void saveMap(char path[],int** map, int size);
void movePlayer(int** map, int size);
int* getPlayerPosition(int** map, int size);
void printWin();
void menu();
void printAsciiTable();

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	char mapFilePath[100]="map.gm";

	int **map,i;
 	int mapDim=-1;

	char chosen;
	do
	{
		printf("\n");
		printf("Choose a task:\n");
		printf("1 - Play\n");
		printf("2 - Generate a new map\n");
		printf("3 - Load a map from file\n");
		printf("4 - Save current map and progress to file\n");
		printf("5 - Reset current level\n");
		printf("6 - Change width size\n");
		printf("0 - Exit\n");
		scanf("%c%*c",&chosen);
		fflush(stdin);

		int tmpDim=0;
		switch(chosen)
		{
			//change width size
			case '6':
			if(mapDim==-1)
				printf("No map loaded! Generate or load a new map first.\n");
			else
			{
				do
				{
					printf("Insert window size: (max: %d): ",mapDim);
					scanf("%d",&windowSize);
					fflush(stdin);
					if(windowSize>mapDim || windowSize<10)
						printf("Invalid size!\n");
				}while(windowSize>mapDim || windowSize<10);
					printf("Window size changed.\n");
			}
			break;
			
			//generate new map
			case '2':
				do
				{
					printf("Choose the size of the map: ");
					scanf("%d%*c",&tmpDim);
					fflush(stdin);

					if(tmpDim>=4 && tmpDim<=230)
					{
						if(tmpDim%2==0)
							tmpDim++;
						
						mapDim=tmpDim;

						map = calloc(mapDim,sizeof(int*));
					 	for (i=0; i<mapDim; i++)
					 			map[i] = calloc(mapDim,sizeof(int));

						map = generate_map(mapDim);

						printMap(map,mapDim);	
					}
					else
					{
						printf("Invalid size!\n");
					}
				}while(tmpDim<4 || tmpDim >230);
				
				
			break;

			//load map from file
			case '3':
				printf("Loading map from file:%s ...\n",mapFilePath);

				mapDim=readDimension(mapFilePath);

				map = calloc(mapDim,sizeof(int*));
			 	for (i=0; i<mapDim; i++)
			 			map[i] = calloc(mapDim,sizeof(int));

				loadMap(map,mapFilePath,mapDim);

				printMap(map,mapDim);
			break;

			//save current map to file
			case '4':
			if(mapDim!=-1)
			{
				printf("Saving current map to %s ...\n",mapFilePath);
				saveMap(mapFilePath,map,mapDim);
				printf("Map saved.\n");
			}
			else
			{
				printf("No map loaded! Generate or load a new map first.\n");
			}
			break;

			//play
			case '1':
				if(mapDim!=-1)
				{
					printf("Starting a new game...\n");
					movePlayer(map,mapDim);
					printf("Game finished!\n");
				}
				else
				{
					printf("No map loaded! Generate or load a new map first.\n");
				}
			break;

			case '5':
				if(mapDim!=-1)
				{
					int* pos = getPlayerPosition(map,mapDim);
					map[1][1]=2;
					map[pos[0]][pos[1]]=0;
					printf("Reset successful.\n");
				}
				else
				{
					printf("No map loaded! Generate or load a new map first.\n");
				}
			break;
		}

	}while(chosen!='0');

	return 0;
}


void printMapCentered(int **map,int size,int width)
{
	// spazio vuoto → 0
	// muro → 1	
	// giocatore → 2
	// uscita labirinto → 3
	int *pos = getPlayerPosition(map,size);
	int xp=pos[0];
	int yp=pos[1];

	int x,y;
	for(x=-width/2;x<width/2;x++)
	{
		for(y=-width/2;y<width/2;y++)
		{
			if(xp+x>=0 && xp+x<size && yp+y>=0 && yp+y<size)
			{
				switch(map[x+xp][y+yp])
				{
					case 1:
					printf("%c",wall);
					break;

					case 2:
					printf("%c",player);
					break;

					case 3:
					printf("%c",exitLab);
					break;

					case 0:
					printf(" ");
					break;
				}	
			}	
		}
		printf("\n");
	}
}

void printMatMap(int **map,int size)
{
	int x,y;
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			printf("%d-",map[x][y] );
		}
		printf("\n");
	}
}

void printMap(int **map,int size)
{
	// spazio vuoto → 0
	// muro → 1	
	// giocatore → 2
	// uscita labirinto → 3

	int x,y;
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			switch(map[x][y])
			{
				case 1:
				printf("%c",wall);
				break;

				case 2:
				printf("%c",player);
				break;

				case 3:
				printf("%c",exitLab);
				break;

				case 0:
				printf(" ");
				break;
			}
		}
		printf("\n");
	}
}

int strToInt(char *str)
{
	int res=0;
	int i;
	for(i=0;i<strlen(str);i++)
	{
		res+=(str[i]-'0')*pow(10,strlen(str)-1-i);
	}
	return res;
}


int readDimension(char *path)
{
	FILE *f = fopen(path,"r");

	if(f==NULL)
	{
		printf("Error opening file %s!\n",path );
		return -1;
	}

	char dimstr[30];
	int buf;
	int i=0;
	
	while(1)
	{
		buf=fgetc(f);

		if(feof(f))
		{
			printf("Error loading dimension of the map!\n");
			return -1;
		}
		if(buf!='\n')
		{	
			dimstr[i]=(char)buf;
			i++;
		}
		else
		{
			dimstr[i]='\0';
			break;
		}
	}
	
	fclose(f);
	return strToInt(dimstr);
}

//1 if success, 0 if error
int loadMap(int **map, char *path,int mapDim)
{
	FILE *f = fopen(path,"r");

	if(f==NULL)
	{
		printf("Error opening file %s!\n",path );
		return 0;
	}

	//fseek to the location of the first element of map
	int buf;
	while(1)
	{
		buf=fgetc(f);
		
		if(buf=='\n')
			break;

		if(feof(f))
		{
			printf("Error loading dimension of the map!\n");
			return 0;			
		}
	}

	//reading map
	int x,y;
	for(x=0;x<mapDim;x++)
	{
		for(y=0;y<mapDim;y++)
		{
			//go on reading until it finds a good characte (no spaces or \n)
			int buf=-1;
			while( buf=='\n' || buf==' ' ||buf==-1)
				buf = fgetc(f);	
			
			map[x][y]=(buf-'0');

		}
	}

	fclose(f);
	return 1;
}

int **generate_map(int size)
{
	int **map,i;
	int **prec;

	printf("Generating a new map...\n");

	do
	{
		map = calloc(size,sizeof(int*));
		for (i=0; i<size; i++)
				map[i] = calloc(size,sizeof(int));

		
		prec = calloc(size,sizeof(int*));
		for (i=0; i<size; i++)
				prec[i] = calloc(size,sizeof(int));

		int x,y;
		for(x=0;x<size;x++)
		{
			for(y=0;y<size;y++)
			{
				map[x][y]=1;
				prec[x][y]=0;
			}
		}

		mapCounter=0;
		placedExit=false;
		genMapRecursive(map,size,1,1,prec);

	} while(!placedExit);
	return map;
}

//>0 if success, 0 if failed
int genMapRecursive(int** map,int size, int x, int y, int** prec)
{
	//put black space in current cell
	map[y][x]=0;
	int i,j;

	//check if there is an odd non-zero and non-diagonally adjacent to cell x-y
	int rx=-1,ry=-1;
	int existAdjacent=0;
	for(i=-2;i<3;i+=2)
	{
		for(j=-2;j<3;j+=2)
		{
			//do if odd
			if(i%2==0 && j%2 ==0)
			{
				//do if not diagonally adjacent and < size of map
				if(!(x+i<0 || x+i>=size || y+j<0 || y+j>=size) && (x+i==x || y+j==y) && (x+i!=x || y+j!=y))
				{
					//if not already checked, consider this adjacent cell
					if(prec[y+j][x+i]!=1)
					{
						//randomly consider or skip current adjacent cell found
						//super fake random
						 if(rand()%1000 / 1000.0<0.7)
						 {
						 	//connect the adjacent found and the current cell with a blank space (up,down, left or right)

						 	rx=x+i;
							ry=y+j;

							map[ry][rx]=0;
							prec[ry][rx]=1;

							if(ry==y)
							{
								if(rx<x)
								{
									//connect up
									prec[y][rx+1]=1;
									map[y][rx+1]=0;
								}
								else
								{
									//connect down
									prec[y][rx-1]=1;
									map[y][rx-1]=0;
								}

							}
							else
							{
								if(ry>y)
								{
									//connect left
									prec[ry-1][x]=1;
									map[ry-1][x]=0;
								}
								else
								{
									//connect right
									prec[ry+1][x]=1;
									map[ry+1][x]=0;
								}
							}
									mapCounter+=2;	

							//repeat for new adjacent found
							genMapRecursive(map,size,rx,ry,prec);

							existAdjacent = 1;
						}
					}
				}
			}
		}
	}
	//if no adjacent exists for current cell, generation is finished
	if(existAdjacent==0)
	{
		//map generation finished, if has a cycle return 0 (failure), else place exit and player and return 1 (success)
		if(x==1&&x==1)
			return 0;
		else
		{
			//place player
			map[1][1]=2;
			if(!placedExit)
			{
				//randomly place exit here or skip
				if(rand()%1000 / 1000.0<0.01)
				{
					map[y][x]=3;
					placedExit = true;
				}
			}
			return mapCounter;
		}
	}
	
}

void saveMap(char path[],int **map, int size)
{
	FILE *f = fopen(path,"w");

	if(f==NULL)
	{
		printf("Error opening file %s!\n",path);
		return;
	}

	char tmp[30];
	sprintf(tmp,"%d\n",size);
	fputs(tmp,f);

	int x,y;
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			sprintf(tmp,"%d",map[x][y]);
			fputs(tmp,f);
		
			if(y!=size-1)
				fputs(" ",f);
		}

		if(x!=size-1)
			fputs("\n",f);
	}
	fclose(f);
}

int* getPlayerPosition(int** map, int size)
{
	int* pos=malloc(sizeof(int)*2);

	int x,y;
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			if(map[x][y]==2)
			{
				*pos=x;
				*(pos+1)=y;
			}
		}
	}

	return pos;
}

void movePlayer(int** map, int size)
{
	bool toPrint=true;	
	char chosen;
	do
	{
		if(toPrint)
		{
			printf("\n\n");
			printMapCentered(map,size,windowSize);
			printf("Choose your next move: (wasd - move, q - quit):\n");

		}
		int* player = getPlayerPosition(map,size);

		int px=player[0];
		int py=player[1];
		int nx=0,ny=0;

		//using getch so you dont need to press enter at every move; it's a non standard input function
		// scanf("%c%*c",&chosen);
		chosen = getch();
		fflush(stdin);

		switch(chosen)
		{
			case 'A':
			case 'a':
			ny=-1;
			break;
			
			case 'D':
			case 'd':
			ny+=1;
			break;

			case 'W':
			case 'w':
			nx=-1;
			break;

			case 'S':
			case 's':
			nx=+1;
			break;
		}

		if(!(nx==0 && ny==0) && !(px+nx<0 || px+nx>size-1 || py+ny<0 || py+ny>size-1))
		{
			if(map[px+nx][py+ny]==0)
			{
				map[px+nx][py+ny]=2;
				map[px][py]=0;
				toPrint=true;
			}
			else
			{
				if(map[px+nx][py+ny]==3)
				{
					printWin();
					chosen='q';
				}
				else
				{
					if(chosen!='q' && chosen!='Q')
					{
						toPrint=false;
					}
				}
			}
		}
		else
		{
			toPrint=false;
		}
	} while(chosen!='q' && chosen!='Q');
}

void printWin()
{
	printf("\n\n");
	printf("---------------------------------------\n");
	printf("\t\tYOU WON!\n");
	printf("---------------------------------------\n");
	printf("\n\n");
}

void printAsciiTable()
{
	int i;
	for(i=0;i<256;i++)
	{
		printf("%c\tcode: %d\n",(char)i,i);
	}
}