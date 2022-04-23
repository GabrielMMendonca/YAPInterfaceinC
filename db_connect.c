#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int c_db_connect(void)
{
    YAP_Term arg_host = YAP_ARG1;
    YAP_Term arg_user = YAP_ARG2;
    YAP_Term arg_passwd = YAP_ARG3;
    YAP_Term arg_database = YAP_ARG4;
    YAP_Term arg_conn = YAP_ARG5;
    MYSQL *conn;
    char *host = YAP_AtomName(YAP_AtomOfTerm(arg_host));
    char *user = YAP_AtomName(YAP_AtomOfTerm(arg_user));
    char *passwd = YAP_AtomName(YAP_AtomOfTerm(arg_passwd));
    char *database = YAP_AtomName(YAP_AtomOfTerm(arg_database));
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        printf("erro no init\n");
        return FALSE;
    }
    if (mysql_real_connect(conn, host, user, passwd, database, 0, NULL, 0) == NULL)
    {
        printf("erro no connect\n");
        return FALSE;
    }
    if (!YAP_Unify(arg_conn, YAP_MkIntTerm((int)conn)))
        return FALSE;
    return TRUE;
}

void init_my_predicates()
{
     YAP_UserCPredicate("c_db_connect",c_db_connect,5);
}