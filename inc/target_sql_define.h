#ifndef UK_SQL_DEFINE_H
#define UK_SQL_DEFINE_H
const QString SQL_CREATE_ADMIN = " CREATE TABLE IF NOT EXISTS man_system_admin("\
							" tid         INTEGER    PRIMARY KEY AUTOINCREMENT,"\
							" createstamp DATETIME,"\
							" updatestamp DATETIME,"\
							" tcode       TEXT(20),"\
							" tname       TEXT(32),"\
							" talias      TEXT(32),"\
							" tmemo       TEXT(254),"\
							" userid      INTEGER,"\
							" lname       TEXT(32),"\
							" lpassword   TEXT(32),"\
							" tstatus     INTEGER)";

const QString SQL_INSERT_ADMIN = " INSERT INTO man_system_admin(createstamp,updatestamp,"\
                                      "tname,talias,lname,lpassword,tstatus)"\
                                      "values(datetime('now','localtime'),datetime('now','localtime'),"\
									  "?,?,?,?,0)";

const QString SQL_QUERY_ADMIN = "SELECT  t.tid,t.lname "\
							" FROM   MAN_SYSTEM_ADMIN t "\
							" where  t.lname = '%1' "\
							" and    t.lpassword = '%2'";
const QString SQL_MODIFY_ADMIN_PWD =" update  MAN_SYSTEM_ADMIN"\
							" set   lpassword='%1' "\
							" where  tid = %2 ";

const QString SQL_CREATE_ROOM = "CREATE TABLE IF NOT EXISTS  man_room ("\
							" tid         INTEGER    PRIMARY KEY AUTOINCREMENT,"\
							" tstatus     INTEGER,"\
							" createstamp TEXT,"\
							" updatestamp TEXT,"\
							" floor       INTEGER,"\
							" type        INTEGER,"\
							" price       REAL,"\
							" discount    REAL,"\
							" setting     TEXT(128),"\
							" talias      TEXT(254),"\
							" tcode       TEXT(254),  "\
							" tmemo       TEXT(254))";

const QString SQL_INSERT_TARGET  = " INSERT INTO man_target(insert_timestamp,"\
							"  name,type,subtype,devno,gallery)"\
							"  values(datetime('now','localtime'),"\
							"  ?,?,?,?,?)";



const QString SQL_CREATE_GUEST = " CREATE TABLE IF NOT EXISTS man_guest ( "\
							" tid      INTEGER     PRIMARY KEY AUTOINCREMENT, "\
							" tname    TEXT(32), "\
							" IdCard   TEXT(24)   UNIQUE, "\
							" talias   TEXT(32),"\
							" tcode    TEXT(32),"\
							" type     INTEGER,"\
							" gender   TEXT(8),"\
							" birth    DATE,"\
							" cardid   INTEGER(8),"\
							" mobile   TEXT(16)   UNIQUE,"\
							" checkin  DATETIME,"\
							" checkout DATETIME, "\
							" tmemo    TEXT(254),"\
							" tstatus  INTEGER,"\
							"  roomid   INTEGER); ";

const QString SQL_INSERT_GUEST = " INSERT INTO man_guest(createstamp,updatestamp,"\
"  IdCard,tname,gender,talias,type,tstatus)"\
"  values(datetime('now','localtime'),datetime('now','localtime'),"\
"  ?,?,?,?,?,0)";

const QString SQL_CREATE_BOOK = " CREATE TABLE IF NOT EXISTS man_book_room( "\
						"	tid         INTEGER   PRIMARY KEY AUTOINCREMENT,"\
						"	tstatus     INTEGER,"\
						"	createstamp DATETIME,"\
						"	updatestamp DATETIME,"\
						"	arrivestamp DATETIME,"\
						"	guest       TEXT(32),"\
						"	IdCard      TEXT(24),"\
						"	gender      TEXT(8),"\
						"	phone       TEXT(16),"\
						"	mobile      TEXT(16),"\
						"	room1       INTEGER,"\
						"	room2       INTEGER,"\
						"	room3       INTEGER)";

const QString SQL_INSERT_BOOK = " INSERT INTO man_book_room(createstamp,updatestamp,"\
"  IdCard,guest,gender,mobile,tstatus)"\
"  values(datetime('now','localtime'),datetime('now','localtime'),"\
"  ?,?,?,?,0)";

const QString SQL_CREATE_OBJTABLE = " CREATE TABLE IF NOT EXISTS MAN_OBJTABLE( "\
								" ID         INTEGER        PRIMARY KEY AUTOINCREMENT,"\
								" valid      INTEGER,"\
								" tableIndex INTEGER,"\
								" name       VARCHAR2(128),"\
								" aliasName  VARCHAR2(128))";

const QString SQL_DELETE_OBJTABLE = " DELETE FROM MAN_OBJTABLE "\
							" where  ID not in( "\
							" select t2.id "\
							" from   sqlite_master t1 "\
							" left join MAN_OBJTABLE t2 "\
							" on t1.name=t2.name) ";

const QString SQL_INSERT_OBJTABLE = " insert into MAN_OBJTABLE(tableindex, name, valid, aliasName) "\
							" select rowid, t.[name],1,t.[name] "\
							" from   sqlite_master t "\
							" where  t.[type] = 'table' "\
							" and    t.[name] not in "\
							" ( "\
							" select t1.name "\
							" from   MAN_OBJTABLE t1 "\
							" left join   sqlite_master t2 "\
							" on     t1.[name] = t2.[name] "\
							" ) ";


const QString SQL_QUERY_OBJTABLE = QString("select t.[ID],t.[name],t.[aliasName] from MAN_OBJTABLE t "\
	"   where t.[ID] "\
	"	not in( "\
	"	select t1.ID "\
	"	from      MAN_OBJTABLE t1 "\
	"	join      MAN_OBJFIELD t2 "\
	"	on       t1.id = t2.tableid "\
	"	)");

const QString SQL_QUERY_OBJTABLE_ALL = QString("select t.ID,t.tableIndex,t.valid ,t.name,t.aliasName from man_objtable t");


const QString SQL_CREATE_OBJFIELD = "   CREATE TABLE IF NOT EXISTS MAN_OBJFIELD( "\
								"	ID        INTEGER        PRIMARY KEY AUTOINCREMENT,"\
								"	valid     INTEGER,"\
								"	objectid  VARCHAR2(64),"\
								"	name      VARCHAR2(128),"\
								"	aliasName VARCHAR2(128),"\
								"	tableid   INTEGER,"\
								"	size      INTEGER,"\
								"	type      INTEGER	)";

const QString SQL_DELETE_OBJFIELD  = " delete from MAN_OBJFIELD t "\
								" where t.[ID] "\
								" not in( "\
								" select t2.ID "\
								" from      MAN_OBJTABLE t1 "\
								" join      MAN_OBJFIELD t2 "\
								" on        t1.id = t2.tableid "\
								" )";

const QString SQL_CREATE_CONFIG = "    CREATE TABLE  IF NOT EXISTS MAN_CONFIG ("\
								" ID       INTEGER        PRIMARY KEY AUTOINCREMENT,"\
								" valid    INTEGER        DEFAULT 1,"\
								" objectid VARCHAR2(64),"\
								" name     VARCHAR2(128),"\
								" memo     VARCHAR2(128),"\
								" strValue VARCHAR2(128),"\
								" intValue INTEGER        DEFAULT 0,"\
								" parent   INTEGER        DEFAULT - 1,"\
								" class    INTEGER        DEFAULT - 1,"\
								" subclass INTEGER        DEFAULT - 1,"\
								" sibling  INTEGER        DEFAULT - 1)";


const QString SQL_QUERY_OBJFIELD_ALL = QString("select t.ID,t.tableid,t.valid ,t.name,t.aliasName,t.type from man_objfield t");


const QString SQL_QUERY_TARGET = "  SELECT t1.tid,t1.name,t1.devno,t1.type,t1.subtype,t1.gallery,t1.status,t1.ipaddr,t1.port,t1.memo FROM MAN_TARGET t1";



#endif