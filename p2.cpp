#include "simulation.h"

using namespace std;

int main(int argc, char *argv[]) {

    ofstream fw;
    string user_name, log_file;
    string user_name_list[MAX_USERS];
    string directory;
    User_t users[MAX_USERS];
    User_t *usersPtr = users;
    unsigned int num = 0;
    try{
        if(argc < 3){
            //Exception
            ostringstream oStream;
            oStream << "Error: Wrong number of arguments!" << endl;
            oStream << "Usage: ./p2 <username> <logfile>" << endl;
            throw Exception_t(INVALID_ARGUMENT, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return 0;
    }
    // read the input
    user_name = argv[1];
    log_file = argv[2];
    // open and read in the username file.
    try{
        read_username(user_name, directory, num, user_name_list, &usersPtr);
    }catch(Exception_t &exception){
        cout << exception.error_info;
        return 0;
    }

    // read information of each user
    try{

        read_info_all(&usersPtr, num, directory + "/");
    }catch(Exception_t &exception){
        cout << exception.error_info;
        return 0;
    }
    // Read request from the logfile.
    read_log(&usersPtr, num, log_file);
    return 0;
}