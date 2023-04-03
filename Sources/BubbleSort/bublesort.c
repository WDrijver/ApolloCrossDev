#include <stdio.h>
void bubbleSort(short *arr, unsigned short n);

void bubbleSort(short *arr, unsigned short n)
{   short cont=1, swap, *pt_arr;
    unsigned short i;
    while(cont)
    {
        cont=0;
        i=n;
        pt_arr=arr;
        while(--i)
        {
            if(*pt_arr>pt_arr[1]) /* swap */
            { cont++;
               swap=*pt_arr;
               *pt_arr++=pt_arr[1];
               *pt_arr=swap;
            }
	    else pt_arr++;
        }
        n--;
        
    }
}
  

int main()
{   int i;
    short arr[6] = { 50, 12, 30, 7, 1 };
    
    bubbleSort(arr, 5);
  
    for (i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
  
    return 0;
}
