class Command_Data
{
 public:
  char*   string;
  bool   ordered;

  Command_Data( char* msg, bool bit ) {
    string = alloc_string( msg, MEM_PLAYER );
    ordered = bit;
    return;
    }

  ~Command_Data( ) {
    free_string( string, MEM_PLAYER );
    return;
    }
};


class Command_Queue
{
 private:
  int             size;
  command_data**  list;

 public:
  Command_Queue( ) {
    size = 0;
    list = NULL;
    } 

  friend int entries( command_queue& queue ) {
    return queue.size;
    }

  friend command_data* pop( command_queue& queue ) {
    if( queue.size == 0 )
      return NULL;
    command_data* cmd = queue.list[0];
    remove( queue.list, queue.size, 0 );
    return cmd;
    }

  friend void push( command_queue& queue, char* string, bool ordered ) {
    command_data* cmd = new command_data( string, ordered );
    insert( queue.list, queue.size, cmd, queue.size );       
    return;
    } 

  friend void clear( command_queue& queue ) {
    int i;
    for( i = 0; i < queue.size; i++ )
      delete queue.list[i];
    delete [] queue.list;
    queue.list = NULL;
    queue.size = 0;
    return;
    }
};
  






