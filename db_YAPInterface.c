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
    if (!YAP_Unify(arg_conn, YAP_MkIntTerm((long)conn)))
        return FALSE;
    return TRUE;
}

int c_db_query(void)
{
    MYSQL *connHandler = (MYSQL *)YAP_IntOfTerm(YAP_ARG1);
    char *query = YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2));
    
    mysql_query(connHandler, query);
    MYSQL_RES *result = mysql_store_result(connHandler);
    
    return (YAP_Unify(YAP_ARG3, YAP_MkIntTerm((long)result)));
}

int c_db_row(void)
{
    YAP_Term arg_result = YAP_ARG1;
    YAP_Term arg_listArgs = YAP_ARG2;

    int i = 0;
    MYSQL_ROW row;
    MYSQL_RES *res_set = (MYSQL_RES *)YAP_IntOfTerm(YAP_ARG1);
    MYSQL_FIELD *field;
    unsigned int arity = mysql_num_fields(res_set);
    

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

int c_db_disconnect(void)
{
    YAP_Term arg_conn = YAP_ARG1;
    MYSQL *conn = (MYSQL *)YAP_IntOfTerm(arg_conn);
    mysql_close(conn);
    return TRUE;
}

int c_db_import(void)
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
}

void init_my_predicates()
{
     YAP_UserCPredicate("c_db_connect",c_db_connect,5);
     YAP_UserCPredicate("c_db_query",c_db_query,3);
     YAP_UserBackCutCPredicate("c_db_row",c_db_row,2);
     YAP_UserCPredicate("c_db_import",c_db_import,5);
    //  YAP_UserCPredicate("c_db_arity",c_db_arity,5);//connhandler,RelationName, arity (devolve arity)
    //mysql_query(connhandler, "Select * from RelationName limit 0")
     YAP_UserCPredicate("disconnect",c_db_disconnect,5);
}

