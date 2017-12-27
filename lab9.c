#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Initializes Node which is of type struct node
typedef struct node {
	char name[20];
	int size;
	struct node *next;
}Node;

//Initialize global pointers to a Node structure for the front and rear
Node *front, *rear = NULL;

int counter = 0;
int found = 0;

//declare prototypes
void reserve(char *filename, int size);
void assign();
void display();
void dcontents(char *filename);
void getinput(char *filename);
void*  writetofile(void* arg);
void clear();

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char *argv[]) {

	char filename[50];
	FILE *inp;

	if (argc == 1){
	    printf("The name of the file is missing!");
	    return 1;
	}

	getinput(argv[1]);
		
  char enter[3];

  printf("Command Options\nR or r - reserve\nA or a - assign\nD or d- display\nC or c- displaybinarycontents\nQ or q - quit\n");
    
  pthread_t tid;
  pthread_create(&tid, NULL, writetofile, (void*) argv[1]);

  while  (1){                                               //loops continuously until q is entered
    printf("Enter a command:\n");
    fpurge(stdin);
    scanf("%s", enter);
    switch (enter[0]){                                      //takes in input from user and uses switch command to call different functions
      case 'R' :
      case 'r' :
        reserve(NULL,0);
        break;
      case 'A' :
      case 'a' :
        assign();
        break;
      case 'D' :
      case 'd' :
        display();
        break;
      case 'C' :
      case 'c' : 
        dcontents(argv[1]);
        break;
      case 'Q' :
      case 'q' :
        clear();
        pthread_cancel(tid);
        printf("%d",found);
        return 0;
      default:
      printf("Not a valid command\n");
      break;
    }
  }
		return 0;
}

void getinput(char *filename){

	FILE *inp;
	char name[50];
	int size = 0;
	Node *p;
	
	if ((p = (Node*)malloc(sizeof(Node))) == NULL){         //allocates memory for struct p;
		printf("Memory could not be allocated\n");
		return;
	}
	
	inp = fopen(filename,"rb");	
	if(inp == NULL){
	    return;
	}

	else{
		while(fread(p,sizeof(Node),1,inp) == 1){
			if (p == NULL){
			    return;
			}
			if(strcmp(p->name,"No")== 0){
			     return;
			}
			reserve(p->name,p->size);
		}
		fclose(inp);
	}

}

void*  writetofile(void* arg){

	char *filename = (char*) arg;
	//char filename = *filename_ptr;
	char header[] = "Name       Group Size\n---------------------\n";
	char norsvn[] = "No reservations";
	Node *p;
	FILE *inp;
	
	inp = fopen(filename,"wb");
	fwrite(header, 1, sizeof(header),inp);
	fclose(inp);	

	while (1){
		
		pthread_mutex_lock(&mymutex);
		inp = fopen(filename,"wb");

		p = front;

			if(p == NULL){
			   fwrite(norsvn,1, sizeof(norsvn), inp);
			}
                      			      
			while (p != NULL){					      //Loop through list until it reaches the end
			   
			    fwrite(p,sizeof(Node),1, inp);    //prints out the name and size data in p
			    p = p->next;                                                     //p becomes a pointer to the next node in the list
			}

		fclose(inp);	
		pthread_mutex_unlock(&mymutex);	
		sleep(5);
	}
}


void reserve (char *name, int size){
	
	char input[50];
	int length;
	int numinparty;
	Node *temp, *p;
	
	if(name == NULL){
		printf("Enter name:\n");
		scanf("%s", input);
	}
	else
		strcpy(input,name);
	
	length = strlen(input);                                        //Assign the length of the string to the int length

		if(length < 20){           						                       //Makes sure that the string is less than 19 characters long

			if ((counter) <10){
        temp = front;                                            //Checks to see if list is full		
					
				while (temp != NULL){                                    //goes through list from front until NULL and compares the name in each struct with the user input

					if(strcmp(temp->name,input) == 0){		
						printf("Name already exists\n");        
						return;
          }
          
					temp = temp->next;                                    //temp gets assign address of the next struct in the list
				}

				pthread_mutex_lock(&mymutex);	

				if ((p = (Node*)malloc(sizeof(Node))) == NULL){         //allocates memory for struct p;
					printf("Memory could not be allocated\n");
					return;
				}

				if(size == 0){
					printf("Enter number in party:\n");
					scanf("%d", &numinparty);            			            //Asks for input from the user for size
				}
				else{
					numinparty = size;
				}

				if ((numinparty) <= 0){         			                  //Checks to see if the value entered is 0
					printf("Must enter a valid party size\n");
					return;
				}		

				else 
				  p->size = (numinparty);
	
				
				strcpy(p->name, input);					                        //copies the value of the name into input 
				(counter)++;				                                    //Increment counter
							
				if (front == NULL){					                            //checks to see if the list is empty
					front = rear = p;                                     //front equals rear which equals p because there is only one struct in the list
					rear->next = NULL;	
				}

				else {
					rear->next = p;					                             //p is added to the end of the list
					rear = rear->next;				                           //rear now points to the added struct 
					rear->next = NULL;			                             //rear is now last struct in list so its next pointer points to NULL
				}	

				pthread_mutex_unlock(&mymutex);
			}
								
													
			else {				                                          // program can not take any more reservations because there are already 10 reservations in the list
				printf("No more available reservations\n");
			}
		}
		else {
			printf("Name is too big\n");						                //If the the user enters a string greater than 19 characters it
		}							                                            //will tell user that its too big
  }
  
//function implementations 

void dcontents(char *filename){

	FILE *inp;
	Node *p;
	int i = 0;

	if ((inp = fopen (filename, "rb")) == NULL){
		return;
	}

	if ((p = (Node*)malloc(sizeof(Node))) == NULL){         //allocates memory for struct p;
		printf("Memory could not be allocated\n");
		return;
  }
  
  while (fread(p,sizeof(Node),1,inp) == 1){
    if (p == NULL){
        return;
    }
      printf("%s     %d\n",p->name, p->size); 
  }
    
	fclose(inp);	
}

void assign () {

	int tablesize;

	printf("Enter table size\n");						
	scanf("%d", &tablesize);						                            //Take input from user
		
	if (tablesize <= 0){						                                //Insures that the value entered is not 0 or negative
		printf("Must enter a valid table size\n");
		return;			
	}

	else {								                                          //If counter is zero than there is no one on the waitlist
	 	if((counter) == 0){
			printf("No one on waitlist\n");
			return;	
		}
	}
	
	pthread_mutex_lock(&mymutex);
	//Initialize local pointers
	Node *p;
	Node *q;
	
	for(p = front; p!= NULL && p->size>tablesize; p = p->next){     //sets p to front and then goes through list until it reaches the node with the desired size or the end of the list
		q = p;                                                        //q becomes the value of the previous node 
	}

	if (p == NULL){
	
		pthread_mutex_unlock(&mymutex);
		return;
	}

	if (front == NULL){                                             //Checks to see if the list is empty
	
		pthread_mutex_unlock(&mymutex);
		printf("Sorry list is empty\n");
		return;
	}

	if (front == rear){						                                //Checks if there is only one node in the list
		
		pthread_mutex_unlock(&mymutex);
		front = rear = NULL;
		free(p);
			
		return;
	}

	if (p == front) {						                                //Special case if the node that needs to be deleted is the front of the list
		
		front = front->next;
		free(p);
		pthread_mutex_unlock(&mymutex);
		return;
	}

	if (p == rear) {						                              //Special case if the node that needs to be deleted is at the rear of the list

		rear = q;
		rear->next = NULL;
		free(p);
		pthread_mutex_unlock(&mymutex);
		return;
	}

	 q->next = p->next; 						                          //assigns the next pointer of the previous node to point to the node after the one being deleted
	 free(p);							                                    //deletes the node
	(counter)--;							                                //Node has been deleted so the value in counter is decremented
	pthread_mutex_unlock(&mymutex);

}

void display(){

	Node *p;

	if (front == NULL)                                            //If the list is empty as front is a NULL pointer
	{
	    printf("No reservations\n");
	    return;
	}

	p = front;                      			      //Assign value of p to front 

	while (p != NULL)					      //Loop through list until it reaches the end
	{
	    printf("Name: %s \nNumber in party: %d\n", p->name, p->size);    //prints out the name and size data in p 
	    p = p->next;                                                     //p becomes a pointer to the next node in the list
	}

}

void clear(){

	Node *p,*q;
	p = front;
	
	if(p == NULL){
	    return;
	}
	else{
    while(p!= NULL){
      q = p->next;
      free(p);
      found++;
      p = q;
	  }	
  }
}








