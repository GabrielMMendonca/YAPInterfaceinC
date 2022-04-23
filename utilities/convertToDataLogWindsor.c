#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void WriteToFile(char dataRows[2 ^ 10])
{
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    fptr = fopen("C:\\program.txt", "w");

    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }

    fprintf(fptr, "%s", dataRows);
    fclose(fptr);
}

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv)
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "user12", "abcdefgh",
                           "windsordb", 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    MYSQL_RES *tablesResult = mysql_list_tables(con, "%");

    MYSQL_ROW tablesRow;
    MYSQL_ROW valuesRow;
    MYSQL_FIELD *field;
    char query[255];
    char selectString[255];
    char dataRows[2 ^ 10];

    MYSQL_FIELD *fields;

    while ((tablesRow = mysql_fetch_row(tablesResult)))
    {
        strcpy(selectString, "SELECT * FROM ");
        strcpy(query, strcat(selectString, tablesRow[0]));
        if (mysql_query(con, query))
        {
            finish_with_error(con);
        }

        MYSQL_RES *result = mysql_store_result(con);

        int num_fields = mysql_num_fields(result);
        while ((valuesRow = mysql_fetch_row(result)))
        {
            fields = mysql_fetch_fields(result);
            strcat(dataRows, tablesRow[0]);
            strcat(dataRows, "(");
            for (int j = 0; j < num_fields; j++)
            {
                if (j != 0)
                {
                    strcat(dataRows, ",");
                }

                if (!IS_NUM(valuesRow[j]))
                {
                    strcat(dataRows, "\'");
                    strcat(dataRows, valuesRow[j]);
                    strcat(dataRows, "\'");
                }
                else
                {
                    strcat(dataRows, valuesRow[j]);
                }
            }
            strcat(dataRows, ")");
            strcat(dataRows, "\n");
        }

        mysql_free_result(result);
        strcat(dataRows, "\n");
    }

    mysql_free_result(tablesResult);

    mysql_close(con);

    WriteToFile(&dataRows);

    exit(0);
}