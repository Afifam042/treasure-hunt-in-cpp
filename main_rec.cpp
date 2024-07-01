#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>

using namespace std;

/*/ ******************** Defines ******************** /*/

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#define TILE_BLANK '0'
#define TILE_WALL 'X'
#define TILE_GASPER 'G'
#define TILE_DEVIL 'D'
#define TILE_TREASURE 'T'
#define TILE_PORTAL 'P'
#define TILE_PATH '@'

#define GAME_STARTED 1
#define GAME_WON 2
#define GAME_LOST 3
#define GAME_KICKED 4

/*/ ******************** Data ******************** /*/

/**
 * Represents a room, and its related map data ingame
 */
typedef struct Room {
    int rows, cols;
    int startRow, startCol;
    int portalRow, portalCol;
    char **map;
} Room;

/*/ ******************** Func ******************** /*/

/**
 * Prints the game state as well as the room number on the console
 * @param room
 * @param r
 * @param c
 * @param roomNum
 */
void display(const Room &room, int r, int c, int roomNum) {
    system("cls");

    // Top Boundary
    cout << "+";
    for (int col = 0; col < room.cols; col++)
        cout << "---";
    cout << "+" << endl;

    for (int row = 0; row < room.rows; row++) {
        // Left Boundary
        cout << "|";
        for (int col = 0; col < room.cols; col++) {
            // Padding before cell
            cout << " ";
            // Inside of cell
            switch (room.map[row][col]) {
                case TILE_BLANK:
                    cout << ((r == row && c == col) ? "*" : " ");
                    break;
                case TILE_WALL:
                    cout << "#";
                    break;
                case TILE_GASPER:
                    cout << "G";
                    break;
                case TILE_DEVIL:
                    cout << "D";
                    break;
                case TILE_PORTAL:
                    cout << "O";
                    break;
                case TILE_TREASURE:
                    cout << "$";
                    break;
                case TILE_PATH:
                    cout << ".";
                    break;
                default:
                    cout << " ";
            }
            // Padding after cell
            cout << " ";
        }
        // Right Boundary
        cout << "|";
        cout << endl;
    }

    // Bottom Boundary
    cout << "+";
    for (int col = 0; col < room.cols; col++)
        cout << "---";
    cout << "+";

    cout << endl << "Room Number: " << roomNum;
}

int play( const Room rooms[], int roomNum, int prevRoom, int treasureRoom, int numOfRooms, int r, int c){

    int pr = r, pc = c;

    display(rooms[roomNum], r, c, roomNum);

    char key;
    key = _getch();
    key = _getch();

    // Take input and update coordinates

    switch (key) {
        case KEY_UP:
            if (r > 0) r = r - 1;
            break;
        case KEY_DOWN:
            if (r < rooms[roomNum].rows - 1) r = r + 1;
            break;
        case KEY_LEFT:
            if (c > 0) c = c - 1;
            break;
        case KEY_RIGHT:
            if (c < rooms[roomNum].cols - 1) c = c + 1;
            break;
        default:
            break;
    }

    // Check tile and perform action accordingly

    switch (rooms[roomNum].map[r][c]) {
        case TILE_WALL:
            r = pr;
            c = pc;
            break;
        case TILE_DEVIL:
            return GAME_LOST;
            break;
        case TILE_TREASURE:
            return GAME_WON;
            break;
        case TILE_PORTAL: {
            char coin = ((rand()%10) < 5 ? 'T' : 'H');
            if (coin == 'T') {
                if (prevRoom == -1) {
                    return GAME_KICKED;
                } else {
                    int t = roomNum;
                    roomNum = prevRoom;
                    prevRoom = t;
                }
            } else {
                prevRoom = roomNum;
                for (int i = 0; i < 10 && roomNum == prevRoom; i++)
                    roomNum = (rand() % numOfRooms);
                r = rooms[roomNum].portalRow;
                c = rooms[roomNum].portalCol;
            }
        }
            break;
        case TILE_GASPER:
            prevRoom = roomNum;
            roomNum = treasureRoom;
            r = rooms[roomNum].startRow;
            c = rooms[roomNum].startCol;
            break;
        case TILE_BLANK:
        default:
            break;
    }

    int res = play(rooms, roomNum, prevRoom,treasureRoom, numOfRooms, r, c);
    if(res == GAME_WON){
        if(rooms[roomNum].map[r][c] == TILE_BLANK)
        rooms[roomNum].map[r][c] = TILE_PATH;
    }
    return res;
}

/*/ ******************** Main ******************** /*/

int main() {

    /*
     * Read Map Data for all the rooms
     */

    ifstream input("hauntedhouse.txt");
    srand((unsigned) time(0));

    int numOfRooms;
    char tmp;
    string roomFilePath;

    input >> numOfRooms;
    getline(input, roomFilePath);

    Room *rooms = new Room[numOfRooms];

    int treasureRoom = -1;

    for (int i = 0; i < numOfRooms; i++) {
        getline(input, roomFilePath);
        ifstream roomDataFile(roomFilePath);

        roomDataFile >> rooms[i].rows >> tmp >> rooms[i].cols;
        roomDataFile >> rooms[i].startRow >> tmp >> rooms[i].startCol;

        rooms[i].map = new char *[rooms[i].rows];

        for (int r = 0; r < rooms[i].rows; r++) {
            rooms[i].map[r] = new char[rooms[i].cols];
            for (int c = 0; c < rooms[i].cols; c++) {
                roomDataFile >> rooms[i].map[r][c];
                if (rooms[i].map[r][c] == TILE_TREASURE)
                    treasureRoom = i;
                if (rooms[i].map[r][c] == TILE_PORTAL) {
                    rooms[i].portalRow = r;
                    rooms[i].portalCol = c;
                }
            }
        }

        roomDataFile.close();
    }

    input.close();

    /*
     * Game Loop
     */

    int roomNum = 0;
    int r = rooms[roomNum].startRow, c = rooms[roomNum].startCol;
    int gameState = GAME_STARTED;

    gameState = play(rooms, 0, -1, treasureRoom, numOfRooms, r, c);

    display(rooms[roomNum], r, c, roomNum);
    cout << endl;

    /*
     * Display End screen
     */

    if (gameState == GAME_WON) {
        cout << "Treasure found at Room: " << roomNum << " on " << r << ", " << c;
    } else if (gameState == GAME_LOST) {
        cout << "Found the devil at: " << r << " " << c;
    } else if (gameState == GAME_KICKED) {
        cout << "Kicked out at: " << r << " " << c;
    }

    cout << endl;

    system("pause");
    return 0;
}