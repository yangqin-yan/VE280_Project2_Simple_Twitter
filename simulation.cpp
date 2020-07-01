/*
 * This is VE280 Project 2, SU2020.
 * Written by Ziqiao Ma and Zhuoer Zhu.
 * Latest Update: 5/29/2020.
 * All rights reserved.
 */

#include "simulation.h"

void read_username(const string & u_name, string & dir, unsigned int & num, string user_name_list[], User_t **users){
    ifstream fr1;
    fr1.open(u_name);
    try{
        if(!fr1.is_open()){
            //Exception
            ostringstream oStream;
            oStream << "Error: Cannot open file " + u_name + "!" << endl;
            throw Exception_t(FILE_MISSING, oStream.str());
        }
    }
    catch(Exception_t &exception){
        throw;
    }

    // operation of the username file.
    getline(fr1, dir);

    while(fr1) {
        try{
            if(num >= MAX_USERS){
                //Exception
                ostringstream oStream;
                oStream << "Error: Too many users!" << endl;
                oStream << "Maximal number of users is " + to_string(MAX_USERS) + "." << endl;
                fr1.close();
                throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
            }
        }
        catch(Exception_t &exception){
            fr1.close();
            throw;
        }

        getline(fr1, user_name_list[num]);
        if(fr1){
            // Initialize information of the server.
            (*users + num)->username = user_name_list[num];
            num++;
        }
    }
    fr1.close();
}

// MODIFIES: **u.
// EFFECTS: read all the information of users from all the user_info files and post files. Store all the values in **u.
//          Throws exceptions if any exceptions occur in the function.
// REQUIRES: **u which is a pointer to pointer to users, number of users, directory of the database.
void read_info_all(User_t **u, int num_user, const string & dir){
   try{
       for(int i = 0; i < num_user; i++){
           read_userinfo(*u, i, dir);
           read_posts(*u, i, dir);
       }
   }catch(Exception_t &exception){
       throw;
   }
}

// MODIFIES: *uu
// EFFECTS: Read the information of the userinfo file and initial the server of the part user info.
//          print error message and throw exceptions when the userinfo file cannnot open or some user
//          has too many posts or there are too many users or there are too many followings or followers.
// REQUIRES: Address of the user.
void read_userinfo(User_t *uu, int seq, const string & d){
    ifstream f;
    f.open(d + (uu + seq)->username + "/user_info");
    if(!f.is_open()) {
        //Exception
        ostringstream oStream;
        oStream << "Error: Cannot open file " << d << (uu + seq)->username << "/user_info!" << endl;
        throw Exception_t(FILE_MISSING, oStream.str());
    }

    // Read the first line which is the number of posts.
    string str;

    getline(f, str);
    (uu + seq)->num_posts = atoi(str.c_str());

    if((uu + seq)->num_posts > MAX_POSTS){
        //Exception: check whether number of posts listed in the userinfo file exceeds the maximal number of posts.
        ostringstream oStream;
        oStream << "Error: Too many posts for user " + (uu + seq)->username + "!" << endl;
        oStream << "Maximal number of posts is " + to_string(MAX_POSTS) + "." << endl;
        f.close();
        throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
    }

    // Read the following
    getline(f, str);
    (uu + seq)->num_following = atoi(str.c_str());

    if((uu + seq)->num_following > MAX_FOLLOWING){
        //Exception: check whether number of following listed in the userinfo file exceeds the maximal number of followings.
        ostringstream oStream;
        oStream << "Error: Too many followings for user " + (uu + seq)->username + "!" << endl;
        oStream << "Maximal number of followings is " + to_string(MAX_FOLLOWING) + "." << endl;
        f.close();
        throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
    }

    for(unsigned int i = 0; i < (uu + seq)->num_following; i++){
        getline(f, str);
        for(unsigned int j = 0; j < MAX_USERS; j++){
            if((uu + j)->username == str){
                (uu + seq)->following[i] = uu + j;
                break;
            }
        }
    }

    // Read the follower.
    getline(f, str);
    (uu + seq)->num_followers = atoi(str.c_str());
    if((uu + seq)->num_followers > MAX_FOLLOWERS){
        //Exception: check whether number of following listed in the userinfo file exceeds the maximal number of followings.
        ostringstream oStream;
        oStream << "Error: Too many followers for user " + (uu + seq)->username + "!" << endl;
        oStream << "Maximal number of followers is " + to_string(MAX_FOLLOWERS) + "." << endl;
        f.close();
        throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
    }

    for(unsigned int i = 0; i < (uu + seq)->num_followers; i++){
        getline(f, str);
        for(unsigned int j = 0; j < MAX_USERS; j++){
            if((uu + j)->username == str){
                (uu + seq)->follower[i] = uu + j;
                break;
            }
        }
    }

    f.close();
}

// MODIFIES: *uu
// EFFECTS: Read the post and store the data. When the post file cannot open or there are too mant tags of one post
//          or there are too many likes or comments in one post, the function will print the error messages and throws and exception.
// REQUIRES: address of the user and the number denoting which user.
void read_posts(User_t *uu, int seq, const string & d){

    for(unsigned int post_id = 0; post_id < (uu + seq)->num_posts; post_id++){
        ifstream f;
        string str;

        // Open the post file.
        f.open(d + (uu + seq)->username + "/posts/" + to_string(post_id + 1));

        if(!f.is_open()){
            //Exception
            ostringstream oStream;
            oStream << "Error: Cannot open file " + d + (uu + seq)->username + "/posts/" + to_string(post_id + 1)
                    + "!"<< endl;
            throw Exception_t(FILE_MISSING, oStream.str());
        }

        // Initialize the post
        (uu + seq)->posts[post_id].owner = uu + seq;
        (uu + seq)->posts[post_id].num_tags = 0;
        (uu + seq)->posts[post_id].num_comments = 0;
        (uu + seq)->posts[post_id].num_likes = 0;

        // read the title.
        getline(f, str);
        (uu + seq)->posts[post_id].title = str;

        // Read tags
        int tag_id = 0;

        getline(f, str);
        while(str[0] == '#'){
            bool repetition = false;
            string::iterator it;
            it = str.begin();
            str.erase(it);
            it = str.end() - 1;
            str.erase(it); // delete '#'
            for(unsigned int n = 0; n < (uu + seq)->posts[post_id].num_tags; n++){
                if((uu + seq)->posts[post_id].tags[n] == str){
                    repetition = true;
                }
            }
            if(repetition){
                continue;
            }
            (uu + seq)->posts[post_id].num_tags++;
            // Check whether there are too many tags.
            if((uu + seq)->posts[post_id].num_tags > MAX_TAGS){
                //Exception: check whether number of tags listed in the userinfo file exceeds the maximal number of tags.
                ostringstream oStream;
                oStream << "Error: Too many tags for post " + (uu + seq)->posts[post_id].title + "!" << endl;
                oStream << "Maximal number of tags is " + to_string(MAX_TAGS) + "." << endl;
                f.close();
                throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
            }
            (uu + seq)->posts[post_id].tags[tag_id] = str;
            tag_id++;
            getline(f,str);
        }

        // Read text
        (uu + seq)->posts[post_id].text = str;

        // Read likes
        getline(f, str);
        (uu + seq)->posts[post_id].num_likes = atoi(str.c_str());

        if((uu + seq)->posts[post_id].num_likes > MAX_LIKES){
            //Exception: check whether number of likes listed in the userinfo file exceeds the maximal number of likes.
            ostringstream oStream;
            oStream << "Error: Too many likes for post " + (uu + seq)->posts[post_id].title + "!" << endl;
            oStream << "Maximal number of likes is " + to_string(MAX_LIKES) + "." << endl;
            f.close();
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }

        for(unsigned int i = 0; i < (uu + seq)->posts[post_id].num_likes; i++){
            getline(f, str);
            for(unsigned int j = 0; j < MAX_USERS; j++){
                if(str == (uu + j)->username){
                    (uu + seq)->posts[post_id].like_users[i] = uu + j;
                    break;
                }
            }
        }

        // Read comments
        getline(f, str);
        (uu + seq)->posts[post_id].num_comments = atoi(str.c_str());

        if((uu + seq)->posts[post_id].num_comments > MAX_COMMENTS){
            //Exception: check whether number of comments listed in the userinfo file exceeds the maximal number of comments.
            ostringstream oStream;
            oStream << "Error: Too many comments for post " + (uu + seq)->posts[post_id].title + "!" << endl;
            oStream << "Maximal number of comments is " + to_string(MAX_COMMENTS) + "." << endl;
            f.close();
            throw Exception_t(CAPACITY_OVERFLOW, oStream.str());
        }

        for(unsigned int i = 0; i < (uu + seq)->posts[post_id].num_comments; i++){
            getline(f, str);
            for(unsigned int j = 0; j < MAX_USERS; j++){
                if(str == (uu + j)->username){
                    (uu + seq)->posts[post_id].comments[i].user = uu + j; // comment user
                    break;
                }
            }
            getline(f, str);
            (uu + seq)->posts[post_id].comments[i].text = str; // comment content.
        }

        f.close();
    }
}

// MODIFIES: **u
// EFFECTS: read the logfile and process each request. If the logfile cannot open, it will print out the error message
//          and throws an exception. It also handles the exception and then return.
// REQUIRES: ifstream.
void read_log(User_t **u, int num_user, string log_name){
    ifstream f;
    f.open(log_name);
    try{
        if(!f.is_open()){
            //Exception
            ostringstream oStream;
            oStream << "Error: Cannot open file " + log_name + "!" << endl;
            throw Exception_t(FILE_MISSING, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }
    // Read the logfile.
    while(!f.eof()){
        string str;
        getline(f, str);
        istringstream is(str);
        // pass the argument of the file
        string arg[5];
        is >> arg[0] >> arg[1] >> arg[2] >> arg[3] >> arg[4];
        // Judge each request
        if(arg[0] == "trending"){
            // pull trending request
            unsigned int topn = atoi(arg[1].c_str());
            trending(*u, topn, num_user);
            continue;
        }
        // Find the users related to the request.
        User_t *operationPtr = NULL;
        User_t *targetPtr = NULL;
        for(unsigned int i = 0; i < MAX_USERS; i++){
            if(arg[0] == (*u + i)->username){
                operationPtr = *u + i;
                break;
            }
        }
        if(arg[1] == "delete"){
            // pull delete request
            int post_id = atoi(arg[2].c_str());
            unpost(operationPtr, post_id);
            continue;
        }

        for(unsigned int i = 0; i < MAX_USERS && arg[2] != ""; i++){
            if(arg[2] == (*u + i)->username){
                targetPtr = *u + i;
                break;
            }
        }
        // Juedge the request
        if(arg[1] == "follow"){
            // pull follow request
            follow(operationPtr, targetPtr);
            continue;
        }
        if(arg[1] == "refresh"){
            // pull follow request
            refresh(operationPtr);
            continue;
        }
        if(arg[1] == "unfollow"){
            // pull unfollow request
            unfollow(operationPtr, targetPtr);
            continue;
        }
        if(arg[1] == "like"){
            // pull like request
            unsigned int post_id;
            post_id = atoi(arg[3].c_str());
            like(operationPtr, targetPtr, post_id);
            continue;
        }
        if(arg[1] == "unlike"){
            // pull unlike request
            unsigned int post_id;
            post_id = atoi(arg[3].c_str());
            unlike(operationPtr, targetPtr, post_id);
            continue;
        }
        if(arg[1] == "comment"){

            unsigned int post_id = stoi(arg[3].c_str());
            getline(f, str);
            // pull comment request
            comment(operationPtr, targetPtr, post_id, str);
            continue;
        }
        if(arg[1] == "uncomment"){
            // pull uncomment request
            unsigned int post_id, comment_id;
            post_id = atoi(arg[3].c_str());
            comment_id = atoi(arg[4].c_str());
            uncomment(operationPtr, targetPtr, post_id, comment_id);
            continue;
        }
        if(arg[1] == "post"){
            // pull post request
            Post_t npost;
            getline(f, str);
            npost.owner = operationPtr;
            npost.title = str;
            npost.num_tags = 0;
            npost.num_comments = 0;
            npost.num_likes = 0;
            getline(f, str);
            while(str[0] == '#'){
                bool repetition = false;
                string::iterator it;
                it = str.begin();
                str.erase(it);
                it = str.end() - 1;
                str.erase(it); // delete '#'
                for(unsigned int n = 0; n < npost.num_tags; n++){
                    if(npost.tags[n] == str){
                        repetition = true;
                        getline(f, str);
                        break;
                    }
                }
                if(repetition){
                    continue;
                }
                npost.tags[npost.num_tags] = str;
                getline(f, str);
                npost.num_tags++;
            }
            npost.text = str;
            post(operationPtr, npost);
            continue;
        }
        if(arg[1] == "visit"){
            // pull visit request
            visit(operationPtr, targetPtr);
            continue;
        }
    }
    f.close();
}

// EFFECTS: Print all the posts of the user and all the posts of the following of the user.
// REQUIRES: A user.
void refresh(User_t *user){
    cout << ">> refresh" << endl;
    for(unsigned int i = 0; i < user->num_posts; i++){
        printPost(user->posts[i]); // Print all the posts of the user
    }
    for(unsigned int i = 0; i < user->num_following; i++){
        for(unsigned int j = 0; j < user->following[i]->num_posts; j++){
            printPost(user->following[i]->posts[j]); // Print all the posts of the following of user
        }
    }
}

// EFFECTS: Print the relationship between 2 users and the information of the visited user.
// REQUIRES: The user and the user who will be visited.
void visit(User_t* ul, User_t* ur){
    string relationship = {};
    bool l_follow_r, r_follow_l, self;
    l_follow_r = false;
    r_follow_l = false;
    self = false;

    cout << ">> visit" << endl;

    // Judge whether the user visits himself.
    if(ul->username == ur->username){
        self = true;
    }

    for(unsigned int i = 0; i < ul->num_following && (!self); i++){
        if(ul->following[i]->username == ur->username){
            l_follow_r = true;
        }
    }
    for(unsigned int i = 0; i < ur->num_following && (!self); i++){
        if(ur->following[i]->username == ul->username){
            r_follow_l = true;
        }
    }


    // Judge the relationship
    if(!self){
        if(l_follow_r && r_follow_l){
            relationship = "friend";
        }
        else if(l_follow_r && !r_follow_l){
            relationship = "following";
        }
        else if(!l_follow_r){
            relationship = "stranger";
        }
    }

    printUser(*ur, relationship);
}

// MODIFIES: *u.
// EFFECTS: Print top n popular tags.
// REQUIRES: Posts of users and the number of the most popular tags the user wants to see.
void trending(User_t *u, const int n, const int num_users){
    cout << ">> trending" << endl;
    Tag_t pop_tag[5001];
    int total_tagnum = 0;
    for(int i = 0; i < num_users; i++){
        for(unsigned int j = 0; j < (u + i)->num_posts; j++){
            for(unsigned int tag_id = 0; tag_id < (u + i)->posts[j].num_tags; tag_id++){
                bool set_tag = false; // Used to judge whether the tag is processed successfully.
                for(int k = 0; k < total_tagnum; k++){
                    if((u + i)->posts[j].tags[tag_id] == pop_tag[k].tag_content){
                        pop_tag[k].tag_score += 5 + (u + i)->posts[j].num_likes + 3 * (u + i)->posts[j].num_comments;
                        set_tag = true;
                        break;
                    }
                }

                if(!set_tag && total_tagnum <= 5000){
                    // no existing tag, to create a new tag in the pop_tag list.
                    pop_tag[total_tagnum].tag_content = (u + i)->posts[j].tags[tag_id];
                    pop_tag[total_tagnum].tag_score = 5 + (u + i)->posts[j].num_likes + 3 * (u + i)->posts[j].num_comments;
                    total_tagnum++;
                }
            }
        }
    }

    simple_sort(pop_tag, total_tagnum);

    for(int i = 0; i < n; i++){
        if(i >= total_tagnum){
            break;
        }
        printTag(pop_tag[i], i + 1);
    }
}

// MODIFIES: *user.
// EFFECTS: write the post into a file and print out to the screen.
// REQUIRES: pointer to the user who wants to post and the structure of the post.
void post(User_t* user, Post_t new_post){

    cout << ">> post" << endl;
    user->posts[user->num_posts] = new_post;
    user->num_posts++;
    // printPost(new_post);
}

// MODIFIES: *u1_ptr, *u2_ptr.
// EFFECTS: pull comment request to the server, add a comment to a post from the user.
//          when there does not exist that post, print the error message and return.
// REQUIRES: the user list, user name of the commenter and user name of the post, the number of post, content of comment.
void comment(User_t *u1_ptr, User_t *u2_ptr, unsigned int n, string c){
    cout << ">> comment" << endl;

    try{
        if(n > u2_ptr->num_posts || n <= 0){
            ostringstream oStream;
            oStream << "Error: " + u1_ptr->username
                       + " cannot comment post #" + to_string(n) + " of " + u2_ptr->username + "!" << endl;
            oStream << u2_ptr->username + " does not have post #" + to_string(n) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }
    u2_ptr->posts[n - 1].comments[u2_ptr->posts[n - 1].num_comments].user = u1_ptr;
    u2_ptr->posts[n - 1].comments[u2_ptr->posts[n - 1].num_comments].text = c;
    u2_ptr->posts[n - 1].num_comments++;
}

// MODIFIES: *u.
// EFFECTS: delete the post. When the post does not exist, print the error message and return.
// REQUIRES: owner of the post and the post id.
void unpost(User_t* u, unsigned int id){
    cout << ">> delete" << endl;

    // Check whether user has posted post id.
    try{
        if(id > u->num_posts || id <= 0){
            ostringstream oStream;
            oStream << "Error: " + u->username
                       + " cannot delete post #" + to_string(id) + "!" << endl;
            oStream << u->username + " does not have post #" + to_string(id) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }

    for(unsigned int i = id; i < u->num_posts; i++){
        u->posts[i - 1].title = u->posts[i].title;
        u->posts[i - 1].text = u->posts[i].text;
        u->posts[i - 1].num_comments = u->posts[i].num_comments;
        for(unsigned int j = 0; j < u->posts[i - 1].num_comments; j++){
            u->posts[i - 1].comments[j] = u->posts[i].comments[j];
        }
        u->posts[i - 1].num_tags = u->posts[i].num_tags;
        for(unsigned int j = 0; j < u->posts[i - 1].num_tags; j++){
            u->posts[i - 1].tags[j] = u->posts[i].tags[j];
        }
        u->posts[i - 1].num_likes = u->posts[i].num_likes;
        for(unsigned int j = 0; j < u->posts[i - 1].num_likes; j++){
            u->posts[i - 1].like_users[j] = u->posts[i].like_users[j];
        }
    }
    u->num_posts--;
}

// MODIFIES: *u1, *u2.
// EFFECTS: Follow a user.
// REQUIRES: The user who want to follow a user and the user who will be followed.
void follow(User_t* u1, User_t* u2){
    cout << ">> follow" << endl;
    u2->follower[u2->num_followers] = u1;
    u2->num_followers++;
    u1->following[u1->num_following] = u2;
    u1->num_following++;
}

// MODIFIES: *u1, *u2.
// EFFECTS: Unfollow a user.
// REQUIRES: The user1 who want to unfollow a user2 and the user2 who is followed by user1.
void unfollow(User_t* u1, User_t* u2){
    cout << ">> unfollow" << endl;
    for(unsigned int i = 0; i < u2->num_followers; i++){
        if(u2->follower[i] == u1){
            // Delete the follower of the user on the list.
            u2->num_followers--;
            for(unsigned int j = i; j < u2->num_followers; j++){
                u2->follower[j] = u2->follower[j + 1];
            }
            u2->follower[u2->num_followers] = NULL;

            for(unsigned int j = 0; j < u1->num_following; j++){
                if(u1->following[j] == u2){
                    // Delete the following of the user in the list.
                    u1->num_following--;
                    for(unsigned int k = j; k < u1->num_following; k++){
                        u1->following[k] = u1->following[k + 1];
                    }
                    u1->following[u1->num_following] = NULL;

                    return;
                }
            }
        }
    }
    // Exceptions
}

// MODIFIES: *u1, *u2.
// EFFECTS: Like a post of the user. When there does not exist the post or the user has liked the post,
//          print the error messages and return.
// REQUIRES: user1 who want to like one post of user2, post id of user2.
void like(User_t* u1, User_t* u2, unsigned int id){
    cout << ">> like" << endl;
    try{
        if(u2->num_posts < id || id <= 0){
            //Exception: check whether the corresponding post of user 2 exists.
            ostringstream oStream;
            oStream << "Error: " + u1->username
            + " cannot like post #" + to_string(id) + " of " + u2->username + "!" << endl;
            oStream << u2->username + " does not have post #" + to_string(id) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
        for(unsigned int i = 0; i < u2->posts[id - 1].num_likes; i++){

            if(u2->posts[id - 1].like_users[i] == u1){
                // Exception: check whether user 1 has liked this post.
                ostringstream oStream;
                oStream << "Error: " + u1->username
                           + " cannot like post #" + to_string(id) + " of " + u2->username + "!" << endl;
                oStream << u1->username + " has already liked post #" + to_string(id) +
                " of " + u2->username + "." << endl;
                throw Exception_t(INVALID_LOG, oStream.str());
            }
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }
    u2->posts[id - 1].like_users[u2->posts[id - 1].num_likes] = u1;
    u2->posts[id - 1].num_likes++;
}

// MODIFIES: *u1, *u2.
// EFFECTS: Unike a post of the user. If the user has not liked the post or the post does not exist,
//          print the error message and return.
// REQUIRES: user1 who want to unlike one post of user2, post id of user2 and user2.
void unlike(User_t *u1, User_t *u2, unsigned int id){
    cout << ">> unlike" << endl;
    // Find whether user 2 has the post id.
    try{
        if(id > u2->num_posts || id <= 0){
            ostringstream oStream;
            oStream << "Error: " + u1->username
                       + " cannot unlike post #" + to_string(id) + " of " + u2->username + "!" << endl;
            oStream << u2->username + " does not have post #" + to_string(id) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }

    // Find whether user1 liked the corresponding post of user 2.
    for(unsigned int i = 0; i < u2->posts[id - 1].num_likes; i++){
        if(u2->posts[id - 1].like_users[i] == u1){
            for(unsigned int j = i; j < u2->posts[id - 1].num_likes - 1; j++){
                u2->posts[id - 1].like_users[j] = u2->posts[id - 1].like_users[j + 1];
            }
            u2->posts[id - 1].like_users[u2->posts[id - 1].num_likes - 1] = NULL;
            u2->posts[id - 1].num_likes--;
            return;
        }
    }

    // Exceptions(have not liked case)
    try{
        ostringstream oStream;
        oStream << "Error: " + u1->username
                   + " cannot unlike post #" + to_string(id) + " of " + u2->username + "!" << endl;
        oStream << u1->username + " has not liked post #" + to_string(id) +
                   " of " + u2->username + "." << endl;
        throw Exception_t(INVALID_LOG, oStream.str());
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }
}

// MODIFIES: *u1, *u2.
// EFFECTS: Uncomment a post of the user.s
// REQUIRES: user1 who want to uncomment one post of user2, post id of user2, comment of user2 and user2.
void uncomment(User_t *u1, User_t *u2, unsigned int pid, unsigned int cid){
    cout << ">> uncomment" << endl;
    // Check whether the post pid exist.
    try{
        if(pid > u2->num_posts){
            ostringstream oStream;
            oStream << "Error: " + u1->username
                       + " cannot uncomment comment #" + to_string(cid)
                       << " of post #"+ to_string(pid) + " posted by " + u2->username + "!" << endl;
            oStream << u2->username + " does not have post #" + to_string(pid) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }

    // Check whether there exists the comment cid of the post pid.
    try{
        if(cid > u2->posts[pid - 1].num_comments || cid <= 0){
            ostringstream oStream;
            oStream << "Error: " + u1->username
                       + " cannot uncomment comment #" + to_string(cid)
                    << " of post #"+ to_string(pid) + " posted by " + u2->username + "!" << endl;
            oStream << "Post #" << to_string(pid) + " does not have comment #" + to_string(cid) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }

    // Check whether it is user 1 that creates this comment.
    try{
        if(u2->posts[pid - 1].comments[cid - 1].user != u1){
            ostringstream oStream;
            oStream << "Error: " + u1->username
                       + " cannot uncomment comment #" + to_string(cid)
                    << " of post #"+ to_string(pid) + " posted by " + u2->username + "!" << endl;
            oStream << u1->username <<  " is not the owner of comment #" + to_string(cid) + "." << endl;
            throw Exception_t(INVALID_LOG, oStream.str());
        }
    }
    catch(Exception_t &exception){
        cout << exception.error_info;
        return;
    }

    // Uncomment operation.
    for(unsigned int i = cid - 1; i < u2->posts[pid - 1].num_comments; i++){
        u2->posts[pid - 1].comments[i] = u2->posts[pid - 1].comments[i + 1];
    }
    u2->posts[pid - 1].num_comments--;
    return;

}
/* Helper Functions */

// MODIFIES t[].
// EFFECTS: Sort different tags according to their scores in reduced order.
//          If the scores of two tags are the same, consider the ASCII order and
//          let the one with smaller ASCII code have the higher rank.
// REQUIRES: address of t, number of tags.
void simple_sort(Tag_t t[], unsigned int num){
    for(unsigned int i = 0; i < num; i++){
        for(unsigned int j = i + 1; j < num; j++){
            // Compare 2 tags
            if(t[i].tag_score > t[j].tag_score){
                continue;
            }
            else if(t[i].tag_score == t[j].tag_score){
                int alpha = 0;
                while(t[i].tag_content[alpha] == t[j].tag_content[alpha] && t[i].tag_content[alpha] != '\0'){
                    alpha++;
                }
                if(t[i].tag_content[alpha] == '\0' && t[j].tag_content[alpha] != '\0'){
                    continue;
                }
                if(t[i].tag_content[alpha] <= t[j].tag_content[alpha] && t[j].tag_content[alpha] != '\0'){
                    continue;
                }
            }
            Tag_t temp;
            temp.tag_content = t[i].tag_content;
            temp.tag_score = t[i].tag_score;
            t[i].tag_content = t[j].tag_content;
            t[i].tag_score = t[j].tag_score;
            t[j].tag_content = temp.tag_content;
            t[j].tag_score = temp.tag_score;
        }
    }
}

// Printing

// EFFECTS: print all the information of users
// REQUIRES: user structure and the relationship between users.
void printUser(User_t& user, const string& relationship){
    cout << user.username << endl;
    cout << relationship << endl;
    cout << "Followers: " << user.num_followers
         << "\nFollowing: " << user.num_following << endl;
}

// EFFECTS: print all the contents and components of post.
// REQUIRES: Corresponding post structure.
void printPost(Post_t& post){
    cout << post.owner->username << endl;
    cout << post.title << endl;
    cout << post.text << endl;
    cout << "Tags: ";
    for(unsigned int i = 0; i<post.num_tags; ++i){
        cout << post.tags[i] << " ";
    }
    cout << "\nLikes: " << post.num_likes << endl;
    if (post.num_comments > 0){
        cout << "Comments:" << endl;
        for(unsigned int i = 0; i<post.num_comments; ++i){
            cout << post.comments[i].user->username << ": "
                 << post.comments[i].text << endl;
        }
    }
    cout << "- - - - - - - - - - - - - - -" << endl;
}

// EFFECTS: print the tags in the trending section.
// REQUIRES: tag structure and its rank.
void printTag(const Tag_t& tag, unsigned int rank){
    cout << rank << " " << tag.tag_content << ": " << tag.tag_score << endl;
}
