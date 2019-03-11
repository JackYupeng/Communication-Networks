/* 
 * File:   receiver_main.c
 * Author: 
 *
 * Created on
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define BUFSIZE 1800
#define HEADER_SIZE 50

struct sockaddr_in si_me, si_other;
int s, slen;

void diep(char *s) {
    perror(s);
    exit(1);
}



void reliablyReceive(unsigned short int myUDPport, char* destinationFile) {
    
    slen = sizeof (si_other);


    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        diep("socket");

    memset((char *) &si_me, 0, sizeof (si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myUDPport);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Now binding\n");
    if (bind(s, (struct sockaddr*) &si_me, sizeof (si_me)) == -1)
        diep("bind");












	/* Now receive data and send acknowledgements */
    //my code starts from here
    remove(destinationFile);    ///////////////////////////////////////////do i need to remove?
    FILE* fp = open(destinationFile,"wb");
    if (fp == NULL) {
        fprintf(stderr, "FILE NOT OPENDED\n");
        return;
    }

    //define variables
    char buf[BUFSIZE];
    char Not_In_Order_Mes[BUFSIZE * 300];

    int out_of_order_index[500];
    int left_out_of_order[500];
    int right_out_of_order[500];

    int Window_Index = 0;
    int Num = 0;


    struct sockaddr_in sender_address;
    socklen_t fromlength = sizeof(sender_address);

    int lask_Ack_Sent = -1;
    int bytes_received = 0;
    //end of variable define



    while(1 == 1) {
        //first handle error cases
        bytes_received = recvfrom(s, buf, BUFSIZE, 0, (struct sockaddr*)&sender_address, &fromlength);
        if (bytes_received == -1) { 
            fprintf(stderr, "Connection closed\n");
            break;
        }

        buf[bytes_received] = '\0';

        int ret;
        ret = strncmp(buf, "EOT", 3);
        if(ret == 0) {
            fprintf(stderr, "end of transmission\n");
            break;
        }

        //there is no error and the receiver should receive the packet
        else { 
            char header[HEADER_SIZE];
            char* curr;

            curr = buf;
            int l;
            for(l = 0; l < BUFSIZE; l++) {
                if(*curr != ';') {
                    header[l] = *curr; //store the hearder of the message to header, and ACK it later
                    curr++;
                }
                else {
                    break;
                }
            }

            curr++; //curr now points to the first bit of the actual content of message
            header[l] = '\0';

            int BufIndex;
            sscanf(header, "header%d", &BufIndex);

            if(BufIndex == lask_Ack_Sent + 1) { //this is what we are expecting
                lask_Ack_Sent = lask_Ack_Sent + 1;
                fwrite(curr, 1, bytes_received - l - 1, fp);

                while(1 == 1) { //remove packets from buffer to prevent from overflow
                    int len_in_buf = 0;
                    int i;
                    for(i = 0; i < Window_Index; i++) {
                        if(out_of_order_index[i]==lask_Ack_Sent + 1){
                            printf("A new Ack arrives\n");
                            lask_Ack_Sent++;
                            int len_in_buf = right_out_of_order[i]-left_out_of_order[i];
                            fwrite(&Not_In_Order_Mes[left_out_of_order[i]], 1, len_in_buf, fp);
                            for(int j=right_out_of_order[i];j<Num; j++){
                                Not_In_Order_Mes[j-len_in_buf] = Not_In_Order_Mes[j];
                            }
                            for(int k=i+1;k<Window_Index; k++){ //reorder packets in the buffer
                                left_out_of_order[k-1]=left_out_of_order[k];
                                right_out_of_order[k-1]=right_out_of_order[k];
                                out_of_order_index[k-1]=out_of_order_index[k];
                            }
                            break;
                        }
                    }
                    if(i == Window_Index) {
                        break;
                    }
                    else{
                        Num -= len_in_buf;
                        Window_Index--;

                    }
                }

                char Ack_To_Be_Sent[HEADER_SIZE];
                sprintf(Ack_To_Be_Sent, "Ack_To_Be_Sent%d;", lask_Ack_Sent); 

                sendto(s, Ack_To_Be_Sent, strlen(Ack_To_Be_Sent), 0, (struct sockaddr *) & sender_address, sizeof(sender_address));
            }
            else if (BufIndex > lask_Ack_Sent + 1) { //larger than expecting
                printf("getting larger new Ack\n");
                char Ack_To_Be_Sent[HEADER_SIZE];
                sprintf(Ack_To_Be_Sent, "Ack_To_Be_Sent%d;", BufIndex);
                sendto(s, Ack_To_Be_Sent, strlen(Ack_To_Be_Sent), 0, (struct sockaddr *) & sender_address, sizeof(sender_address));
                
                //store out of order packets in the buffer
                out_of_order_index[Window_Index] = BufIndex;
                left_out_of_order[Window_Index] = Num;
                for(int i=0; i<bytes_received-l-1; i++){
                    Not_In_Order_Mes[Num] = *curr;
                    curr++;
                    Num++;
                }
                right_out_of_order[Window_Index] = Num;
                Window_Index++;
            }

            memset(buf, 0, BUFSIZE);
        }
    }

    close(s);
    fclose(fp);
	printf("%s received.", destinationFile);
    return;
}

















/*
 * 
 */
int main(int argc, char** argv) {

    unsigned short int udpPort;

    if (argc != 3) {
        fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);
        exit(1);
    }

    udpPort = (unsigned short int) atoi(argv[1]);

    reliablyReceive(udpPort, argv[2]);
}

