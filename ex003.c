
// C program to implement concurrent merge sort 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
  
void insertionSort(int arr[], int n) { 
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


int getM(){
    int M;
    scanf("%d",&M);
    return M;
}  

void mergeSort(int a[], int l, int h) { 
    int i, len=(h-l+1); 

    
  
    // Using insertion sort for small sized array 
    
    //len<=M => M:1,10,100,1000 , but 10000 does not work
    if (len<=10 ) { 
        insertionSort(a+l, len); 
        return; 
    }

    else{
    pid_t lpid,rpid; 
    lpid = fork(); 
    if (lpid<0) 
    { 
        // Lchild proc not created 
        perror("Left Child Proc. not created\n"); 
        _exit(-1); 
    } 
    else if (lpid==0) 
    { 
        mergeSort(a,l,l+len/2-1); 
        _exit(0); 
    } 
    else
    { 
        rpid = fork(); 
        if (rpid<0) 
        { 
            // Rchild proc not created 
            perror("Right Child Proc. not created\n"); 
            _exit(-1); 
        } 
        else if(rpid==0) 
        { 
            mergeSort(a,l+len/2,h); 
            _exit(0); 
        } 
    } 
  
    int status; 
  
    // Wait for child processes to finish 
    waitpid(lpid, &status, 0); 
    waitpid(rpid, &status, 0); 
  
    // Merge the sorted subarrays 
    merge(a, l, l+len/2-1, h); 
    } 
  
} 
  

  

  
// To check if array is actually sorted or not 
void isSorted(int arr[], int len) { 
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


void printArray(int A[], int size) { 
    int i; 
    for (i=0; i < size; i++) 
        printf("%d ", A[i]); 
    printf("\n"); 
} 
  
// fill randome values in array for testing 
//first I put dynamic .
void fillData(int a[], int len) { 
    // Create random arrays 
    int i; 
    for (i=0; i<len; i++) 
        a[i] = rand(); 
    return; 
} 
  
// Driver code 
int main() { 
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
    printf("enter array length:");
    scanf("%d",&length);

    size_t SHM_SIZE = sizeof(int) * length;
    //size_t is the size in Bytes of requested shared memory
    

    
  
    // Create a random array of given length 
    srand(time(NULL)); 
    fillData(shm_array, length); 
    // for (int i = 0 ; i<length ; i++){
    //     scanf("%d",&shm_array[i]);
    // }
  
    printf("numbers are:\n");

    printArray(shm_array,length);

    mergeSort(shm_array, 0, length-1); 

    printf("sorted numbers are:\n");

    printArray(shm_array,length);
  
    isSorted(shm_array, length); 
  

    if (shmdt(shm_array) == -1) 
    { 
        perror("shmdt"); 
        _exit(1); 
    } 
  
    if (shmctl(shmid, IPC_RMID, NULL) == -1) 
    { 
        perror("shmctl"); 
        _exit(1); 
    } 
  
    return 0; 
}
