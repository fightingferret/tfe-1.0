TFE 1.0

There are three patches of source available -
src-gc     the original source from greyclaw... this does not compile
           as is, but is kept for reference

src-msvc   the original source hacked (by Seltha) to compile under
           Windows using MS Visual C++ 6.0 ... compile as a console
           application and don't forget to add the wsock32.lib file
           to your project settings.

src-unix   the original source hacked (by Marsupilami and Seltha) to
           compile under Linux using gcc version egcs-2.91.66
           19990314/Linux (egcs-1.1.2 release)

An executable for Windows is included for quick access, and a god is
thrown in for good measure:

Connect (via Zmud/telnet) to 'localhost' on port '23' or '4000'.

Log in as: newimmortal
Password: newimmortal

Create your new character... and in the newimmortal session:
  trust <player> 99
  advance <player> 99

Now save with your new player, then reboot the MUD and you should be
good to go.

If you want others to be able to create accounts, you can try using the
softmail utility for Windows that is also included.  To get it set up,
you need to edit the sendmail.ini file and place it in the windows directory.

Be careful, I think there are some issues with \r\n and \n\r in this
current version (especially when running under windows).  You should
keep a backup of your working files just in case.

Enjoy,
-- Seltha [www.seltha.net]

PS: you may need to delete the "create.dir" files after extracting the files,
they are just there to make sure the folders are created.
