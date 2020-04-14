ltl spec {
    [] (([](alarm_state == 1) && presence) -> <> [](buzzer == 1)) &&
    [] ([](alarm_state == 0) -> <> [](buzzer == 0)) &&
    [] (((alarm_state == 1) && code_ok) -> <> (alarm_state == 0)) &&
    [] (((alarm_state == 0) && code_ok) -> <> (alarm_state == 1))
}

bit presence;
bit buzzer;
bit code_ok;

byte alarm_state;


active proctype alarm_fsm () {
    alarm_state = 0;
    do
    :: (alarm_state == 0) -> atomic {
        if
        :: code_ok -> alarm_state = 1; code_ok = 0; buzzer = 0
        fi
    }
    :: (alarm_state == 1)  -> atomic {
        if
        :: code_ok -> alarm_state = 0; code_ok = 0; buzzer = 0
        :: (!code_ok && presence) -> buzzer = 1; presence = 0
        fi
    }
    od
}

active proctype code_fsm () {
    do
    :: code_ok = 1
    :: code_ok -> skip
    od
}

active proctype entorno () {
    do
    :: if
       :: presence = 1
       fi;
       printf ("presence = %d, alarm_state = %d, code_ok = %d, buzzer = %d\n",
               presence, alarm_state, code_ok, buzzer)
    od
}
