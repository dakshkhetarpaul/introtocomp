#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>  
#include <math.h>

int H, PN;

int main(){
    double time_spent = 0.0;
    clock_t begin = clock();
    printf("Input PN : ");
    scanf("%d", &PN);
    printf("Input H : ");
    scanf("%d", &H);
    
    int maxChildren;
    int parentRoot = getpid();
    char* input = malloc(50);
    int h[150];
    for(int i =0; i < 150; i++)
    h[i]= -1;
    int hstart =0;
    printf("Children Spawn: Pick 2, 3 or 4?\n");
    scanf("%d",&maxChildren);//get input from user

    if (maxChildren==0)//if the input is not a number......
    {
        printf("Invalid input entered\n\n");
        return -1;
    }
    printf("\n");
    
    FILE* fp;
    FILE* fp2;
    fp2 = fopen("resultsFileBFS.txt","w");
    fp = fopen("numFile.txt","r");
    int* value;
    int i=0,L = 1,Length=0;
    value = malloc(L*sizeof(int));

while(!feof(fp)){
    fscanf(fp,"%d", &value[i++]);
    Length++;
    if(Length == L){
        value = realloc(value,(i+1)*sizeof(int));
        Length = 0;
    }
}

fclose(fp);
int size = i-1;
float floatsize = size;
    //Makes N number of childs for Parent without Children making Processes.

    //Idea: Try utilizing the level of the tree to make the processes!    

    pid_t childMaker;

    int childCounter=-1;
    int fd[2*PN];
    int pid;
    int start = 0;
    int end = size+50;
       //START THE BFS TREE
    int childTrack[4] = {-1, -1, -1, -1};
    int parentPipe = -1;
    int oldEnd;
    int returnCode;
    int returnArg = 1;
    for (int j=0; j<floor(log2(PN)); j++) 
    {
        if(j!=0){
            if(maxChildren == 2)
            returnArg = 2*(returnArg)-1;
            else if(maxChildren == 3)
            returnArg = 3*(returnArg)-2;
            else
            returnArg = 4*(returnArg)-3;
        }
        childMaker = getpid();
        int increments;
         increments = ceil((end-start)/maxChildren);
         oldEnd = end;
         end = start;
         int found = 0;
        for(int i = 0; i < maxChildren; i++){
            
            if(childMaker == getpid() && returnArg < PN){
                childCounter++;
                
                for(int l = 0; l < maxChildren; l++)
                if(childTrack[l]==-1 ){
                    childTrack[l] = returnArg-1;
                    
                    break;
                }
                
                pipe(&fd[2*(returnArg-1)]);
                pid = fork();
                if(i!=0)
                    start = start+increments;
                end = end+increments;
                if((L+50)-end < 5)
                end = L+50;   
                returnArg = returnArg+1;
            }
        }

        if (pid == -1) {  
            return 1;
        }

        //CHILD PROCESS
        else if (pid == 0) {
                
            
            for(int l = 0; l < maxChildren; l++){
            if(childTrack[l]!=-1){
                parentPipe = childTrack[l];
                childTrack[l] = -1;
            }
            }
            fprintf(fp2,"%s %d %s %d %s %d", "HI, I am process",getpid(),"with return argument", returnArg, ".My parent is process", getppid());
            fputc('\n',fp2);
            returnCode = returnArg;
            pid = getpid();
            if(j == (floor(log2(PN))-1)){
                int max = 0;
                int64_t avg = 0;
                int count = 0;
                for(int j = start; j < end; j++){
                    if (value[j] > max)
                        max = value[j];
                    avg += value[j];

                    if(value[j] == -1){
                        h[hstart] = getpid();
                        h[hstart+1] = j;
                        h[hstart+2] = returnArg;
                        hstart = hstart +3;
                    }
                        
                }


                avg = avg / (end - start);
                count = end-start;
                close(fd[2*parentPipe]);
                write(fd[2*parentPipe+1], &max, sizeof(int));
                write(fd[2*parentPipe+1], &avg, sizeof(int64_t));
                write(fd[2*parentPipe+1], &count, sizeof(int));
                write(fd[2*parentPipe+1], &h, sizeof(int)*150);
                write(fd[2*parentPipe+1], &hstart, sizeof(int));
                close(fd[2*parentPipe+1]);
                
            exit(0);
            }

        
        }
        else { // parent process (NO FORKING IN HERE TO KEEP CHAIN FORMAT!) Some ends need to be closed.
                int hasChildren = 0;
                int childCount = 0;
                int max = 0;
                int tracked = 0;
                int64_t avg = 0;
                int count = 0;
                int tempMax = -1;
                int tempAvg = -1;
                int tempCount = -1;
                int tempH[150];

                int tempHstart = 0;
                for(int r = 0; r < maxChildren; r++){
                    if(childTrack[r] != -1){
                        hasChildren = 1;
                        childCount++;
                    }

                    else if(childTrack[r] == -1 && hasChildren && !tracked){
                        start = end;
                        end = oldEnd;
                        tracked = 1;
                        
                    }
                    
                }

                if(!hasChildren){
                    end = oldEnd;
                }

                
                if(childCount == 0) //If process with no children end up here
                {
                   
                    for(int j = start; j < end; j++){
                        if (value[j] > max)
                            max = value[j];
                        avg += value[j];
                        if(value[j] == -1){
                        h[hstart] = getpid();
                        h[hstart+1] = j;
                        h[hstart+2] = returnArg;
                        hstart = hstart +3;
                    }
                               
                    }
                    avg = avg / (end - start);
                    count = end-start;
                    
                    
                }

                else if(childCount < maxChildren) //If a process with not the max children ends up here
                {

                    for(int j = start; j < end; j++){
                        if (value[j] > max)
                            max = value[j];
                        avg += value[j];
                        if(value[j] == -1){
                        h[hstart] = getpid();
                        h[hstart+1] = j;
                        h[hstart+2] = returnCode;
                        hstart = hstart +3;
                    }

                    }
                    avg = avg / (end - start);
                    count = end-start;
                    
                    for(int r = 0; r < childCount; r++){

                        //PIPE CALLED ONE
                        read(fd[2*childTrack[r]], &tempMax, sizeof(int));
                        read(fd[2*childTrack[r]], &tempAvg, sizeof(int64_t));
                        read(fd[2*childTrack[r]], &tempCount, sizeof(int));
                        read(fd[2*childTrack[r]], &tempH, sizeof(int)*150);
                        read(fd[2*childTrack[r]], &tempHstart, sizeof(int));
            
                        if(tempMax >= max){
                            max = tempMax;
                        }
                        avg = (avg*count + tempAvg * tempCount)/(tempCount+count);
                        count += tempCount;

                        
                        for(int i = 0; i < tempHstart; i++){
                            h[hstart] = tempH[i];
                            hstart++;
                        } 
                        
                        
                    }


                }

                else{
                    for(int r = 0; r < childCount; r++){
                        //PIPE CALLED TWO
                        if(r!=0){
                            
                        read(fd[2*childTrack[r]], &tempMax, sizeof(int));
                        read(fd[2*childTrack[r]], &tempAvg, sizeof(int64_t));
                        read(fd[2*childTrack[r]], &tempCount, sizeof(int));
                        read(fd[2*childTrack[r]], &tempH, sizeof(int)*150);
                        read(fd[2*childTrack[r]], &tempHstart, sizeof(int));
                        if(tempMax >= max){
                            max = tempMax;
                        }
                       avg = (avg*count + tempAvg * tempCount)/(tempCount+count); 
                       for(int i = 0; i < tempHstart; i++){
                            h[hstart] = tempH[i];
                            hstart++;
                        } 
                       
                        
                        }
                        else{
                            read(fd[2*childTrack[r]], &max, sizeof(int));
                            read(fd[2*childTrack[r]], &avg, sizeof(int64_t));
                            read(fd[2*childTrack[r]], &count, sizeof(int));
                            read(fd[2*childTrack[r]], &h, sizeof(int)*150);
                            read(fd[2*childTrack[r]], &hstart, sizeof(int));

                            
                        }
                        
                    }
                }

                if(parentRoot != getpid()){

                    //PIPE CALLED THREE
                    write(fd[2*parentPipe+1], &max, sizeof(int));
                    write(fd[2*parentPipe+1], &avg, sizeof(int64_t));
                    write(fd[2*parentPipe+1], &count, sizeof(int));
                    write(fd[2*parentPipe+1], &h, sizeof(int)*150);
                    write(fd[2*parentPipe+1], &hstart, sizeof(int));
                }

                else{
                    wait(NULL);

                    printf("Max: %d, Avg: %ld\n\n", max, avg);
                    for(int i = 0; i < H*3; i+=3){
                        printf("Hi I am Process %d with return argument %d and I found the hidden key at position A[%d].\n", h[i], h[i+2], h[i+1]);

                    }
                    clock_t end = clock();
                    time_spent += (double)(end-begin) / CLOCKS_PER_SEC;
                    printf("\nThe program completed in %f seconds\n", (time_spent ));
                    exit(0);
                }
                //printf("Parent Process %d: My start is %d and my end is %d\n", getpid(), start, end);
                wait(NULL);
                exit(0);
            }       
    }   
}
