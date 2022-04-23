#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
#include <mysql/mysql.h>

int c_db_assert(void)
{
    YAP_Term arg_result = YAP_ARG1;
    YAP_Term arg_arity = YAP_ARG2;
    char *query = YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2));
    YAP_Term arg_conn = YAP_ARG3;

    YAP_Functor f_pred, f_assert;
    YAP_Term t_pred, *t_args, t_assert;
    MYSQL *conn = (MYSQL *)YAP_IntOfTerm(arg_conn);
    MYSQL_RES *res_set = (MYSQL_RES *)YAP_IntOfTerm(arg_result);
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    unsigned int arity = arg_arity;

    sprintf(query, "SELECT * FROM %s", YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2)));

    mysql_query(conn, query);
    res_set = mysql_store_result(conn);
    arity = mysql_num_fields(res_set); // get the number of column fields
    f_pred = YAP_MkFunctor(YAP_AtomOfTerm(YAP_ARG3), arity);
    f_assert = YAP_MkFunctor(YAP_LookupAtom("assert"), 1);
    while ((row = mysql_fetch_row(res_set)) != NULL)
    {
        for (int i = 0; i < arity; i++)
        { // test each column data type to...
            field = mysql_fetch_field(res_set);
            if (IS_SQL_INT(field->type))
            {
                t_args[i] = YAP_MkIntTerm((int)row[i]);
            }
            else if (IS_SQL_INT(field->type))
            {
                t_args[i] = YAP_MkFloatTerm(atof(row[i]));
            }
            else if (IS_SQL_INT(field->type))
            {
                t_args[i] = YAP_MkStringTerm(row[i]);
            }

            t_pred = YAP_MkApplTerm(f_pred, arity, t_args);
            t_assert = YAP_MkApplTerm(f_assert, 1, &t_pred);
            YAP_CallProlog(t_assert); // assert the row as a Prolog fact
        }
        mysql_free_result(res_set);
        return TRUE;
    }