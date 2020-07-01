/*
 * This is VE280 Project 2, SU2020.
 * Written by Ziqiao Ma and Zhuoer Zhu.
 * Latest Update: 5/29/2020.
 * All rights reserved.
 */

#include "server_type.h"

// TODO: Declare your functions in this header file.
void read_username(const string & u_name, string & dir, unsigned int & num, string user_name_list[], User_t **users);
void read_log(User_t **u, int num_user, string log_name);
void read_info_all(User_t **u, int num_user, const string & dir);
void read_userinfo(User_t *uu, int seq, const string & d);
void read_posts(User_t *uu,  int seq, const string & d);
void refresh(User_t *user);
void visit(User_t* ul, User_t* ur);
void post(User_t* user, Post_t new_post);
void trending(User_t *u, const int n, const int num_users);
void comment(User_t *u1_ptr, User_t *u2_ptr, unsigned int n, string c);
void unpost(User_t* u, unsigned int id);
void follow(User_t* u1, User_t* u2);
void unfollow(User_t* u1, User_t* u2);
void like(User_t* u1, User_t* u2, unsigned int id);
void unlike(User_t *u1, User_t *u2, unsigned int id);
void uncomment(User_t *u1, User_t *u2, unsigned int pid, unsigned int cid);

/* Helper Functions */
// sort
void simple_sort(Tag_t t[], unsigned int num);
// Printing
void printUser(User_t& user, const string& relationship);
void printPost(Post_t& post);
void printTag(const Tag_t& tag, unsigned int rank);

