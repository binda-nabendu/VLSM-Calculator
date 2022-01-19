#include <stdio.h>
#include <math.h>

typedef struct assignForNetwork{
    char network[32], broadcast[32],firstAssignable[32], lastAssignable[32];
    int subnet;
} AssignForNetwork;

typedef struct ipBlock{
    char network[32];
    int subnet;
} IpBlock;

typedef struct network{
    int totalNodes;
    int serialNo;
} Network;


IpBlock remainingIpNetworkSet[1000];
int stkPtr = 0;


void push(char network[32],int subnet);
int pop();
void StringIpToNumConv(const char *ip, int *ipOctet, int p, int c);
void decimalToBinaryConv(int octet[4], char bin[32]);
void assignIpBlockForNet(int needBit, AssignForNetwork *pSet, int counter);
void assignThisSubnetworksAllDetails(char network[32], int subnet, AssignForNetwork *pSet, int counter);
void selectionSort(int arr[], int n);
void selectionSortForCustom(Network arr[], int n);

void printThem(AssignForNetwork *allSubnetwork, int n, Network *serialWithTotal);

void printAllChar(const char *string, char charSet[32]);
void printAllCharInDec(const char *string, char *charSet);

int main() {
    int n;
    printf("Please enter number of Network: ");
    scanf("%d",&n);

    int total=0,bitRequired[n];
    Network allNetworks[n];

    for(int i=0; i<n; i++){
        printf("Please enter the number of host in your Network %d: ",i+1);

        allNetworks[i].serialNo=i;
        scanf("%d",&allNetworks[i].totalNodes);
        allNetworks[i].totalNodes+=2;
        int c=1,p=0;
        while(c < allNetworks[i].totalNodes){
            c=c<<1;
            p++;
        }
        total+=c;

        bitRequired[i]=p;
    }

    char ip[19]; int subnet;

    printf("Please enter your ip address(format: 110.12.1.0): ");
    scanf("%s",&ip);
    printf("Please enter your subnet mask(format: 27): ");
    scanf("%d",&subnet);

    int ipOctet[4]={0,0,0,0};
    StringIpToNumConv(ip, ipOctet, 0, 0);

    ///Check is vlsm possible or not
    int maximum = (int)pow(2,(32-subnet));
    if(maximum<total){
        printf("VLSM Not possible...");
        return 0;
    }

    selectionSortForCustom(allNetworks,n);
    selectionSort(bitRequired,n);

    char ipInBin[32];
    decimalToBinaryConv(ipOctet,ipInBin);


    push(ipInBin,subnet);

    AssignForNetwork allAssignNetwork[n];

    for(int i=0; i<n; i++){
        assignIpBlockForNet(bitRequired[i],allAssignNetwork,i);
    }

    printThem(allAssignNetwork,n,allNetworks);

}

void printThem(AssignForNetwork *allSubnetwork, int n, Network *serialWithTotal) {
    printf("press 1 for see all value in decimal or 0 for binary:\n");
    int userChoice;
    scanf("%d",&userChoice);
    for(int i=0; i<n; i++){
        printf("For Network: %d which contain: %d nods\n",serialWithTotal[i].serialNo+1,serialWithTotal[i].totalNodes-2);
        if(userChoice==0) {
            printAllChar("Network Address: ", allSubnetwork[i].network);
            printAllChar("FirstAssignableHost: ", allSubnetwork[i].firstAssignable);
            printAllChar("LastAssignableHost: ", allSubnetwork[i].lastAssignable);
            printAllChar("Broadcast Address: ", allSubnetwork[i].broadcast);
        } else if(userChoice==1) {
            printAllCharInDec("Network Address: ", allSubnetwork[i].network);
            printAllCharInDec("FirstAssignableHost: ", allSubnetwork[i].firstAssignable);
            printAllCharInDec("LastAssignableHost: ", allSubnetwork[i].lastAssignable);
            printAllCharInDec("Broadcast Address: ", allSubnetwork[i].broadcast);
        }
        printf("Subnet Mask: %d\n",allSubnetwork[i].subnet);
        printf("----------------------------------------------------\n\n");

    }
}

void printAllChar(const char *string, char *charSet) {
    printf("%s",string);
    for(int i=0; i<32; i++){
        if(i!=0 && i%8==0) printf(".");
        printf("%c",charSet[i]);
    }
    printf("\n");
}
void printAllCharInDec(const char *string, char *charSet) {
    int octet[4]={0,0,0,0};
    int cBitVal=1;
    int total=0;
    int currentOctet=3;
    int counter=1;
    for(int i=31; i>=0; i--){
        total+= charSet[i]=='1'? cBitVal : 0;
        cBitVal=cBitVal<<1;
        if(counter==8){
            octet[currentOctet]=total;
            counter=0;
            currentOctet--;
            cBitVal=1;
            total=0;
        }
        counter++;
    }
    printf("%s(In decimal):",string);
    printf("%d:%d:%d:%d\n",octet[0],octet[1],octet[2],octet[3]);
}


void push(char network[32],int subnet){
    for(int i=0; i<32; i++){
        remainingIpNetworkSet[stkPtr].network[i] = network[i];
    }
    remainingIpNetworkSet[stkPtr].subnet = subnet;
    stkPtr++;

}
int pop(){
    return --stkPtr;
}

void selectionSort(int arr[], int n)
{
    int i, j, min_idx;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] > arr[min_idx])
                min_idx = j;
        int c=arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = c;

    }
}

void selectionSortForCustom(Network arr[], int n)
{
    int i, j, min_idx;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j].totalNodes > arr[min_idx].totalNodes)
                min_idx = j;
        Network c=arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = c;

    }
}

void assignIpBlockForNet(int needBit, AssignForNetwork *pSet, int counter) {
    int now=pop();
    IpBlock *popedBlock = &remainingIpNetworkSet[now];
    int newSubnet = 32 - needBit;
    int newBit = newSubnet - popedBlock->subnet;
    int newBlocks = (int) pow(2, newBit);

    //add first block to the list;
    assignThisSubnetworksAllDetails(popedBlock->network, newSubnet, pSet, counter);
    //push all unused block
    for(int i=1; i<newBlocks;i++){

        char ip[32];
        for(int j=0; j<32; j++){
            ip[j]=popedBlock->network[j];
        }


        int c=i;
        for(int j= newSubnet - 1; c > 0; j--){
            ip[j]=(c%2 ==1)?'1':'0';
            c/=2;
        }
        push(ip, newSubnet);
    }

}

void assignThisSubnetworksAllDetails(char network[32], int subnet, AssignForNetwork *pSet, int counter) {
    for(int i=0; i<32; i++) {
        pSet[counter].network[i] = network[i];
        pSet[counter].firstAssignable[i] = network[i];
        pSet[counter].lastAssignable[i] = network[i];
        pSet[counter].broadcast[i] = network[i];
    }
    pSet[counter].firstAssignable[31] = '1';
    for(int i=subnet; i<32; i++) {
        pSet[counter].broadcast[i] = '1';
        pSet[counter].lastAssignable[i] = '1';
    }
    pSet[counter].lastAssignable[31] = '0';
    pSet[counter].subnet=subnet;
}

void decimalToBinaryConv(int octet[4], char *bin) {
    int p=0;
    for(int i=0; i<4; i++){
        int target=octet[i];
        char cc[8];
        for(int j=0; j<8; j++){
            cc[j]= (target % 2 == 1)?'1':'0';
            target/=2;
        }
        for(int j=7; j>=0; j--){
            bin[p++]=cc[j];
        }
    }
}

void StringIpToNumConv(const char *ip, int *ipOctet, int p, int c) {
    while(ip[p] != '\0'){
        if(ip[p]=='.'){
            p++;
            c++;
        }else{
            ipOctet[c]*=10;
            ipOctet[c]+=(ip[p]-'0');
            p++;
        }
    }
}
