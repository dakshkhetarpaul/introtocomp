#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
clock_t Start=0,End;
pid_t 
child_pid, wpid,masterpid;
int PN,H;
int max=0, sum = 0,j=0;
float avg;
masterpid = getpid();
//Accept Input
printf("Input PN : ");
scanf("%d", &PN);
printf("Input H : ");
scanf("%d", &H);
if(H > 50){H = 50;}
int numOfProcesses = PN;
int HiddenKeyIndex[50];
//Start Clock
Start = clock();

// Read ints from file and put them into array value
FILE* fp;
FILE* fp2;
fp2 = fopen("resultsFile.txt","w");
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
int baseoffset = size/numOfProcesses;
int offset = (size % numOfProcesses);
int* myValues = malloc((baseoffset + offset)*sizeof(int));

for(int j = 0; j < (baseoffset+offset);j++){
    myValues[j] = value[j];
}

if (PN > 1){ // If multiple processes (child is made)
    pid_t mycpid;
    int Cpipe[PN][2];
    pipe(Cpipe[PN]);
    while(((mycpid = fork()) == 0)&& PN > 2){ // Child fork child PN times | This is the child code
        read(Cpipe[PN][0],&PN,sizeof(int));
        read(Cpipe[PN][0],&offset,sizeof(int));
        read(Cpipe[PN][0],myValues,baseoffset*sizeof(int));
        pipe(Cpipe[--PN]);
    }

    if(mycpid == 0){
        read(Cpipe[PN][0],&PN,sizeof(int));
        read(Cpipe[PN][0],&offset,sizeof(int));
        read(Cpipe[PN--][0],myValues,baseoffset*sizeof(int));
    }

    if(mycpid < 0){ // Error Catch
        return 1;
    }
    else{ // Parent of child process execution code
        offset = offset + baseoffset;
        value = value + offset;
        write(Cpipe[PN][1],&PN, sizeof(int));
        write(Cpipe[PN][1],&offset,sizeof(int));
        write(Cpipe[PN][1],value,baseoffset*sizeof(int));     


        j = 0;
        for(i = 0; i < baseoffset; i++){
            if(myValues[i]> max){
                max = myValues[i];
            }
            if(myValues[i] == -1 && j != H){
                HiddenKeyIndex[j++] = i + offset - baseoffset;
            }
            sum = sum+myValues[i];
    }


    if( getpid() == masterpid){
        for(int k = i; k < offset;k++){
            if(myValues[k]> max){
                max = myValues[k];
        }
            if(myValues[k] == -1 && j != H){
                HiddenKeyIndex[j++] = i + offset - baseoffset;
                //printf("Hidden Key found at index %d\n",HiddenKeyIndex[j++]);
        }
            sum = sum+myValues[k];
        }
    }


    fprintf(fp2,"%s %d %s %d %s %d", "HI, I am process",getpid(),"with return argument", PN, ".My parent is process", getppid());
    fputc('\n',fp2);
    if (mycpid == 0){
        write(Cpipe[++PN][1],&sum,sizeof(int));
        write(Cpipe[PN][1],&max,sizeof(int));
        write(Cpipe[PN][1],&j,sizeof(int));
        write(Cpipe[PN][1],HiddenKeyIndex,(j)*sizeof(int));
        exit(PN);
    }
    

    int cmax, hkeysize;
    int cSum;
    wait(NULL);
    read(Cpipe[PN][0],&cSum,sizeof(int));
    read(Cpipe[PN][0],&cmax,sizeof(int));
    read(Cpipe[PN][0],&hkeysize,sizeof(int));
    read(Cpipe[PN][0],HiddenKeyIndex+j,hkeysize*sizeof(int));

    if (max < cmax){ max = cmax;}
    hkeysize = hkeysize + j;
    cSum = (sum + cSum);
    if (getpid() != masterpid){
        write(Cpipe[++PN][1],&cSum,sizeof(int));
        write(Cpipe[PN][1],&max,sizeof(int));
        write(Cpipe[PN][1],&hkeysize,sizeof(int));
        write(Cpipe[PN][1],HiddenKeyIndex,hkeysize*sizeof(int));
        exit(PN);
    }

    avg = cSum/size;
    wait(NULL);
    fprintf(fp2,"%s %d %s %f","Max = ", max, "Average = ", avg);
    fputc('\n',fp2);
    for(int i = 0; i < H; i++){
        fprintf(fp2, "%s %d %s","Hidden Key found at index [", HiddenKeyIndex[i], "]");
        fputc('\n',fp2);
    }

    }
    free(myValues);
}
else{ // If only 1 process (parent)
    fprintf(fp2,"%s %d %s %d %s %d", "HI, I am process",getpid(),"with return argument", PN, ".My parent is process", getppid());
    fputc('\n',fp2);
    for(i = 0; i < size; i++){
        if(value[i]> max){
            max = value[i];
        }
        if(value[i] == -1 && j != H){
            HiddenKeyIndex[j++] = i;
            fprintf(fp2, "%s %d %s","Hidden Key found at index [", i, "]");
            fputc('\n',fp2);
        }
        sum = sum+value[i];
    }
    avg = sum/size;
    fprintf(fp2, "%s %d %s %f","Max value =", max, "Average =", avg);
    fputc('\n',fp2);
    free(value);
}

// End time
End = clock();
double executionTime = ((double)End-Start)/CLOCKS_PER_SEC;
fprintf(fp2, "%s %d %s %f","Time taken to execute process", getpid(), "in seconds:", executionTime);
fputc(' ', fp2);
fclose(fp2);
return 0;
}