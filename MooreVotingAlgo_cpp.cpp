#include <stdio.h>
float majorityElement(float *values, int dataLength){
    int counter = 1;                            // initializing counter
    float candidate = *values;                  // considering first element as candidate majority element
    
    int maxCount = 0;                           // max frequency of the candidate is zero
    int i = 0; 
    while (i < dataLength){                     // finding the possible candidate
        if (counter == 0){
            candidate = *(values+i);            
        }
        
        if (*(values + i) == candidate){        // if matches with previous element, counter will increment
            counter++;
        } else {
            counter --;
        }
        i++;
    }
    
    i = 0; 
    while( i < dataLength){                     // finding the frequency of the candidate
        if (*(values+i) == candidate){
            maxCount++;
        }
        i++;
    }
    
    if (maxCount > dataLength/2){               // if frequency > dataLength/2 --> this is what we want
        return candidate;
    } else {
        return 1000.0;                          // means no candidate found
    }
    
}

int main()
{
    float x[] = {1.1 ,2.2, 3.0 ,2.2 ,2.2};
    int dataLength = sizeof(x) / sizeof(x[0]);
    printf("%d\n" , dataLength);
    float y = majorityElement(&x[0] , dataLength); 
    printf("%f", y);
}