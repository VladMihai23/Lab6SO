#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/wait.h>

#define NRPROCESE 10

int ePrim(int x)
{
	int d;
	if (x < 2)
	{
		return 0;
	}
	for (d = 0; d * d <= x / 2; d++)
	{
		if (x % d == 0)
		{
			return 0;
		}
	}
	return 1;
}

int main()
{
	int a, b;//capetele intervalului crt
	int i, j;
	int pids[NRPROCESE];//id-urile proceselor create
	int pipes[2][2];//cele 2 pipe-uri
	int dimens = 0;//totalul de nr prime
	if (pipe(pipes[0]) == -1)
	{
		cout << "Eroare la crearea pipe-ului" << endl;
		return 1;
	}
	if (pipe(pipes[1]) == -1)
	{
		cout << "Eroare la crearea pipe-ului" << endl;
		return 1;
	}
	for (i = 0; i < NRPROCESE; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			cout << "Eroare la crearea procesului" << endl;
			return 2;
		}
		if (pids[i] == 0)//proces copil
		{
			//inchidem pipe-urile din care nu se citeste sau in care nu se scrie
			close(pipes[1][0]);
			close(pipes[0][1]);
			if (read(pipes[1][0], &a, sizeof(a)) == -1)
			{
				cout << "Eroare la citirea din pipe" << endl;
				return 3;
			}
			if (read(pipes[1][0], &b, sizeof(b)) == -1)
			{
				cout << "Eroare la citirea din pipe" << endl;
				return 3;
			}
			int nrPrime[250];
			int k = 0;
			for (j = a; j <= b; j++)
			{
				if (ePrim(j) == 1)
				{
					nrPrime[k] = j;
					k++;
					dimens++;
				}
			}
			if (write(pipes[1][1], nrPrime, k * sizeof(int)) == -1)
			{
				cout << "Eroare la scrierea in pipe" << endl;
				return 4;
			}
			//inchide pipe-urile in care a scris si din care a citit
			close(pipes[0][0]);
			close(pipes[1][1]);
		}
		else
		{
			//inchide pipe-urile in care nu a scris si din care nu a citit
			close(pipes[0][0]);
			close(pipes[1][1]);
			a = i * 1000 + 1;
			b = (i + 1) * 1000;
			if (write(pipes[0][1], &a, sizeof(int)) == -1)
			{
				cout << "Eroare la scrierea in pipe" << endl;
				return 4;
			}
			//inchide pipe-urile in care a scris si din care a citit
			close(pipes[0][0]);
			close(pipes[1][1]);
		}
		return 0;
	}
	int buffer[25000];
	if (read(pipes[1][0], buffer, sizeof(buffer)) == -1)
	{
		cout << "Eroare la citirea din pipe" << endl;
		return 3;
	}
	cout << "Numerele prime din [1, 10000] sunt: " << endl;
	for (i = 0; i < dimens; i++)
	{
		cout << buffer[i] << " ";
	}
	cout << endl << endl;
	//inchede pipe-urile
	close(pipes[0][0]);
	close(pipes[1][1]);
	close(pipes[0][1]);
	close(pipes[1][0]);
	//se asteapta terminarea executiei tuturor celor 10 procese
	for (i = 0; i < NRPROCESE; i++)
	{
		wait(NULL);
	}
	return 0;
}
