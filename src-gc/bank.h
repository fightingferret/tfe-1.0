class Locker_Data : public Thing_Data
{
 public:
  char_data*  owner;

  virtual int type( ) { return LOCKER_DATA; }

  Locker_Data( char_data* ch ) {
    owner = ch;
    return;
    }
};
