#include "containers.h"
#include "header.h"
#include "read.h"
#include "semaphores.h"
#include <GL/glut.h>

#define shm_key 0x1236
#define totalContainers_key 0x1238
#define Container_ke 0x1239
#define storage_key 0x1240
#define family_keyy 0x1241
 // Adjust according to your expected maximum

Plane *shared_planes;
Container *shared_container;
totalContainers *shared_TotalContainer;
Storage *storage;
Family *FAMILLYshared;

void display(void);
void reshape(int w, int h);
void timer(int z);
void renderText(int, int, void *, char *);
void drawReceptionHall();
void drawSquaresWithContainers();
Config c;
void drawSquaresWithNumbers();
void update(int value);
void initTimer();
void updateTimer(int value);
void drawStorageArea();
void drawCollectingWorker();
void drawStorageValue();
void updateStorageValue(int value);
void mouseMotion(int x, int y);
void drawSplittingworker();
void drawSplitValue();
void updateSplit_value(int value);
void drawDistributedWorker();
void drawDistributedValue();
void updateDistributed_value(int value);
void drawFamilly();
void timerFunction(int value);
void timerC(int val);
void drawFamillyValue();
void updateFamilly_value(int value);



#define MAX_WORKERS 50 
int flagg =0;
char Distributed_value[100] = "Each worker carry 10 bags";
char Familly_value[100] = "Each Familly take 2 bags";
char Split_value[100] = "Split Value";
char storageValuee[100] = "Storage Value"; // Variable to hold the value
 int storageValue = 0;
float mouseX = 0.0f;
float mouseY = 0.0f;
int windowWidth = 1800;
int windowHeight = 1000;
char *drawer_current_time = "";
int fram_rate = 1000 / 120;
int remainingTime;   // Time remaining in milliseconds
char timeString[20]; // Buffer to hold the time display string

float yOffset = 0.0f;
float direction = 1.0f;
clock_t lastTime = 0;
int currentWorker = 0; // Current worker index to change color
clock_t lastTimeS = 0;
int currentWorkerS = 0;
clock_t lastTimeSS = 0;
int currentWorkerSS= 0;
float colors[60][3];  // Assuming total_points is 60
int num_collectingWorkers = 9;
int colorChangeTimer[MAX_WORKERS];  // Timer to keep track of color change
int yellowState[MAX_WORKERS];       // 0 = normal color, 1 = yellow color
 
int numberloop =0 ;
int flaggD =0;
clock_t lastTimeD = 0;
int currentWorkerD = 0;
clock_t lastTimeDD = 0;
int currentWorkerDD = 0;
clock_t lastTimeF = 0;
int currentWorkerF = 0;
int fllagfirst = 0;
int flagcolor =0;
int numf = 5;
int kill2 = 0;
int loopsplit = 0;
int split =10;


int collectertakeflag = 0;
int colectflag2 =0;
int lastcolllect =0;
//--------------------------------------------------------------------------------
// DISPLAY FUNCTION
void display() {
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Set text color to white
  glColor3f(1.0, 1.0, 1.0);

  // Position for the text
  glRasterPos2f(-0.8f, -17.9f);

  // Render the string
  for (char *c = timeString; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }

  glFlush();

  drawReceptionHall();
  drawSquaresWithNumbers();
  drawSquaresWithContainers();
  drawCollectingWorker();
  drawStorageArea(); // Draw the storage area box
  drawStorageValue();
  drawSplittingworker();
  drawSplitValue();
  drawDistributedWorker();
  drawDistributedValue();
  drawFamilly();
  drawFamillyValue();
  // Render the mouse coordinates
  char mouseCoords[50];
  sprintf(mouseCoords, "Mouse X: %.2f Y: %.2f", mouseX, mouseY);
  renderText(10, -18, GLUT_BITMAP_TIMES_ROMAN_24, mouseCoords);

  float startX = -22.0f;   // Starting X coordinate for the table
  float startY = 18.0f;    // Starting Y coordinate for the table
  float cellWidth = 6.0f;  // Width of each cell
  float cellHeight = 2.0f; // Height of each cell
                           // Render the titles for each row and column
  renderText(startX + cellWidth * 0.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Planes");
  renderText(startX + cellWidth * 1.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Conteiners");
  renderText(startX + cellWidth * 2.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Collecting Worker");
  renderText(startX + cellWidth * 3.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Storage Area");
  renderText(startX + cellWidth * 4.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Splitting Team");
  renderText(startX + cellWidth * 5.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Distribution Worker");
  renderText(startX + cellWidth * 6.5, startY + 0.5 * cellHeight,
             GLUT_BITMAP_HELVETICA_12, "Family");

  drawCollectingWorker();

  // Swap the buffers at the very end after all drawing commands
  glutSwapBuffers();
}

// MAIN FUNCTION

int main(int argc, char **argv) {

  c = read_config("userdefined.txt");
  printf("Number of Cargo Planes: %d\n", c.num_cargoPlanes);
  printf("Number of Collecting: %d\n", c.num_collectingTeams);
  printf("Number of SPLIT: %d\n", c.num_splittingWorkers);
  printf("Number of DISTRIBUTE: %d\n", c.num_distributingWorkers);
  printf("Number of FAMILLY: %d\n", c.numFamilies);
  printf("Number of bagtoeachworker: %d\n", c.bags_eachWorker);
  printf("Number of bagtoFamilly: %d\n", c.bags_eachFamily);

  


  // Attach shared memory and verify
  int shm_id =
      shmget(shm_key, c.num_cargoPlanes * sizeof(Plane), IPC_CREAT | 0666);
  if (shm_id < 0) {
    perror("shmget error");
    exit(1);
  }

  shared_planes = (Plane *)shmat(shm_id, NULL, 0);
  if (shared_planes == (Plane *)(-1)) {
    perror("shmat error");
    exit(1);
  }

  // SHARED MEM FOR CONTAINER
  int shmid2 = shmget(Container_ke, sizeof(Container), IPC_CREAT | 0666);
  if (shmid2 == -1) {
    perror("shmget for Container failed");
    exit(EXIT_FAILURE);
  }

  shared_container = (Container *)shmat(shmid2, NULL, 0);
  if (shared_container == (Container *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  // SHARED MEM FOR TOTAL CONTAINER
  int shmid3 =
      shmget(totalContainers_key, sizeof(totalContainers), IPC_CREAT | 0666);
  if (shmid3 == -1) {
    perror("shmget for Total Containers failed");
    exit(EXIT_FAILURE);
  }

  shared_TotalContainer = (totalContainers *)shmat(shmid3, NULL, 0);
  if (shared_TotalContainer == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  // SHARED MEM FOR STORAGE
  int shmid4 = shmget(storage_key, sizeof(Storage), IPC_CREAT | 0666);
  if (shmid4 == -1) {
    perror("shmget for Total Containers failed");
    exit(EXIT_FAILURE);
  }

  storage = (Storage *)shmat(shmid4, NULL, 0);
  if (storage == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }


  //SHARED MEM FOR FAMILLY
  
        //attach to the shared memory to assign the starvation rate
     int shmidF = shmget(family_keyy, sizeof(Family), 0666 | IPC_CREAT);

  if (shmidF == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  FAMILLYshared = (Family *)shmat(shmidF, NULL, 0);
  if (FAMILLYshared  == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  
  //sprintf("famillytake %d",FAMILLYshared->numOfBags);
  
  int storageshared =  storage->flourStorage;
 printf("shared storage %d\n", storageshared);
fflush(stdout);

  int bag=  storage->bags_number;
    
    
  printf("bag number in storage %d\n", bag);
  
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(1000, 1000);
  glutCreateWindow("Distributing wheat flour in nourthen");
  glLineWidth(5);
  glutReshapeFunc(reshape);
  initTimer(); // Initialize the timer
  glutDisplayFunc(display);
  glutTimerFunc(1000, updateTimer, 0);
  glutPassiveMotionFunc(mouseMotion);
  glutTimerFunc(0, timer, 0);
  glutTimerFunc(0, update, 0);
  
  glutTimerFunc(8000, updateStorageValue,0); // Start the timer with initial value 1
  glutTimerFunc(18000, updateSplit_value, 1);
  glutTimerFunc(23000, updateDistributed_value, 0);
  glutTimerFunc(27000, updateFamilly_value, 0);
  
  glutTimerFunc(1000, timerC, 0);
  // glutDisplayFunc(drawSquaresWithContainers);
  // glutTimerFunc(5000, timerFunction, 0);

  glutMainLoop();
  return 0;
}

//-------------------------------------------------------------------------------

// MOUSE AXIS
void mouseMotion(int x, int y) {
  // Convert pixel coordinates (x, y) to OpenGL coordinates
  mouseX = (x / (float)windowWidth) * 40.0f - 20.0f;
  mouseY = ((windowHeight - y) / (float)windowHeight) * 40.0f - 20.0f;
  glutPostRedisplay(); // Add this line to ensure the display function is called
                       // to update the rendering
}

// TIMER ON SCREEN
void initTimer() {
  remainingTime = 250 * 1000; // 3 minutes in milliseconds
}

void init() {
  // Set the clear color (background color) to black
  glClearColor(0.0, 0.0, 0.0, 1.0);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-20, 20, -20, 20);
  glMatrixMode(GL_MODELVIEW);
}

void timer(int z) {
  glutTimerFunc(fram_rate, timer, 0);
  glutPostRedisplay();
}

// TO PRINT TITLE ON SCREEN
void renderText(int x, int y, void *font, char *string) {
  glRasterPos2f(x, y);
  int len, i;
  len = strlen(string);
  for (i = 0; i < len && string[i] != '\0'; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}
// DRAW LINE OF SCREEN
void drawReceptionHall() {
  glBegin(GL_LINES);
  glVertex2f(-20.0, -14.0);
  glVertex2f(20.0, -14.0);
  glEnd();

  glBegin(GL_LINES);
  glVertex2f(-20.0, 17.0);
  glVertex2f(20.0, 17.0);
  glEnd();
}

//---------------------------------------------------------
// PLAN MOVE
void update(int value) {
  // Update the yOffset based on the direction
  yOffset += 0.05f * direction;

  // Check if the square reaches the top limit
  if (yOffset > 5.0f) {
    // Reverse the direction
    direction *= -1.0f;

    // Generate a random wait time between 0.1 seconds (100,000 microseconds) and 1 second (1,000,000 microseconds)
    srand(time(NULL));  // Seed the random number generator (ideally should be done once in main)
    int waitTime = (rand() % 900000) + 100000;

    // Pause execution for the random wait time
    usleep(waitTime);
  }
  // Check if the square reaches the bottom limit
  else if (yOffset < -5.0f) {
    // Reverse the direction without pausing
    direction *= -1.0f;
  }

  glutPostRedisplay(); // Mark the window as needing to be redisplayed
  glutTimerFunc(16, update, 0); // Set up the next timer for roughly 60 frames per second
}


// DRAW PLANES
void drawSquaresWithNumbers() {
  float squareSize = 0.8f;      // Define the size of the square
  float numberPosX = -18.5f;    // Starting X position for the squares
  float numberPosYStart = 9.5f; // Y position for the first square
  char numberText[10];          // Buffer to hold the number as a string

  // Calculate vertical spacing between squares
  float spacingY = 2.0f;

  // Define the colors
  float colors[][3] = {
      {1.0f, 0.0f, 1.0f}, // Pink RGB
      {0.0f, 0.0f, 1.0f}, // Blue RGB
      {1.0f, 1.0f, 0.0f}  // Yellow RGB
  };

  // Loop through the number of cargo planes
  for (int i = 0; i < c.num_cargoPlanes; i++) {
    float numberPosY =
        numberPosYStart + yOffset - (i * (squareSize + spacingY));
    // Set color for the current square
    glColor3fv(colors[i % 3]); // Cycle through the color array

    // Draw square
    glBegin(GL_QUADS);
    glVertex2f(numberPosX, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
    glVertex2f(numberPosX, numberPosY + squareSize / 2);
    glEnd();

    // Convert the number of the plane to text
    snprintf(numberText, sizeof(numberText), "%d", i + 1);

    // Set color for the text (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);

  }

  // Reset color to white (or any other color you'll use next)
  glColor3f(1.0f, 1.0f, 1.0f);
}

//--------------------------------------------------


void timerFunction(int value) {
  glutPostRedisplay();                  // Redraw the window
  glutTimerFunc(100, timerFunction, 0); // Schedule next update
}

void updateTimer(int value) {
  if (remainingTime > 0) {
    remainingTime -= 1000; // Decrease by one second each call

    int minutes = (remainingTime / 1000) / 60;
    int seconds = (remainingTime / 1000) % 60;

    // Format the time string to display as "Timer: XX m:XX s"
    sprintf(timeString, "Timer: %d m:%02d s", minutes, seconds);

    // Redisplay to update the screen
    glutPostRedisplay();

    // Register itself to be called again in 1000 milliseconds (1 second)
    glutTimerFunc(1000, updateTimer, 0);
  } else {
    // Exit the program when the timer reaches zero
    printf("Timer reached zero. Exiting program.\n");
    exit(0);
  }
}


//------------------------------------------
//Collecting worker 
void timerC(int val) {
    glutPostRedisplay();  // Request a new frame
    glutTimerFunc(1000, timer, 0);  // Approximate time to keep updating
}



void drawCollectingWorker() {
    float squareSize = 0.8f;       // Define the size of the square
    float numberPosX = -6.5f;      // Starting X position for the squares
    float numberPosYStart = 15.0f; // Y position for the first square
    char numberText[10];           // Buffer to hold the number as a string

    float spacingY = 2.0f; // Vertical spacing between squares

    // Define the colors
    float colors[][3] = {
        {0.7f, 0.7f, 1.0f}, // Very light blue
        {0.6f, 0.6f, 1.0f}, // Soft light blue
        {0.5f, 0.5f, 1.0f}, // Light blue
        {0.4f, 0.4f, 1.0f}, // Medium light blue
        {0.3f, 0.3f, 1.0f}, // True light blue
        {0.2f, 0.2f, 1.0f}, // Slightly darker light blue
        {0.1f, 0.1f, 1.0f}, // Medium dark light blue
        {0.0f, 0.0f, 1.0f}, // Dark blue
        {0.0f, 0.0f, 0.8f}  // Very dark blue
    };

    // Check if it's time to update the color of the next square
    clock_t currentTime = clock();
    if ((double)(currentTime - lastTime) / CLOCKS_PER_SEC > (currentWorker == 0 ? 5 : 3)) {
        currentWorker++;  // Move to the next worker
        lastTime = currentTime;  // Reset the last update time
        if (currentWorker > num_collectingWorkers) {
            currentWorker = 0; // Reset if all workers have changed color
        }
    }

    // Loop through the number of collecting workers
    for (int i = 0; i < num_collectingWorkers; i++) {
        float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
        if (i < currentWorker) {
            glColor3f(1.0f, 1.0f, 0.0f); // Set to yellow if updated
            collectertakeflag=1;
            if (i ==4){
            colectflag2 =1;
            
            
        
       
           }
           if( i == 7){
           lastcolllect =1;
                    
          
           }
        
        } else {
        
            glColor3fv(colors[i % 9]); // Original color if not yet updated
        }

        // Draw square
        glBegin(GL_QUADS);
        glVertex2f(numberPosX, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
        glVertex2f(numberPosX, numberPosY + squareSize / 2);
        glEnd();

         }
}





void drawStorageArea() {
  float boxWidth = 5.0f;   // Width of the box
  float boxHeight = 10.5f; // Height of the box
  float boxPosX = -2.5f;   // X position of the box
  float boxPosY = 3.5f;    // Y position of the box (Adjust as needed)

  glColor3f(1.0f, 0.0f, 1.0f);
  glBegin(GL_QUADS);
  glVertex2f(boxPosX, boxPosY);
  glVertex2f(boxPosX + boxWidth, boxPosY);
  glVertex2f(boxPosX + boxWidth, boxPosY + boxHeight);
  glVertex2f(boxPosX, boxPosY + boxHeight);
  glEnd();
}

void drawStorageValue() {
  glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
  glRasterPos2f(-2.2f, 0.3f);  // Adjust to position below the box
  for (char *c = storageValuee; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
}


void updateStorageValue(int value) {

  
  sprintf(storageValuee, "Storage Value: %d", storageValue );

  // Redisplay to update the screen
  glutPostRedisplay();
  storageValue += 20;
  // Check conditions and update color if necessary
  int total = shared_TotalContainer->totalContainers;
  if (storageValue == storage->flourStorage) {
       //storageValue = storageValue- storage->bags_number;
        sprintf(storageValuee, "Storage Value: %d", storage->flourStorage);
    
        return;  // Stop the function here to prevent further increments or timers
    }else{
  
    if (storageValue == 180 || storageValue == 360 ) {
        // If storageValue is 180, wait 12 seconds before calling updateStorageValue again
        storageValue = storageValue - 40;
        sprintf(storageValuee, "Storage Value: %d", storageValue );
        glutPostRedisplay();
        glutTimerFunc(10000, updateStorageValue, 0);
         storageValue = storageValue- storage->bags_number;
     
    } else {
        // Otherwise, continue to call updateStorageValue every 4 seconds
        glutTimerFunc(4000, updateStorageValue, 0);
    }
}
}

//--------------------------------------------------
//SPLIT



void drawSplittingworker() {
  float squareSize = 0.8f;       // Define the size of the square
  float numberPosX = 5.5f;       // Starting X position for the squares
  float numberPosYStart = 15.0f; // Y position for the first square
  char numberText[10];           // Buffer to hold the number as a string

  float spacingY = 2.0f; // Vertical spacing between squares

  // Define the colors
  float colors[][3] = {
      {1.0f, 1.0f, 0.7f}, // Very light yellow
      {1.0f, 1.0f, 0.6f}, // Soft light yellow
      {1.0f, 1.0f, 0.5f}, // Light yellow
      {1.0f, 1.0f, 0.4f}, // Medium light yellow
      {1.0f, 1.0f, 0.3f}, // True light yellow
      {1.0f, 1.0f, 0.2f}, // Slightly darker light yellow
      {1.0f, 1.0f, 0.1f}, // Medium dark light yellow
      {1.0f, 1.0f, 0.0f}, // Dark yellow
      {0.8f, 0.8f, 0.0f}  // Very dark yellow
  };

  // Loop through the number of collecting workers
  for (int i = 0; i < c.num_splittingWorkers; i++) {
    float numberPosY =
        numberPosYStart -
        (i * (squareSize + spacingY)); // Corrected position calculation

    // Set color for the current square
    glColor3fv(colors[i % 9]); // Cycle through the color array

}

if( flagg == 0){
// Check if it's time to update the color of the next square
     clock_t currentTimeS = clock();
        if ((double)(currentTimeS - lastTimeS) / CLOCKS_PER_SEC > (currentWorkerS == 0 ? 7 : 0.3)) {
            currentWorkerS++;  // Move to the next worker
            lastTimeS = currentTimeS;  // Reset the last update time
            if (currentWorkerS >= c.num_splittingWorkers) {
                currentWorkerS = 0;
                flagg = 1; // Change to else part after first full cycle
            }
    }

   for (int i = 0; i < c.num_splittingWorkers; i++) {
        float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
        glColor3fv(colors[i % 9]); // Set original color for all
        glBegin(GL_QUADS);
        glVertex2f(numberPosX, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
        glVertex2f(numberPosX, numberPosY + squareSize / 2);
        glEnd();

        snprintf(numberText, sizeof(numberText), "%d", i + 1);
        glColor3f(1.0f, 1.0f, 1.0f); // Reset text color
        // renderText(numberPosX + squareSize + 0.1, numberPosY, GLUT_BITMAP_HELVETICA_18, numberText); // Assuming renderText is defined
    }
  
}else {

    clock_t currentTimeSS = clock();
    if ((double)(currentTimeSS - lastTimeSS) / CLOCKS_PER_SEC > (currentWorkerSS == 0 ? 0.2 : 0.3)) {
        currentWorkerSS++;  // Move to the next worker
         loopsplit++;
        lastTimeSS = currentTimeSS;  // Reset the last update time
        if(loopsplit == 8 || loopsplit ==20){
           split--;
           }
        if (currentWorkerSS > c.num_splittingWorkers) {
            currentWorkerSS = 0; // Reset if all workers have changed color
        }
        
    }

    // Loop through the number of collecting workers
    for (int i = 0; i < split ; i++) {
        float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
        if (i < currentWorkerSS) {
            glColor3f(0.4f, 0.4f, 1.0f); // Set to yellow if updated
           
        
        } else {
            glColor3fv(colors[i % 9]); // Original color if not yet updated
        }
        
        
    // Draw square
    glBegin(GL_QUADS);
    glVertex2f(numberPosX, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
    glVertex2f(numberPosX, numberPosY + squareSize / 2);
    glEnd();

    // Convert the number of the worker to text
    snprintf(numberText, sizeof(numberText), "%d", i + 1);
 // Set color for the text (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);


}
}

  // Reset color to white (or any other color you'll use next)
  glColor3f(1.0f, 1.0f, 1.0f);
}




void drawSplitValue() {
  glColor3f(1.0f, 1.0f, 1.0f); // Set text color to white
  glRasterPos2f(4.2f, -12.9f); // Adjust to position below the box
  for (char *c = Split_value; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
}

void updateSplit_value(int value) {
  // Update the variable's value
  sprintf(Split_value, "Split Value: %d", value);

  // Redisplay to update the screen
  glutPostRedisplay();
if (value ==  100) {
       value = value -30;
       sprintf(Split_value, "Split Value: %d",value );
     // storage->flourStorage - storage->bags_number;
       // return;  // Stop the function here to prevent further increments or timers
    }
  
  // Register itself to be called again after a specific time (e.g., 1000
  // milliseconds)
  glutTimerFunc(300, updateSplit_value,
                value + 1); // Increment the value for demonstration
}


//----------------------------------------------------------------------------------
//Distribute


void drawDistributedWorker() {
  float squareSize = 0.8f;       // Define the size of the square
  float numberPosX = 11.5f;      // Starting X position for the squares
  float numberPosYStart = 15.0f; // Y position for the first square
  char numberText[10];           // Buffer to hold the number as a string

  float spacingY = 2.0f; // Vertical spacing between squares

  // Define the colors
  float colors[][3] = {
      {1.0f, 0.7f, 0.7f}, // Very light pink
      {1.0f, 0.6f, 0.6f}, // Soft light pink
      {1.0f, 0.5f, 0.5f}, // Light pink
      {1.0f, 0.4f, 0.4f}, // Medium light pink
      {1.0f, 0.3f, 0.3f}, // True light pink
      {1.0f, 0.2f, 0.2f}, // Slightly darker light pink
      {1.0f, 0.1f, 0.1f}, // Medium dark light pink
      {1.0f, 0.0f, 0.0f}, // Dark pink
      {0.8f, 0.0f, 0.0f}  // Very dark pink
  };

  // Loop through the number of collecting workers
  for (int i = 0; i < c.num_distributingWorkers; i++) {
    float numberPosY =
        numberPosYStart -
        (i * (squareSize + spacingY)); // Corrected position calculation

    // Set color for the current square
    glColor3fv(colors[i % 9]); // Cycle through the color array
}
   if( flaggD == 0){
// Check if it's time to update the color of the next square
     clock_t currentTimeD = clock();
        if ((double)(currentTimeD - lastTimeD) / CLOCKS_PER_SEC > (currentWorkerD == 0 ? 2 : 3)) {
            currentWorkerD++;  // Move to the next worker
            lastTimeD = currentTimeD;  // Reset the last update time
            if (currentWorkerD >= c.num_distributingWorkers) {
                currentWorkerD = 0;
                flaggD = 1; // Change to else part after first full cycle
            }
    }

   for (int i = 0; i < c.num_distributingWorkers; i++) {
        float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
        glColor3fv(colors[i % 9]); // Set original color for all
        glBegin(GL_QUADS);
        glVertex2f(numberPosX, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
        glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
        glVertex2f(numberPosX, numberPosY + squareSize / 2);
        glEnd();

        snprintf(numberText, sizeof(numberText), "%d", i + 1);
        glColor3f(1.0f, 1.0f, 1.0f); // Reset text color
        // renderText(numberPosX + squareSize + 0.1, numberPosY, GLUT_BITMAP_HELVETICA_18, numberText); // Assuming renderText is defined
    }
  
}else {

    clock_t currentTimeDD = clock();
    if ((double)(currentTimeDD - lastTimeDD) / CLOCKS_PER_SEC > (currentWorkerDD == 0 ? 1 : 3)) {
        currentWorkerDD++;  // Move to the next worker
        lastTimeDD = currentTimeDD;  // Reset the last update time
        if (currentWorkerDD > c.num_distributingWorkers) {
            currentWorkerDD = 0; // Reset if all workers have changed color
        }
    }

    // Loop through the number of collecting workers
    for (int i = 0; i < c.num_distributingWorkers; i++) {
        float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
        if (i < currentWorkerDD) {
            glColor3f(1.0f, 0.0f, 1.0f); // Set to yellow if updated
           
        
        } else {
            glColor3fv(colors[i % 9]); // Original color if not yet updated
        }
        
        
    // Draw square
    glBegin(GL_QUADS);
    glVertex2f(numberPosX, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
    glVertex2f(numberPosX, numberPosY + squareSize / 2);
    glEnd();

    // Convert the number of the worker to text
    snprintf(numberText, sizeof(numberText), "%d", i + 1);
 // Set color for the text (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);


}
}

  // Reset color to white (or any other color you'll use next)
  glColor3f(1.0f, 1.0f, 1.0f);
}


void drawDistributedValue() {
  glColor3f(1.0f, 1.0f, 1.0f);  // Set text color to white
  glRasterPos2f(10.2f, -12.9f); // Adjust to position below the box
  for (char *c = Distributed_value; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
}

void updateDistributed_value(int value) {
  // Update the variable's value
  sprintf(Distributed_value, "Distributed_value: %d", value);

  // Redisplay to update the screen
  glutPostRedisplay();

//if (value > storage->bags_number) {
       
     //  sprintf(Distributed_value, "Distributed_value: %d", value);
      //storage->flourStorage - storage->bags_number;
     /// value = value - storage->bags_number;
      // return;  // Stop the function here to prevent further increments or timers
    //}
  // Register itself to be called again after a specific time (e.g., 1000
  // milliseconds)
  if (fllagfirst  == 0){
  glutTimerFunc(5000, updateDistributed_value, value +10); // Increment the value for demonstration
  fllagfirst =1;
  
  
  }else{
   glutTimerFunc(5000, updateDistributed_value, value +8); 
  }
  
}






void drawFamilly() {
  float squareSize = 0.8f;       // Define the size of the square
  float numberPosX = 17.0f;      // Starting X position for the squares
  float numberPosYStart = 15.0f; // Y position for the first square
  char numberText[10];           // Buffer to hold the number as a string

  float spacingY = 2.0f; // Vertical spacing between squares

  // Define the colors
  float colors[][3] = {
      {1.0f, 0.8f, 1.0f}, // Very light purple
      {0.9f, 0.7f, 1.0f}, // Light purple
      {0.8f, 0.6f, 1.0f}, // Medium light purple
      {0.7f, 0.5f, 1.0f}, // True light purple
      {0.6f, 0.4f, 1.0f}, // Lighter purple
      {0.5f, 0.3f, 1.0f}, // Lightest purple
      {0.4f, 0.2f, 1.0f}, // Very lightest purple
      {0.3f, 0.1f, 1.0f}, // Extremely light purple
      {0.7f, 0.2f, 1.0f}, // Pale purple
      {0.6f, 0.1f, 0.9f}, // Very pale purple
      {0.5f, 0.1f, 0.8f}, // Light pale purple
      {0.4f, 0.1f, 0.7f}  // Lighter pale purple
  };

  // Loop through the number of collecting workers
  for (int i = 1; i <  c.numFamilies; i++) {
    float numberPosY =
        numberPosYStart -
        (i * (squareSize + spacingY)); // Corrected position calculation

    // Set color for the current square
    glColor3fv(colors[i % 12]); // Cycle through the color array

    // Draw square
    glBegin(GL_QUADS);
    glVertex2f(numberPosX, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
    glVertex2f(numberPosX, numberPosY + squareSize / 2);
    glEnd();

    // Convert the number of the worker to text
    snprintf(numberText, sizeof(numberText), "%d", i + 1);

    // Set color for the text (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);

if (FAMILLYshared[i].numOfBags !=0){
clock_t currentTimeF = clock();
    if ((double)(currentTimeF - lastTimeF) / CLOCKS_PER_SEC > (currentWorkerF == 0 ? 10 : 3)) {
        currentWorkerF++;  // Move to the next worker
         numberloop++;

    
      
        lastTimeF = currentTimeF;  // Reset the last update time
         if ( numberloop == 4 ||numberloop == 10 ){
        numf--;
     }
        if (currentWorkerF > c.numFamilies) {
            currentWorkerF = 0; // Reset if all workers have changed color
        }
    }

for (int i = 1; i <= numf; i++) {
    float numberPosY = numberPosYStart - (i * (squareSize + spacingY));
   if (i < currentWorkerF) {
            
        
        glColor3f(1.0f, 0.4f, 0.4f); // Set to red if 'i' is less than 'currentWorkerF'
        }
        
    
    else {
    
        glColor3fv(colors[i % 9]); // Use the original color if not updated
    }
        
        
    // Draw square
    glBegin(GL_QUADS);
    glVertex2f(numberPosX, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY - squareSize / 2);
    glVertex2f(numberPosX + squareSize, numberPosY + squareSize / 2);
    glVertex2f(numberPosX, numberPosY + squareSize / 2);
    glEnd();

    // Convert the number of the worker to text
    snprintf(numberText, sizeof(numberText), "%d", i + 1);
 // Set color for the text (white in this case)
    glColor3f(1.0f, 1.0f, 1.0f);
    

}
   
    
  }
  

  // Reset color to white (or any other color you'll use next)
  glColor3f(1.0f, 1.0f, 1.0f);
}
}


void drawFamillyValue() {
  glColor3f(1.0f, 1.0f, 1.0f);  // Set text color to white
  glRasterPos2f(10.2f, -10.9f); // Adjust to position below the box
  for (char *c = Familly_value; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
}


void updateFamilly_value(int value) {
  // Update the variable's value
  sprintf(Familly_value, "Familly_value: %d", value);

  // Redisplay to update the screen
  glutPostRedisplay();
//for (int i = 1; i <6; i++) {

//   sprintf("Family ID %d has %d bags.\n", FAMILLYshared[i].id, FAMILLYshared[i].numOfBags);
 // }
  
  
  glutTimerFunc(5300, updateFamilly_value,  value +2);
  
  }
              
              
              // DRAW CONTAINERS

void drawSquaresWithContainers() {
  if (!shared_planes) {
    fprintf(stderr, "Shared memory not attached.\n");
    return;
  }

  int columns = 5;
  int rows = 12;
  int total_points = columns * rows;

  // Define array to store color state for each point
  float colors[total_points][3]; // Array to hold RGB values

 // Initialize all points to white
for (int i = 0; i < total_points; i++) {
    colors[i][0] = 1.0; // R
    colors[i][1] = 1.0; // G
    colors[i][2] = 1.0; // B
}

// Update color for the first 5 points to blue



for (int i = 0; i < 5; i++) {
    if (i < total_points) { // Ensure within range
        colors[i][0] = 0.0; // R
        colors[i][1] = 0.0; // G
        colors[i][2] = 1.0; // B (blue)
      
    }
}

// Update color for the last 5 points to r



for (int i = 53; i < total_points; i++) {
    if (i >= 0) { // Ensure within range
        colors[i][0] = 1.0; // R
        colors[i][1] = 0.0; // G
        colors[i][2] = 0.0; // B (red)
    }
}

// Check conditions and update other colors if necessary
int total = shared_TotalContainer->totalContainers;

for (int i = 1; i <= total; i++) {
  if (shared_container[i].dropped == 1) {
    int baseIndex = (i - 1) * 2; // Calculate the base index for the pair of points
    if (baseIndex < total_points - 1) { // Ensure the second point is also within range
      // Set first point of pair to blue
      colors[baseIndex][0] = 0.0; // R
      colors[baseIndex][1] = 0.0; // G
      colors[baseIndex][2] = 1.0; // B (blue)
      colors[baseIndex + 2][0] =0.0; // R
      colors[baseIndex + 2][1] = 0.0; // G
      colors[baseIndex + 2][2] = 1.0; // B (red)
      colors[baseIndex + 3][0] =0.0; // R
      colors[baseIndex + 3][1] = 0.0; // G
      colors[baseIndex + 3][2] = 1.0; // B (red)
        colors[baseIndex + 4][0] =0.0; // R
      colors[baseIndex + 4][1] = 0.0; // G
      colors[baseIndex + 4][2] = 1.0; // B (red)
      
      if (collectertakeflag  ==  1 ){ //white
      
       colors[baseIndex + 1][0] =1.0; // R
      colors[baseIndex + 1][1] = 1.0; // G
      colors[baseIndex + 1][2] = 1.0; // B (red)
      // Set second point of pair to red
      colors[baseIndex + 2][0] =0.0; // R
      colors[baseIndex + 2][1] = 0.0; // G
      colors[baseIndex + 2][2] = 1.0; // B (red)
      colors[baseIndex + 3][0] =0.0; // R
      colors[baseIndex + 3][1] = 0.0; // G
      colors[baseIndex + 3][2] = 1.0; // B (red)
        colors[baseIndex + 4][0] =0.0; // R
      colors[baseIndex + 4][1] = 0.0; // G
      colors[baseIndex + 4][2] = 1.0; // B (red)
      
       colors[baseIndex + 5][0] =0.0; // R
      colors[baseIndex + 5][1] = 0.0; // G
      colors[baseIndex + 5][2] = 1.0; // B (red)
      
       colors[baseIndex + 6][0] =0.0; // R
      colors[baseIndex + 6][1] = 0.0; // G
      colors[baseIndex + 6][2] = 1.0; // B (red)
      
       colors[baseIndex + 7][0] =0.0; // R
      colors[baseIndex + 7][1] = 0.0; // G
      colors[baseIndex + 7][2] = 1.0; // B (red)
      
   if (colectflag2 ==1){
   //blue
    colors[baseIndex + 1][0] =0.0; // R
      colors[baseIndex + 1][1] = 0.0; // G
      colors[baseIndex + 1][2] = 1.0; // B (red)
      
       // Set second point of pair to red
      colors[baseIndex + 2][0] =0.0; // R
      colors[baseIndex + 2][1] = 0.0; // G
      colors[baseIndex + 2][2] = 1.0; // B (red)
      colors[baseIndex + 3][0] =0.0; // R
      colors[baseIndex + 3][1] = 0.0; // G
      colors[baseIndex + 3][2] = 1.0; // B (red)
        colors[baseIndex + 4][0] =0.0; // R
      colors[baseIndex + 4][1] = 0.0; // G
      colors[baseIndex + 4][2] = 1.0; // B (red)
      
      colors[baseIndex + 5][0] =0.0; // R
      colors[baseIndex + 5][1] = 0.0; // G
      colors[baseIndex + 5][2] = 1.0; // B (red)
      
       colors[baseIndex + 6][0] =0.0; // R
      colors[baseIndex + 6][1] = 0.0; // G
      colors[baseIndex + 6][2] = 1.0; // B (red)
      
       colors[baseIndex + 7][0] =0.0; // R
      colors[baseIndex + 7][1] = 0.0; // G
      colors[baseIndex + 7][2] = 1.0; // B (red)
        colors[baseIndex + 8][0] =0.0; // R
      colors[baseIndex + 8][1] = 0.0; // G
      colors[baseIndex + 8][2] = 1.0; // B (red)
      
       colors[baseIndex + 9][0] =0.0; // R
      colors[baseIndex + 9][1] = 0.0; // G
      colors[baseIndex + 9][2] = 1.0; // B (red)
      
       colors[baseIndex + 10][0] =0.0; // R
      colors[baseIndex + 10][1] = 0.0; // G
      colors[baseIndex + 10][2] = 1.0; // B (red)
      
        colors[baseIndex + 11][0] =0.0; // R
      colors[baseIndex + 11][1] = 0.0; // G
      colors[baseIndex + 11][2] = 1.0; // B (red)
      
       colors[baseIndex + 12][0] =0.0; // R
      colors[baseIndex + 12][1] = 0.0; // G
      colors[baseIndex + 12][2] = 1.0; // B (red)
      
       colors[baseIndex + 13][0] =0.0; // R
      colors[baseIndex + 13][1] = 0.0; // G
      colors[baseIndex + 13][2] = 1.0; // B (red)
       colors[baseIndex + 14][0] =0.0; // R
      colors[baseIndex + 14][1] = 0.0; // G
      colors[baseIndex + 14][2] = 1.0; // B (red)
      
       colors[baseIndex + 15][0] =0.0; // R
      colors[baseIndex + 15][1] = 0.0; // G
      colors[baseIndex + 15][2] = 1.0; // B (red)
      
       colors[baseIndex + 16][0] =0.0; // R
      colors[baseIndex + 16][1] = 0.0; // G
      colors[baseIndex + 16][2] = 1.0; // B (red)
      
      colectflag2 =0;
     
   }
  if (lastcolllect ==1){
    colors[baseIndex + 1][0] =1.0; // R
      colors[baseIndex + 1][1] = 1.0; // G
      colors[baseIndex + 1][2] = 1.0; // B (red)
      
      colors[baseIndex + 8][0] =0.0; // R
      colors[baseIndex + 8][1] = 0.0; // G
      colors[baseIndex + 8][2] = 1.0; // B (red)
      
       colors[baseIndex + 9][0] =0.0; // R
      colors[baseIndex + 9][1] = 0.0; // G
      colors[baseIndex + 9][2] = 1.0; // B (red)
      
       colors[baseIndex + 10][0] =0.0; // R
      colors[baseIndex + 10][1] = 0.0; // G
      colors[baseIndex + 10][2] = 1.0; // B (red)
      lastcolllect =0;
      
  }
   
   
    }else{
    
     // Set second point of pair to red
      colors[baseIndex + 1][0] =0.0; // R
      colors[baseIndex + 1][1] = 0.0; // G
      colors[baseIndex + 1][2] = 1.0; // B (red)
      
       colors[baseIndex + 2][0] =0.0; // R
      colors[baseIndex + 2][1] = 0.0; // G
      colors[baseIndex + 2][2] = 1.0; // B (red)
      colors[baseIndex + 3][0] =0.0; // R
      colors[baseIndex + 3][1] = 0.0; // G
      colors[baseIndex + 3][2] = 1.0; // B (red)
        colors[baseIndex + 8][0] =0.0; // R
      colors[baseIndex + 8][1] = 0.0; // G
      colors[baseIndex + 8][2] = 1.0; // B (red)
      
       colors[baseIndex + 9][0] =0.0; // R
      colors[baseIndex + 9][1] = 0.0; // G
      colors[baseIndex + 9][2] = 1.0; // B (red)
      
       colors[baseIndex + 10][0] =0.0; // R
      colors[baseIndex + 10][1] = 0.0; // G
      colors[baseIndex + 10][2] = 1.0; // B (red)
    
    }
       
  }
 
}

}

    
    // Draw points with updated colors
    glPointSize(10);
    glBegin(GL_POINTS);

    // Coordinates shift
    int xShift = -14;
    int yShift = 2;
    int ySpace = -1;
    int xSpace = 1;

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        int index = i * columns + j;

        int x = j * xSpace + xShift;
        int y = i * 2 + yShift + (i * ySpace);

        // Set color for this point
        glColor3f(colors[index][0], colors[index][1], colors[index][2]);

        // Draw the point
        glVertex2i(x, y);

        if (index >= total_points - 1) {
          break;
        }
      }
    }
    glEnd();
  

 
}

