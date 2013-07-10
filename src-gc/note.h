#define NOTE_PRIVATE         -1
#define NOTE_GENERAL          0
#define NOTE_IMMORTAL         1
#define NOTE_IDEAS            2
#define NOTE_BUGS             3
#define NOTE_JOBS             4
#define NOTE_ANNOUNCEMENTS    5
#define NOTE_INFORMATION      6 
#define NOTE_STORIES          7
#define NOTE_CHANGES          8
#define NOTE_WANTED           9
#define NOTE_FIXED           10
#define NOTE_CODE            11
#define NOTE_AVATAR          12
#define NOTE_CLAN            13
#define MAX_NOTEBOARD        14


class Note_Data
{
 public:
  note_data*   next;
  char*        from;
  char*        title;
  char*        message;
  time_t       date;
  int          noteboard;

  Note_Data( );
  ~Note_Data( );
};



void    load_notes        ( void );
void    load_notes        ( clan_data* );
void    save_notes        ( int, clan_data* = NULL );
void    save_notes        ( clan_data* );


extern const char* noteboard_name [ MAX_NOTEBOARD ];

