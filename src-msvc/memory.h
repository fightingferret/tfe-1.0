/*
 *   MEMORY HEADER
 */


#define MEM_UNKNOWN                 0
#define MEM_ACCOUNT                 1
#define MEM_ACTION                  2
#define MEM_AFFECT                  3
#define MEM_ALIAS                   4
#define MEM_AREA                    5
#define MEM_ARRAY                   6
#define MEM_AUCTION                 7
#define MEM_BADNAME                 8
#define MEM_BAN                     9
#define MEM_CLAN                   10
#define MEM_CUSTOM                 11
#define MEM_DESCR                  12
#define MEM_ENEMY                  13
#define MEM_EVNT                   14
#define MEM_EXIT                   15
#define MEM_EXTRA                  16
#define MEM_HELP                   17
#define MEM_INFO                   18
#define MEM_LINK                   19
#define MEM_MEMORY                 20
#define MEM_MOBS                   21 
#define MEM_MPROG                  22
#define MEM_NOTE                   23
#define MEM_OBJ_CLSS               24
#define MEM_OBJECT                 25
#define MEM_OPROG                  26
#define MEM_PFILE                  27
#define MEM_PLAYER                 28
#define MEM_PROGRAM                29
#define MEM_QUEST                  30
#define MEM_QUEUE                  31            
#define MEM_RECOGNIZE              32
#define MEM_REQUEST                33 
#define MEM_RESET                  34
#define MEM_ROOM                   35
#define MEM_SHDATA                 36
#define MEM_SHOP                   37
#define MEM_SPECIES                38
#define MEM_SPELL                  39
#define MEM_TABLE                  40
#define MEM_TELL                   41
#define MEM_TRACK                  42
#define MEM_TRAINER                43
#define MEM_WIZARD                 44
#define MAX_MEMORY                 45


extern char* empty_string;


char*  alloc_string       ( const char*, int );
void   free_string        ( char*, int );
void   record_new         ( int, int );
void   record_delete      ( int, int );
void   extract            ( wizard_data*, int, const char* );


inline int offset( void* pntr1, void* pntr2 ) {
  return( int( pntr1 )-int( pntr2 ) );
  }


/*
 *   MEMORY STRUCTURES
 */


class mem_block
{
 public:
  class mem_block*    next;
  void*               pntr;
  int                 size;

  mem_block( int i ) {
    record_new( sizeof( mem_block ), MEM_MEMORY );
    size  = i;
    pntr  = new char[size];
    return;
    }

  ~mem_block( ) {
    record_delete( sizeof( mem_block ), MEM_MEMORY );
    delete [] pntr;
    return;
    }
};









