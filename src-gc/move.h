#define DIR_NORTH                  0
#define DIR_EAST                   1
#define DIR_SOUTH                  2
#define DIR_WEST                   3
#define DIR_UP                     4
#define DIR_DOWN                   5
#define DIR_EXTRA                  6


class direction_type
{
 public:
  const char*       name;
  int            reverse;
  char*      arrival_msg;  
  char*            where;
};


extern const direction_type  dir_table    [ ];


/*
 *   MOVEMENT
 */


void   move_char          ( char_data*, int, bool );
void   enter_water        ( char_data* );
bool   speed_walking      ( char_data*, char* );


extern const char*           leaving_msg  [ ];


/*
 *   PATHS
 */


class Path_Data
{
 public:
  char_data*  summoner;
  int             step;
  int           length;
  int*      directions;  

  Path_Data( );
  ~Path_Data( );
};


void   exec_range      ( char_data*, int, path_func*,
                         char* = empty_string );










