ltl spec_alarm {
    [] (([](alarm_state == 1) && presence) -> <> [](buzzer == 1)) &&
    [] ([](alarm_state == 0) -> <> [](buzzer == 0)) &&
    [] (((alarm_state == 1) && code_ok) -> <> (alarm_state == 0)) &&
    [] (((alarm_state == 0) && code_ok) -> <> (alarm_state == 1))
}

ltl spec_light {
    [] (((state == 0) && buttonl) -> <> (state == 1)) &&
    [] ((state == 1) -> <> (state == 0))
}

#define timeout true


/* --------------- ALARM -----------------------*/

bit code_ok;

bit presence;
bit buzzer;

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


/* --------------- CODE ------------------------*/

active proctype code_fsm () {
    do
    :: code_ok = 1
    :: code_ok -> skip
    od
}


/* --------------- LIGHT -----------------------*/

bit buttonl;
byte state;

active proctype lampara_fsm () {
    state = 0;
    do
    :: (state == 0) -> atomic {
        if
        :: buttonl -> state = 1; buttonl = 0
        fi
    }
    :: (state == 1)  -> atomic {
        if
        :: buttonl -> state = 0; buttonl = 0
        :: timeout -> state = 0
        fi
    }
    od
}


/* --------------- ENTORNO ---------------------*/

active proctype entorno () {
    do
    :: if
       :: presence = 1
       :: buttonl = 1
       :: skip
       fi;
       printf ("ALARM: presence = %d, alarm_state = %d, buzzer = %d\n",
               presence, alarm_state, buzzer)
       printf ("LIGHT: buttonl = %d, state = %d\n",
               buttonl, state)
    od
}
