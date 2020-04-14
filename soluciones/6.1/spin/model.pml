ltl spec {
    [] (((state == 1) && presence) -> <> [](buzzer == 1)) &&
    [] ([](state == 0) -> <> [](buzzer == 0)) &&
    [] (((state == 1) && [](!armed)) -> <> [](state == 0)) &&
    [] (((state == 0) && [](armed)) -> <> [](state == 1))
}

bit armed;
bit presence;
bit buzzer;
byte state;

active proctype lampara_fsm () {
    state = 0;
    buzzer = 0;
    do
    :: (state == 0) -> atomic {
        if
        :: armed -> state = 1; buzzer = presence
        fi
    }
    :: (state == 1)  -> atomic {
        if
        :: !armed -> state = 0; buzzer = 0
        :: (armed && presence) -> buzzer = 1
        fi
    }
    od
}

proctype entorno () {
    do
    :: if
       :: armed = 0
       :: armed = 1
       :: presence = 1
       :: (!armed) -> skip
       fi;
       printf ("armed = %d, presence = %d, state = %d, buzzer = %d\n",
               armed, presence, state, buzzer)
    od
}
