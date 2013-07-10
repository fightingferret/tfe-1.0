/*
 *   SHOPS
 */


class Shop_Data
{
 public:
  shop_data*        next;        
  room_data*        room;
  custom_data*    custom; 
  int             keeper;
  int          open_hour;       
  int         close_hour;      
  int             repair; 
  int              flags;
  int           buy_type  [ 2 ];
  int          materials;

  Shop_Data( ) {
    record_new( sizeof( shop_data ), MEM_SHOP );
    next   = NULL;
    custom = NULL;
    }

  ~Shop_Data( ) {
    record_delete( sizeof( shop_data ), MEM_SHOP );
    }
};


extern shop_data*  shop_list;


void          load_shops     ( void );
void          save_shops     ( void );
char_data*    active_shop    ( char_data* );


#define SHOP_STOLEN  0
#define MAX_SHOP     1


/*
 *   CUSTOMS
 */


class Custom_Data
{
 public:
  custom_data*      next;
  int               cost;
  obj_clss_data*    item;
  obj_clss_data*  ingred  [ MAX_INGRED ];
  int             number  [ MAX_INGRED ];

  Custom_Data( ) {
    record_new( sizeof( custom_data ), MEM_CUSTOM );
    item  = NULL;
    next  = NULL;
    cost  = 100;
    vzero( ingred, MAX_INGRED );
    vzero( number, MAX_INGRED );
    }

  ~Custom_Data( ) {
    record_delete( sizeof( custom_data ), MEM_CUSTOM );
    }
};


