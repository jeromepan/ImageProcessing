// MPI_Mat1_6_10_2014.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <mpi.h>
#include <mpicxx.h>
#include <iostream>
#include <random>
void Show(double*Yk,const int wide,const int myid);


int _tmain(int argc, char* argv[])
{	
	const int n=400;
	double* Ak;//���������䵽����A��n/p��
	double* Xk;//�������ֵ�������x��n/p��ֵ
	double* Yk;//������Ҫ����Ľ������y��n/p��ֵ

	int myid;//����ID
	int p;//�����ռ�Ľ����ܸ���
	double wtime;//��¼��������ʱ��

	MPI::Init(argc,argv);
	myid=MPI::COMM_WORLD.Get_rank();
	p=MPI::COMM_WORLD.Get_size();

	int wide=n/p;

	//ΪAk,Yk,Xk�����ڴ�
	Ak=new double[wide*n];
	Xk=new double[wide];
	Yk=new double[wide];

	//��ʼ��Yk
	for (int i=0;i<wide;++i)
	{
		Yk[i]=0;
	}

/************************************************************************/
/* 1.���������̷ַ����ݲ���                                           */
/************************************************************************/

	if (myid==0)
	{
		//��ʼ������A������X
		std::srand(10000);
		int A[n][n];
		int X[n];
		for (int i=0;i<n;++i)
		{
			X[i]=std::rand()%5;
			for (int j=0;j<n;++j)
			{
				A[i][j]=std::rand()%20-7;
			}
		}

		//��A��0��wide-1�д���0�Ž��̣�X��0��wide-1�д���Xk;
		for (int i=0;i<wide;++i)
		{
			for (int j=0;j<n;++j)
			{
				Ak[i*n+j]=A[i][j];
			}
			Xk[i]=X[i];
		}

		for (int i=1;i<p;++i)
		{
			//������i�����̵�Ak��Xk
			double* tAk=new double[wide*n];
			double* tXk=new double[wide];
			for (int r=0;r<wide;++r)
			{
				for (int c=0;c<n;++c)
				{
					tAk[r*n+c]=A[i*wide+r][c];
				}
				tXk[r]=X[i*wide+r];
			}

			//����Ak��Xk����������
			MPI::COMM_WORLD.Send(tAk,wide*n,MPI::DOUBLE,i,i+100);
			MPI::COMM_WORLD.Send(tXk,wide,MPI::DOUBLE,i,i+200);
		}
	}
	else
	{
		//Ϊ�������̽���Ak��Xk
		MPI::COMM_WORLD.Recv(Ak,wide*n,MPI::DOUBLE,0,myid+100);
		MPI::COMM_WORLD.Recv(Xk,wide,MPI::DOUBLE,0,myid+200);
	}

/************************************************************************/
/* 2.���м��㲿��                                                       */
/************************************************************************/
	if (myid==0)
	{
		wtime=MPI::Wtime();
	}

	//Yk���ֳ�ʼֵ
	for (int k=0;k<wide;++k)
	{
		for (int i=0;i<wide;++i)
		{
			Yk[k]+=Ak[k*n+myid*wide+i]*Xk[i];
		}
	}

	//ѭ������p-1��
	for (int j=1;j<p;++j)
	{
		//ÿ�ּ���ǰ����Xk��ǰһ������,���ܺ�һ�����̷�����Xk
		MPI::COMM_WORLD.Send(Xk,wide,MPI::DOUBLE,(myid-1+p)%p,3000);
		MPI::COMM_WORLD.Recv(Xk,wide,MPI::DOUBLE,(myid+1)%p,3000);

		//�ۼ�Yk
		for (int k=0;k<wide;++k)
		{
			for (int i=0;i<wide;++i)
			{
				Yk[k]+=Ak[k*n+((myid+j)%p)*wide+i]*Xk[i];
			}
		}
	}

	if (myid==0)
	{
		wtime=MPI::Wtime()-wtime;
	}

	MPI::Finalize();//�������в���
	
	//���Yk�ڸ������ϵ�ֵ
	Show(Yk,wide,myid);

	if (myid==0)
	{
		std::cout<<"The time is "<<wtime<<std::endl;
	}

	return 0;
}

void Show( double *Yk,const int wide,const int myid )
{
	std::cout<<"Process results of "<<myid<<std::endl;
	for (int i=0;i<wide;++i)
	{
		std::cout<<Yk[i]<<'\t';
	}
	std::cout<<"\n***************************************************************\n";
}
