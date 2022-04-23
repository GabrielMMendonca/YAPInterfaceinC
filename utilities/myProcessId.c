#include "Yap/YapInterface.h"
#include <sys/types.h>
#include <unistd.h>
 
static int my_process_id(void)
{
     YAP_Term pid = YAP_MkIntTerm(getpid());
     YAP_Term out = YAP_ARG1;
     return(YAP_Unify(out,pid));
}
 
void init_my_predicates()
{
     YAP_UserCPredicate("my_process_id",my_process_id,1);
}

int main(int argc, char **argv)
{

}