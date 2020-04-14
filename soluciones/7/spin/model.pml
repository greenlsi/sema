ltl spec {
    [] (([](state.alarm == 1) && presence) -> <> [](buzzer == 1)) &&
    [] ([](state.alarm == 0) -> <> [](buzzer == 0)) &&
    [] (((state.alarm == 1) && code_ok) -> <> (state.alarm == 0)) &&
    [] (((state.alarm == 0) && code_ok) -> <> (state.alarm == 1))
}

#define timeout true

bit button;
bit presence;
bit buzzer;
bit code_ok;

typedef state_t {
    byte alarm;
    byte code;
};

state_t state;


active proctype alarm_fsm () {
    state.alarm = 0;
    do
    :: (state.alarm == 0) -> atomic {
        if
        :: code_ok -> state.alarm = 1; code_ok = 0; buzzer = 0
        fi
    }
    :: (state.alarm == 1)  -> atomic {
        if
        :: code_ok -> state.alarm = 0; code_ok = 0; buzzer = 0
        :: (!code_ok && presence) -> buzzer = 1; presence = 0
        fi
    }
    od
}

active proctype code_fsm () {
    state.code = 0;
    do
    :: (state.code == 0) -> atomic {
        if
        :: button -> state.code = 1; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 1) -> atomic {
        if
        :: button -> state.code = 0; button = 0
        :: timeout -> state.code = 2; button = 0
        fi
    }
    :: (state.code == 2) -> atomic {
        if
        :: button -> state.code = 3; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 3) -> atomic {
        if
        :: button -> state.code = 4; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 4) -> atomic {
        if
        :: button -> state.code = 0; button = 0
        :: timeout -> state.code = 5; button = 0
        fi
    }
    :: (state.code == 5) -> atomic {
        if
        :: button -> state.code = 6; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 6) -> atomic {
        if
        :: button -> state.code = 7; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 7) -> atomic {
        if
        :: button -> state.code = 8; button = 0
        :: timeout -> state.code = 0; button = 0
        fi
    }
    :: (state.code == 8) -> atomic {
        if
        :: button -> state.code = 0; button = 0
        :: timeout -> state.code = 0; code_ok = 1; button = 0
        fi
    }
    od
}

active proctype entorno () {
    do
    :: if
       :: button = 1
       :: presence = 1
       :: skip
       fi;
       printf ("button = %d, presence = %d, { state.code = %d, state.alarm = %d }, code_ok = %d, buzzer = %d\n",
               button, presence, state.code, state.alarm, code_ok, buzzer)
    od
}
