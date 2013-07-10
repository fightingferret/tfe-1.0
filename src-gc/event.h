class Event_Data
{
 public:
  event_func*    func;
  event_data*    loop;
  thing_data*   owner;
  int            time;
  void*       pointer; 

  Event_Data( event_func*, thing_data* );
  Event_Data( );

  ~Event_Data( );
};


/* 
 *   ROUTINES
 */


#define QUEUE_LENGTH  50000


extern event_data*   event_queue  [ QUEUE_LENGTH ];
extern int            event_pntr;


void  extract        ( event_data* );
void  unlink         ( event_data* );
void  stop_events    ( thing_data*, event_func* = NULL );

void         add_queue      ( event_data*, int );
void         set_delay      ( char_data*, int );
const char*  name           ( event_data* );
int          time_till      ( event_data* );
void         event_update   ( );

void delay_wander    ( event_data* );
void execute_wander  ( event_data* );
void next_action     ( event_data* );
void execute_leap    ( event_data* );
void execute_drown   ( event_data* );
void execute_path    ( event_data* );
void update_affect   ( event_data* );
void execute_decay   ( event_data* );

