Release Notes 
SoftForNet Solutions Ltd.
Bogdan Fiedur
Bogdan@softfornet.com

This is freeware.

Any suggestions to improve this utility are welcomed.


This utility allows for sending email by specifying all needed information in SENDMAIL.INI file. INI file has to be placed in windows directory. This program takes as an argument an email address of receiving person. e.g. [softmail.exe webmaster@softfornet.com]. This program can be called from a Perl script, dll, batch file , command line, an other EXE or can be run as standalone application. In future releases this program will be included as part of form processing dll and will allow for sending email with confirmation to person who submitted the form. 


This is how SENDMAIL.INI file looks . 

[SMTP]
Server=your mail server
Port=25 

[UserInfo]
Name=Your name
Address=your email address

[MailInfo]
Subject=Whatever subject you want
Messagepath=path of your message file 
Attachment=path of your attachment file (just one)