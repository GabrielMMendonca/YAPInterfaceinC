#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
#include <mysql/mysql.h>

int c_db_disconnect(void)
{
    YAP_Term arg_conn = YAP_ARG1;
    MYSQL *conn = (MYSQL *)YAP_IntOfTerm(arg_conn);
    mysql_close(conn);
    return TRUE;
}
