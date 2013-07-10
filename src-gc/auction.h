typedef class   Auction_Data       auction_data;


class Auction_Data : public Thing_Data
{
 public:
  pfile_data*    seller;
  pfile_data*     buyer;
  int               bid;
  int             proxy;
  int              time;
  int              slot;
  bool          deleted;

  Auction_Data( ) {
    record_new( sizeof( auction_data ), MEM_AUCTION );
    buyer   = NULL;
    deleted = FALSE;
    time    = 50;
    slot    = 1;
    proxy   = 0;
    }

  ~Auction_Data( ) {
    record_delete( sizeof( auction_data ), MEM_AUCTION );
    }

  int Type ( ) { return AUCTION_DATA; }

  int minimum_bid( ) {
    if( buyer == NULL && !deleted )
      return bid;
    return max( 21*bid/20, bid+5 );
    }

  friend void add_time( auction_data* auction ) {
    if( auction->time < 30 )
      auction->time = max( 5, auction->time+2 );
    return;
    }
};


extern auction_array auction_list;


int   free_balance      ( player_data*, auction_data* = NULL );
void  clear_auction     ( pfile_data* );
void  auction_message   ( char_data* );
void  auction_update    ( void );

























