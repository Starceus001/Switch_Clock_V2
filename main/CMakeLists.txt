idf_component_register(SRCS "main.c"
                       "task/buttons_task.c"
                       "task/cores_task.c"
                       "task/display_task.c"
                       "task/ms_timer_task.c"
                       "task/rtc_task.c"
                       "task/switch_timer_task.c"
                       INCLUDE_DIRS "." "task")