#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <wait.h>

void merge(int arr[] , int l , int m , int r){
    int n1 = m-l+1;
    int n2 = r-m;
    //temp arrays
    int L[n1],R[n2];
    //copy data to temp arrays
    for(int i = 0 ; i < n1 ; i++){
        L[i] = arr[l +i];
    }
    for (int j = 0; j < n2; j++){
        R[j] = arr[m+1 +j];
    }
    //merge the temp arrays
    //initial index of subarrays 
    int i = 0; //left 
    int j = 0; //right
    int k = l; //merged subarray
    while (i<n1 && j<n2){
        if(L[i] <= R[j]){
            arr[k] = L[i];
            i++;
        }else{
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    //copy remaining elements of L and R
    while (i<n1){
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j<n2){
        arr[k] = R[j];
        j++;
        k++;
    }
}

// void merge(int a[], int l1, int h1, int h2) 
// { 
//     // We can directly copy  the sorted elements 
//     // in the final array, no need for a temporary 
//     // sorted array. 
//     int count=h2-l1+1; 
//     int sorted[count]; 
//     int i=l1, k=h1+1, m=0; 
//     while (i<=h1 && k<=h2) 
//     { 
//         if (a[i]<a[k]) 
//             sorted[m++]=a[i++]; 
//         else if (a[k]<a[i]) 
//             sorted[m++]=a[k++]; 
//         else if (a[i]==a[k]) 
//         { 
//             sorted[m++]=a[i++]; 
//             sorted[m++]=a[k++]; 
//         } 
//     } 
  
//     while (i<=h1) 
//         sorted[m++]=a[i++]; 
  
//     while (k<=h2) 
//         sorted[m++]=a[k++]; 
  
//     int arr_count = l1; 
//     for (i=0; i<count; i++,l1++) 
//         a[l1] = sorted[i]; 
// }


void insertionSort(int arr[], int n) 
{ 
   int i, key, j; 
   for (i = 1; i < n; i++) 
   { 
       key = arr[i]; 
       j = i-1; 
  
       /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */
       while (j >= 0 && arr[j] > key) 
       { 
           arr[j+1] = arr[j]; 
           j = j-1; 
       } 
       arr[j+1] = key; 
   } 
} 


void mergeSort(int arr[] , int l , int r){
    int i;
    int len = r-l+1;
    int m = l + len/2 - 1;

    if (len<=5) 
    { 
        insertionSort(arr+l, len); 
        return; 
    } 

    pid_t lpid , rpid;
    lpid = fork();
    if(lpid < 0){
        perror(" left child fork failed ");
        _exit(-1); 
   }
    //left child 
    else if (lpid == 0){
        mergeSort(arr,l,m);
        _exit(0); 
    }
    //parent 
    else{
        rpid = fork();
        if (rpid < 0){
            perror(" right child fork failed ");
            _exit(-1); 
        }
        else if(rpid == 0){
            mergeSort(arr,m+1,r);
            _exit(0);
        }
    }
    int status;
    //wait for processes to finish
    waitpid(lpid,&status,0);
    waitpid(rpid,&status,0);
    //and then merge
    merge(arr,l,m,r);
}
    

void isSorted(int arr[], int len) 
{ 
    if (len==1) 
    { 
        printf("Sorting Done Successfully\n"); 
        return; 
    } 
  
    int i; 
    for (i=1; i<len; i++) 
    { 
        if (arr[i]<arr[i-1]) 
        { 
            printf("Sorting Not Done\n"); 
            return; 
        } 
    } 
    printf("Sorting Done Successfully\n"); 
    return; 
} 

void printArray(int arr[] , int size){
    for (int i = 0; i < size; i++){
        printf("%d " , arr[i]);
    }
    printf("\n");
}

void fillData(int a[], int len) { 
    // Create random arrays 
    int i; 
    for (i=0; i<len; i++) 
        a[i] = rand(); 
    return; 
}
    

int main(){
    int shmid;
    //shmid means shared memory id
    //return value "from" shmget

    key_t key = IPC_PRIVATE;
    //key is a access value associated with the semaphore ID
    //key to be passed "to" shmget
    //IPC_PRIVATE is ...

    int* shm_array;
    //int* ?
    int length;
    //get length of array(size of shared memory too?) from user
    scanf("%d",&length);

    size_t SHM_SIZE = sizeof(int) * length;
    //size_t is the size in Bytes of requested shared memory
    


    //note:
        //about IPC_... in iphone
    
    //create the segment
    if((shmid = shmget(key , SHM_SIZE , IPC_CREAT | 0666)) < 0){
        perror("shmget error");
        _exit(1);
    }
    //0666 ?

    //now we attach the segment to our data space
    if((shm_array = shmat(shmid,NULL,0)) == (int*) - 1){
        perror("shmat error");
        _exit(1);
    }
    //(int *)-1 ??

    // int arr[length];
    //get array from user
    for (int i = 0; i < length; i++){
        scanf("%d",&shm_array[i]);
    }
    //print array before sorting
    printArray(shm_array,length);
    //now merge sort
    mergeSort(shm_array,0,length-1);
    //print array after sorting
    isSorted(shm_array,length);
    printArray(shm_array,length);
    
    if (shmdt(shm_array) == -1) 
    { 
        perror("shmdt"); 
        _exit(1); 
    } 
  
    /* Delete the shared memory segment. */
    if (shmctl(shmid, IPC_RMID, NULL) == -1) 
    { 
        perror("shmctl"); 
        _exit(1); 
    } 

    return 0;
}