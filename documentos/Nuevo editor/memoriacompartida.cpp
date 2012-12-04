#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
		int shmid; 
		int *variable;
		key_t llave;
		
		llave = ftok("kinectTool", 'k');
		
		if((shmid = shmget(llave,  sizeof(int)*8, IPC_CREAT | 0600)) == -1)
		{
				perror("Error en shmget");
				exit(-1);
		}
		
		if((variable = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))
		{
				perror("Fallo shmat");
				exit(-1);
		}
		while(1)
		{
			char letra;
			scanf("%c", &letra);
			
			switch(letra)
			{
					case 't':
						shmctl(shmid, IPC_RMID, 0);
						exit(0);
					case 'v':
						printf("%d,%d,%d,%d,%d", variable[0],variable[1],variable[2],variable[3], variable[4]);
						break;
					case 'm':
						scanf("%d,%d,%d,%d,%d", &variable[0],&variable[1],&variable[2],&variable[3], &variable[4]);
						break;
			}
			
		}
}
