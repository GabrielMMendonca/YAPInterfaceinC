#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int c_db_query(void)
{
    MYSQL *connHandler = (MYSQL *)YAP_IntOfTerm(YAP_ARG1);
    char *query = YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2));

    MYSQL_RES *result;
    // conn = mysql_init(NULL);
    if (connHandler == NULL)
    {
        printf("erro no init\n");
        return FALSE;
    }
    if (mysql_query(connHandler, query))
    {
        finish_with_error(connHandler);
    }

    result = mysql_store_result(connHandler);

    if (result == NULL)
    {
        finish_with_error(connHandler);
    }
    else
    {
        print_result_set(result);
    }

    if (!Yap_unify(YAP_ARG3, MkIntegerTerm((int)result)))
    {
        mysql_free_result(result);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void init_my_predicates()
{
     YAP_UserCPredicate("c_db_query",c_db_query,3);
}