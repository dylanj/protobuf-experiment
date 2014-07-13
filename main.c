	#define MAX_USERS 32

enum log_level {
  DEBUG,  // HANDY INFO FOR ME
	INFO, // IMPORTANT INFO
  WARNING, // SOMETHING THAT PROBABLY SHOULDNT HAPPEN
  ERROR,  // SOMETHING THAT SHOULD NOT HAPEN
  FATAL, // KILLS THE PROGRAM
  OFF, //
};

/* set_log_level(log_level.debug); */

typedef struct {
  int x;
  int y;
  int z;
} vec3_t;

typedef struct {
  int x;
  int y;
} vec2_t;

typedef struct entity_s {
  int mesh_id;

  vec3_t position;
  vec3_t velocity;
} entity_t;

typedef struct {
  int socket;
  int ip_address;
  int ping;
  int joined_at;
} net_info_t;

typedef struct user_s {
  net_info_t net_info;
  char *username;
  char *real_name;

} user_t;

typedef struct net_msg_t {
  int type;
  void *data;
} net_msg_t;

user_t *users[MAX_USERS];

void zlog( log_level level, char *msg ) {
  printf( msg );
}

int main(int argc, char **argv ) {
  zlog(INFO, "Starting server");

}
