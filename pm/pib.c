#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct neat_policy_s {
    char *uid;
    int priority;
    bool replace_matched;
    char *filename;
    time_t time;
    int match;  //PropertyArray
    int properties; //PropertyMultiArray
} neat_policy_t ;

neat_policy_t *
neat_policy_init(){
    neat_policy_t *policy;
    policy->uid = "";   //TODO: Fix real uid init
    policy->priority = 0;
    policy->replace_matched = false;
    policy->filename = "";
    policy->time = time(NULL);
    //TODO: init match
    //TODO: init properties
    return policy;
}

int
main()
{
    neat_policy_t *policy = neat_policy_init();
    policy->priority = 1;
    return 0;
}
