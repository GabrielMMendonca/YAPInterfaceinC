#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
#include <mysql/mysql.h>

int c_db_row(void)
{
    YAP_Term arg_result = YAP_ARG1;
    YAP_Term arg_arity = YAP_ARG2;
    YAP_Term arg_fieldTypes = YAP_ARG3;

    int i = 0;
    unsigned int arity = arg_arity;
    MYSQL_ROW row;
    MYSQL_RES *res_set = (MYSQL_RES *)YAP_IntOfTerm(YAP_ARG1);
    MYSQL_FIELD *field;

    if ((row = mysql_fetch_row(res_set)) != NULL)
    {
        YAP_Term head = YAP_ARG2;
        YAP_Term list = YAP_ARG2;
        for (i = 0; i < arity; i++)
        {
            field = mysql_fetch_field(res_set);
            head = YAP_HeadOfTerm(list);
            list = YAP_TailOfTerm(list);

            if (!YAP_Unify(head, YAP_MkAtomTerm(YAP_LookupAtom(row[i]))))
            {
                return FALSE;
            }        
            
        }
        return TRUE;
    }

    mysql_free_result(res_set);
    YAP_cut_fail();
}