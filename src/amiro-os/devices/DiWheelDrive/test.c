#include <stdio.h>
/*Do not change the order*/
typedef enum{
    WALL_AWAY = 1,  /* No wall found */
    WALL_FAR  = 2,  /* Wall too far from the sensos */
    WALL_SAFE = 3,  /* Wall at safe distance from sensor */
    WALL_NEAR = 4  /* Wall too close to sensor */
}wall_status_t;

/*Do not change the order*/
typedef enum{
    WF_CODE_NONE = 0,
    WF_CODE_WALL_AWAY = WALL_AWAY,   //1 
    WF_CODE_WALL_SAFE = WALL_SAFE,   //3 
    WF_CODE_WALL_FAR  = WALL_FAR,    //2 
    WF_CODE_WALL_NEAR = WALL_NEAR,   //4 
    WF_CODE_DOCK_LINE_FOUND = 5,     /* Sensor identified black dock indication line */
    WF_CODE_DOCK_LINE_NOT_FOUND = 6 /* Sensor couldn't find black dock indication line*/
}wf_state_codes_t;

void main()
{
    wall_status_t t = (wall_status_t) WF_CODE_WALL_NEAR;
    printf("%d\n", t);
}
