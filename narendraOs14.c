//11701338 narendra.D
#include<stdio.h>
#include<stdlib.h>

int process_count;							//total process count
int queue1_size=-1;
int queue2_size=-1;
int queue2_process_count=0;

struct process_q1{
int pid;
int a_time;
int b_time;
int priority;
int wait_time;
int turnaround_time;
}*p1;

int *burst;								//temp burst time storage
int *q1;
int *q2;

void insert1(int process_id)
{
	q1[++queue1_size]=process_id;
}

void insert2(int process_id)
{
	q2[++queue2_size]=process_id;
}

void delete1(int process_id)
{
	int i,j;
	for(i=0;i<=queue1_size;i++)
	{
		if(q1[i]==process_id)
		break;
	}
	for(j=i;j<=queue1_size-1;j++)
	{
		q1[j]=q1[j+1];
	}
	queue1_size--;
}

void delete2()
{
	int j;
	for(j=0;j<=queue2_size-1;j++)
	{
		q2[j]=q2[j+1];
	}
	queue2_size--;
}

void wait(int process_id)
  {  
    int i,j;
	for(i=0;i<=queue1_size;i++)
	{
		if(process_id!=q1[i])
		{
			p1[q1[i]-1].wait_time++;			//incrementing wait time of processes in the queue.
		}
	}
	for(j=0;j<=queue2_size;j++)
	{
		p1[q2[j]-1].wait_time++;
	}
}

void wait2()
{    int i;
	for(i=0;i<=queue2_size;i++)
	{
		p1[q2[i]-1].wait_time+=1;			//incrementing wait time of processes in the queue.
	}
}

int process_arrival(int time)
{
	int *id=(int*)malloc(sizeof(int)*process_count);	//array to store processes arriving at same time
	int i,pos=-1;
	for(i=0;i<process_count;i++)
	{
		if(p1[i].a_time==time)
		{
			id[++pos]=p1[i].pid;
			insert1(id[pos]);
		}
	}
	if(pos==-1)
	{						//no process
		return 0;
	}
	else if(pos==0)						//only one process
	{
		return id[pos];
	}
	else							//more than one process at given time
	{
		int i,max_p=id[0];
		for(i=1;i<=pos;i++)
		{
			if(p1[id[i]-1].priority<p1[max_p-1].priority)		//checking priority
			{
				max_p=id[i];
			}
		}
		return max_p;
	}
}

int process_arrival2(int time)
{
	int i=0;
	int id=0;
	for(;i<process_count;i++)
	{
		if(time==p1[i].a_time)
		{
			id=p1[i].pid;
		}
	}
	return id;
}

int allocate_p()
{
	if(queue1_size==0)			//if only one process in queue
	return q1[0];
	else
	{
		int i,max_p=q1[0];
		for(i=1;i<=queue1_size;i++)
		{
			if(p1[q1[i]-1].priority<p1[max_p-1].priority)
			{
				max_p=q1[i];
			}
			else if(p1[q1[i]-1].priority==p1[max_p-1].priority)		//checking arrival time of equal priority processes
			{
				if(p1[q1[i]-1].a_time<p1[max_p-1].a_time)
				{
					max_p=q1[i];
				}
			}
		}
		return max_p;
	}
}

int round_robin(int time,int flag)
{
	int time_q=10;
	int counter=0;
	int run_id=0;
	static int finish_process=0;
	while(finish_process!=queue2_process_count)
	{
		if(flag==1)
		{
			int new_id=process_arrival2(time);
			if(new_id!=0)
			{
				if(run_id!=0)
				{
					insert2(run_id);
				}
				return time;
			}
		}
		if(run_id==0)
		{
			run_id=q2[0];
			delete2();
		}
		time+=1;
		burst[run_id-1]-=1;
		wait2();
		counter+=1;
		if(burst[run_id-1]==0)
		{
			finish_process++;
			run_id=0;
			counter=0;
		}
		else if(counter==time_q)
		{
			insert1(run_id);
			counter=0;
			run_id=0;
		}
	}
	return time;
}

int preemptive()
{
	int temp_process_count=process_count;
	int finished_process=0;
	int time=0;
	int run_id=0;
	int new_id;
	while(finished_process!=temp_process_count)
	{
		new_id=process_arrival(time);					//process arriving at current time
		if(new_id!=0)
		{
			if(run_id!=0)
			{
				if(p1[new_id-1].priority<p1[run_id-1].priority)		//checking priority
				{
					delete1(run_id);
					insert2(run_id);			//swap into queue2
					queue2_process_count++;
					run_id=new_id;
					temp_process_count--;
				}
			}
			else
			{
				run_id=new_id;					//first running process or no process was running before
			}
		}
		else
		{
			if(queue2_size>-1 &&queue1_size==-1)
			{
				time=round_robin(time,1);
				continue;
			}
		}
		if(queue1_size>-1)						//if queue1 not empty
		{
			wait(run_id);
			burst[run_id-1]=burst[run_id-1]-1;
			if(burst[run_id-1]==0)					//if a process is finished
			{
				delete1(run_id);
				finished_process++;
				run_id=0;					//resetting running process id to NULL
				if(queue1_size!=-1)
				{
					run_id=allocate_p();				//select new process from queue1 if not empty
				}
			}

		}
		time++;
	}
	return time;
}

void display()
{
	int i,time=preemptive();
	float avg_wait_time,avg_turnaround_time;
	int wait_s=0,turn_s=0;
	if(queue2_size!=-1)
		time=round_robin(time,0);
	printf("\nPID\tPriority\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time");
	for(i=0;i<process_count;i++)
	{
		p1[i].turnaround_time=p1[i].b_time+p1[i].wait_time;
		printf("\n%-8d%-16d%-16d%-16d%-16d%d",p1[i].pid,p1[i].priority,p1[i].a_time,p1[i].b_time,p1[i].wait_time,p1[i].turnaround_time);
	}
	printf("\n\nTotal CPU time used(including idle time):%d",time);
	for(i=0;i<process_count;i++)
	{
		wait_s+=p1[i].wait_time;
		turn_s+=p1[i].turnaround_time;
	}
	avg_wait_time=wait_s*1.0/process_count;
	avg_turnaround_time=turn_s*1.0/process_count;
	printf("\nAverage Waiting time:%.3f",avg_wait_time);
	printf("\nAverage Turnaround time:%.3f",avg_turnaround_time);
	printf("\n");
}
int main()
{
	int i,temp;
	while(1)
	{
		printf("Enter number of Processes:");
		scanf("%d",&temp);
		if(temp>0)
		{
			process_count=temp;
			break;
		}
		else
		{
			printf("Error:Only enter positive number greater than 0.\n\n");
		}
	}
	p1=(struct process_q1*)malloc(sizeof(struct process_q1)*process_count);
	q1=(int*)malloc(sizeof(int)*process_count);
	q2=(int*)malloc(sizeof(int)*(process_count-1));
	burst=(int*)malloc(sizeof(int)*process_count);
	printf("\n\t\tDetails of Processes\n");
	for(i=0;i<process_count;i++)
	{
		p1[i].pid=i+1;
		while(1)
		{
			printf("\nEnter Arrival time of Process %d:",i+1);
			scanf("%d",&temp);
			if(temp>=0)
			{
				p1[i].a_time=temp;
				break;
			}
			else
			{
				printf("Error:Only enter positive numbers.\n");
			}
		}
		while(1)
		{
			printf("Enter Burst time of Process %d:",i+1);
			scanf("%d",&temp);
			if(temp>0 && temp%2==0)
			{
				p1[i].b_time=temp;
				break;
			}
			else
			{
				printf("Error:Only enter positive numbers more than 0 which are multiples of 2.\n\n");
			}
		}
		burst[i]=p1[i].b_time;
		while(1)
		{
			printf("Enter Priority of Process %d:",i+1);
			scanf("%d",&temp);
			if(temp>=0)
			{
				p1[i].priority=temp;
				break;
			}
			else
			{
				printf("Error:Only enter positive numbers.\n\n");
			}
		}
		p1[i].wait_time=0;
	}
	display();
	free(p1);
	free(q1);
	free(burst);
	free(q2);
}

