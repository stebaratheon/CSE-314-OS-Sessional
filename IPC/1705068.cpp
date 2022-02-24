#include <iostream>
#include <fstream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <time.h>
#include <chrono>
#include <thread>
#include<bits/stdc++.h>
#define numOfPassengers 6
#define upperLimit 100
using namespace std;

int M,N,P,W,X,Y,Z,NP;
bool flag = false;

struct passengerId{
    int num_id=0;
    char id[3];
    bool isVIP = false;
    bool isBoardingLost = false;
    int waitingTime = 0;
}pId;

int arrKiosk[upperLimit],arrBelt[upperLimit];
fstream outFile("output.txt",ios_base::out);

sem_t empty_kiosk,current_pass_at_kiosk;
sem_t free_belts,occupied_belts;
sem_t empty_vip_channel,full_vip_channel;
sem_t empty_boarding_slot,full_boarding_slot;
sem_t empty_special_kiosk,full_special_kiosk;


pthread_mutex_t mtx;
pthread_mutex_t passenger_mtx[upperLimit];
pthread_mutex_t belt_mtx[upperLimit];
pthread_mutex_t vip_mtx,boarding_slot_mtx,special_passenger_mtx;

int clockTime = 1;

void goToBoardingGate(void * arg);
void goToVipChannel(void * arg);
void goToVipChannel2(void * arg);
void goToSecCheck(void * arg);
void goToSpecialKiosk(void * arg);
void* goToKiosk(void * arg)
{
    sem_wait(&empty_kiosk);
    
    pthread_mutex_lock(&passenger_mtx[((passengerId *)arg)->num_id]);
    //sem_wait(&empty_kiosk);

    struct passengerId* a =(passengerId *)arg;
    a->waitingTime = clockTime;
    int uid = a->num_id;
    outFile<<"Passenger "<<a->num_id<<" has arrived at the airpport at time "<<a->waitingTime<<endl;
    
    int k;
    for(int i=0;i<M;i++)
    {
        if(arrKiosk[i]==0){
            k=i;
            arrKiosk[i]++;
            break;
        }
    }
    outFile<<"Passenger "<<a->num_id<<" has started self-check in at kiosk "<<k<<" at time "<<a->waitingTime<<endl;
    sleep(W);
    a->waitingTime += W;
    outFile<<"  Passenger "<<a->num_id<<" has finised check in at time "<<a->waitingTime<<endl;
    
    arrKiosk[k]=0;
    bool status=a->isVIP;
    //sem_post(&empty_kiosk);
    pthread_mutex_unlock(&passenger_mtx[((passengerId *)arg)->num_id]);
    sem_post(&empty_kiosk);
    if(status==false){
        goToSecCheck(arg);
    }else{
        flag = true;
        goToVipChannel(arg);
    }

    int *x = new int(5);
    return x;
}
void goToSpecialKiosk(void * arg)
{
    sem_wait(&empty_special_kiosk);
    
    pthread_mutex_lock(&passenger_mtx[((passengerId *)arg)->num_id]);
    //sem_wait(&empty_kiosk);

    struct passengerId* a =(passengerId *)arg;
    a->waitingTime = clockTime;
    int uid = a->num_id;
    outFile<<"Passenger "<<a->num_id<<" has started self-check in at special kiosk at time "<<a->waitingTime<<endl;
    sleep(W);
    a->waitingTime += W;
    outFile<<"  Passenger "<<a->num_id<<" has finised check in at special kiosk at time "<<a->waitingTime<<endl;
    
    bool status=a->isVIP;
    //sem_post(&empty_kiosk);
    pthread_mutex_unlock(&passenger_mtx[((passengerId *)arg)->num_id]);
    sem_post(&empty_special_kiosk);
    
    goToBoardingGate(arg);
}
void goToSecCheck(void * arg)
{
    sem_wait(&free_belts);

    pthread_mutex_lock(&passenger_mtx[((passengerId *)arg)->num_id]);
    
    int i,k;
    struct passengerId* a =(passengerId *)arg;
    //a->waitingTime = clockTime;
    int uid = a->num_id;
    //finding the belt
    for(i=0;i<N;i++)
    {
        if(arrBelt[i]<P)
        {
            k=i;
            arrBelt[i]++;
            break;
        }
    }
    a->waitingTime = clockTime;
    outFile<<" passenger "<<uid<<" has started waiting for security check in belt "<<k<<" from time "<<a->waitingTime<<endl;
    outFile<<" passenger "<<uid<<" has started security check in belt "<<k<<" from time "<<clockTime<<endl;

    sleep(X);
    a->waitingTime += X;
    outFile<<" passenger "<<uid<<" has crossed security check in belt "<<k<<" at time "<<a->waitingTime<<endl;
    //outFile<<"                         clockTIme "<<clockTime<<endl;
    clockTime++;
    arrBelt[i]--;
    bool status = a->isBoardingLost;
    a->isBoardingLost = true;
    pthread_mutex_unlock(&passenger_mtx[((passengerId *)arg)->num_id]);

    sem_post(&free_belts);
    if(status){
        goToVipChannel2(arg);
    }else{
        goToBoardingGate(arg);
    }
}
void goToBoardingGate(void * arg)
{
    sem_wait(&empty_boarding_slot);

    pthread_mutex_lock(&passenger_mtx[((passengerId *)arg)->num_id]);
    struct passengerId* a =(passengerId *)arg;
    //a->waitingTime = clockTime;
    int uid = a->num_id;
    a->waitingTime = clockTime;

    outFile<<" Passenger "<<uid<<" has started waiting to be boarded at time "<<a->waitingTime<<endl;
    outFile<<" Passenger "<<uid<<" has started boarding the plane at time "<<clockTime<<endl;

    sleep(Y);
    a->waitingTime += Y;
    outFile<<" Passenger "<<uid<<" has boarded at time "<<a->waitingTime<<endl;
    clockTime++;
    
    pthread_mutex_unlock(&passenger_mtx[((passengerId *)arg)->num_id]);

    sem_post(&empty_boarding_slot);

}
void goToVipChannel(void * arg)
{
    sem_wait(&empty_vip_channel);   

    struct passengerId* a =(passengerId *)arg;
    //a->waitingTime = clockTime;
    int uid = a->num_id;
    a->waitingTime = clockTime;

    outFile<<"  Passenger "<<uid<<" has arrived at vip channel at time "<<a->waitingTime<<endl;
    sleep(Z);
    a->waitingTime += Z;
    outFile<<"  Passenger "<<uid<<" has passed through vip channel at time "<<a->waitingTime<<endl;
    flag = false;
    sem_post(&empty_vip_channel);

    goToBoardingGate(arg);
}
void goToVipChannel2(void * arg)
{
    sem_wait(&empty_vip_channel);   

    struct passengerId* a =(passengerId *)arg;
    //a->waitingTime = clockTime;
    int uid = a->num_id;
    a->waitingTime = clockTime;

    outFile<<"  Passenger "<<uid<<" has arrived at vip channel(opposite) at time "<<a->waitingTime<<endl;
    sleep(Z);
    a->waitingTime += Z;
    outFile<<"  Passenger "<<uid<<" has passed through vip channel(opposite) at time "<<a->waitingTime<<endl;
    sem_post(&empty_vip_channel);
    
    goToSpecialKiosk(arg);
}
void * incTime(void *arg)
{
    while(1){
        sleep(1);
        clockTime++;
    }
    return new int(5);
}

int main()
{
    void* returnedDummy;
    int res,i;

    int arrivalTime;
    random_device randD;
    mt19937 mt(5);
    double lambda = 1.0/13.0;
    exponential_distribution<double> ed(lambda); 
    

    // taking input
    fstream inFile("input.txt",ios_base::in);
    fstream outFile("output.txt",ios_base::out);
    inFile>>M>>N>>P>>W>>X>>Y>>Z;
    NP = N*P;
    
    //initializing semaphores
    res = sem_init(&empty_kiosk,0,M);
    if(res!=0){
        cout<<"Failed to init empty_kiosk semaphore"<<endl;
    }

    res = sem_init(&current_pass_at_kiosk,0,0);
    if(res!=0){
        cout<<"Failed to init current pass at_kiosk semaphore"<<endl;
    }

    res = sem_init(&free_belts,0,NP);
    if(res!=0){
        cout<<"Failed to init free_belt semaphore"<<endl;
    }

    res = sem_init(&occupied_belts,0,0);
    if(res!=0){
        cout<<"Failed to init occupied_belts semaphore"<<endl;
    }

    res = sem_init(&empty_vip_channel,0,1);
    if(res!=0){
        cout<<"Failed to init empty_vip_channel semaphore"<<endl;
    }

    res = sem_init(&full_vip_channel,0,0);
    if(res!=0){
        cout<<"Failed to init full_vip_channel semaphore"<<endl;
    }

    res = sem_init(&empty_boarding_slot,0,1);
    if(res!=0){
        cout<<"Failed to init empty_boarding_slot semaphore"<<endl;
    }

    res = sem_init(&full_boarding_slot,0,0);
    if(res!=0){
        cout<<"Failed to init full_boarding slot semaphore"<<endl;
    }

    res = sem_init(&empty_special_kiosk,0,1);
    if(res!=0){
        cout<<"Failed to init empty_special_kiosk semaphore"<<endl;
    }

    res = sem_init(&full_special_kiosk,0,0);
    if(res!=0){
        cout<<"Failed to init empty_vip_channel semaphore"<<endl;
    }

    //init mutexes
    res = pthread_mutex_init(&mtx,NULL);
    for( i = 0 ; i < numOfPassengers ; i++ )
    {
        res = pthread_mutex_init(&passenger_mtx[i],NULL);
        if(res!=0){
            cout<<"Failed to init kiosk mutex"<<i<<endl;
        }
    }
    for( i = 0 ; i < N ; i++ )
    {
        res = pthread_mutex_init(&belt_mtx[i],NULL);
        if(res!=0){
            cout<<"Failed to init belt mutex"<<i<<endl;
        }
    }

    res = pthread_mutex_init(&vip_mtx,NULL);
    if(res!=0){
        cout<<"Failed to init vip mutex"<<i<<endl;
    }

    res = pthread_mutex_init(&boarding_slot_mtx,NULL);
    if(res!=0){
        cout<<"Failed to init boarding slot mutex"<<i<<endl;
    }

    res = pthread_mutex_init(&special_passenger_mtx,NULL);
    if(res!=0){
        cout<<"Failed to init special kiosk mutex"<<i<<endl;
    }

    //init pthreads
    pthread_t passenger[numOfPassengers];
    pthread_t timeThread;
    pthread_mutex_init(&mtx,NULL);
    
    res = pthread_create(&timeThread,NULL,incTime,NULL);

    passengerId pass[numOfPassengers];
    for(i=0;i<numOfPassengers;i++)
    {
        arrivalTime = (int)ed.operator()(mt);
        //cout<<"sleep time = "<<arrivalTime<<endl;
        sleep(arrivalTime);
        strcpy(pass[i].id, to_string(i + 1).c_str());
        pass[i].num_id=i;
        pass[i].isVIP = (i%3==0 && i>1)? true:false;
        pass[i].isBoardingLost = (i%5==0 && i>1)? true:false;
        res = pthread_create(&passenger[i],NULL,goToKiosk,(void *)(&pass[i]));
    }
    
    for( i = 0; i < numOfPassengers; i++){
        void *result;
        pthread_join(passenger[i],&result);
        printf("%s",(char*)result);
    }
    return 0;
}
